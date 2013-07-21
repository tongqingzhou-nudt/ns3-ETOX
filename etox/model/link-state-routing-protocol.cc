/*
 * Copyright (c) 2011 Yang CHI, CDMC, University of Cincinnati
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * Authors: Yang CHI <chiyg@mail.uc.edu>
 */

#include "link-state-routing-protocol.h"

#include "ns3/udp-socket-factory.h"
#include "ns3/inet-socket-address.h"
#include "ns3/log.h"
#include "ns3/names.h"
#include "ns3/mync-codenum.h"
#include "ns3/mync-repo.h"
#include "ns3/mync-neighbor.h"

#include <set>
#include <limits>
#include <algorithm>
#include <stdlib.h>
#include <limits>

NS_LOG_COMPONENT_DEFINE("ETOXLinkStateRouting");

namespace ns3{
namespace etox{

NS_OBJECT_ENSURE_REGISTERED(LinkStateRouting);

const uint32_t LinkStateRouting::ETOX_LINK_STATE_PORT = 713;
//const NFMetric LinkStateRouting::INF = std::numeric_limits<uint32_t>::max();
//double LinkStateRouting::m_confidence = 0.8;

TypeId
LinkStateRouting::GetTypeId()
{
	static TypeId tid = TypeId ("ns3::etox::LinkStateRouting")
		.SetParent<Ipv4RoutingProtocol>()
		.AddConstructor<LinkStateRouting> ()
		;
	return tid;
}

LinkStateRouting::LinkStateRouting()
{
	NS_LOG_FUNCTION(this);
	Init();
}

/*
LinkStateRouting::LinkStateRouting(const double hello_interval, uint8_t ttl)
	: m_maxTTL(ttl)
{
	m_helloTimerInterval = Seconds(hello_interval+rand()%10);
	Init();
}

LinkStateRouting::LinkStateRouting(uint8_t ttl)
	: m_maxTTL(ttl)
{
	Init();
}
*/

LinkStateRouting::LinkStateRouting(uint8_t ttl, double alpha)
	: m_maxTTL(ttl), m_confidence(alpha)
{
	Init();
}

LinkStateRouting::~LinkStateRouting()
{}

void
LinkStateRouting::Init()
{
	//m_maxTTL = 32;
	m_nftable.clear();
	m_ptable.clear();
	m_ipv4 = 0;
	m_helloTimer.SetFunction(&LinkStateRouting::LinkStateBroadcast, this);
	if(m_helloTimerInterval.GetSeconds())
		m_helloTimer.SetDelay(m_helloTimerInterval);
}

void
LinkStateRouting::SetHelloInterval(const double interval)
{
	m_helloTimerInterval = Seconds(interval+rand()%10);
	m_helloTimer.SetDelay(m_helloTimerInterval);
}

void
LinkStateRouting::DoStart()
{
	NS_LOG_FUNCTION(this<<m_confidence);
	Ipv4Address loopback("127.0.0.1");
	if (m_address == Ipv4Address())
	{
		NS_LOG_FUNCTION(this<<"DoStart"<<m_ipv4->GetNInterfaces());
		for(uint32_t i = 0; i< m_ipv4->GetNInterfaces(); i++)
		{
			Ipv4Address addr = m_ipv4->GetAddress(i, 0).GetLocal();
			NS_LOG_FUNCTION(this<<addr<<loopback);
			if(addr != loopback)
			{
				m_address = addr;
				break;
			}
		}
		NS_ASSERT(m_address != Ipv4Address());
	}
	NS_LOG_DEBUG("Starting ETOX link state routing at "<<m_address);
	
	bool canRun = false;
	for(uint32_t i = 0; i<m_ipv4->GetNInterfaces(); i++)
	{
		Ipv4Address addr = m_ipv4->GetAddress(i, 0).GetLocal();
		if(addr == loopback)
			continue;
		Ptr<Socket> socket = Socket::CreateSocket(GetObject<Node>(), UdpSocketFactory::GetTypeId());
		socket->SetAllowBroadcast(true);
		InetSocketAddress inetAddr(m_ipv4->GetAddress(i, 0).GetLocal(), ETOX_LINK_STATE_PORT);
		socket->SetRecvCallback(MakeCallback (&LinkStateRouting::Receive, this));
		if(socket->Bind(inetAddr))
		{
			NS_FATAL_ERROR("Failed to bind socket");
		}
		socket->BindToNetDevice(m_ipv4->GetNetDevice(i));
		m_socketAddresses[socket] = m_ipv4->GetAddress(i, 0);
		canRun = true;
	}
	if(canRun)
	{
		//Start timers and related functions
		NS_LOG_DEBUG("ETOX Link State at "<<m_address <<" started");
		m_helloTimer.Schedule();
	}

	//Add myself into network object
	m_network.AddNode(m_address);
	m_self = m_network.Search(m_address);
	NS_ASSERT(m_self != NULL);

	//Connect to myncDevice:
	m_ncProtocol = GetObject<Node>()->GetObject<mync::MyncProtocol>();
	NS_LOG_DEBUG("Set TxRatio");
	if(m_ncProtocol->GetTxRatio())
		SetTxRatio(m_ncProtocol->GetTxRatio());
	else
		NS_LOG_DEBUG("TxRatio empty from MyncProtocol");
	//Ptr<mync::MyncDevice> myncDevice = GetObject<Node>()->GetObject<mync::MyncDevice>();
	//Ptr<mync::MyncProtocol> myncProtocol = myncDevice->GetMyncProtocol();
	//Ptr<mync::MyncProtocol> myncProtocol = GetObject<Node>()->GetObject<mync::MyncProtocol>();
	//myncProtocol->SaySth();
	/*
	Ptr<cope::CopeDevice> copeDevice = GetObject<Node>() ->GetObject<cope::CopeDevice>();
	copeDevice->SaySth();
	Ptr<cope::CopeProtocol> copeProtocol = copeDevice->GetProtocol();
	copeProtocol->SaySth();
	*/
}

void
LinkStateRouting::SetIpv4 (Ptr<Ipv4> ipv4)
{
	NS_LOG_FUNCTION_NOARGS();
	NS_ASSERT(ipv4 != 0);
	NS_ASSERT(m_ipv4 == 0);
	m_ipv4 = ipv4;
	//DoStart();
}

/*
void
LinkStateRouting::DoDispose()
{
	Ipv4RoutingProtocol::DoDispose();
}*/

void
LinkStateRouting::UpdateCodeNum()
{
	NS_LOG_FUNCTION_NOARGS();
	m_codeNum = m_ncProtocol->GetCodeNum();

	//path info
	std::vector<mync::PathInfo> pathInfo = m_codeNum.GetPathInfo();
	//std::cout<<"Print out PathInfo: "<<pathInfo.size()<<" at: "<<m_address<<"\n";
	std::vector<mync::PathInfo>::const_iterator pathInfoIter;
	for(pathInfoIter = pathInfo.begin(); pathInfoIter != pathInfo.end(); pathInfoIter++)
	{
		/*
		std::cout<<"Destination: "<<pathInfoIter->address;
		std::cout<<" LastHop: "<<pathInfoIter->lastHop;
		std::cout<<" Time: "<<pathInfoIter->time;
		std::cout<<std::endl;*/
		PathTableEntry pathEntry;
		pathEntry.destination = pathInfoIter->address;
		pathEntry.nexthop = pathInfoIter->lastHop;
		pathEntry.interface = 1;
		pathEntry.p_etox = pathInfoIter->time/pathInfoIter->packetSize;
		pathEntry.expired = false;
		std::map<Ipv4Address, PathTableEntry>::iterator iter = m_ptable.find(pathInfoIter->address);
		if(iter != m_ptable.end())
			m_ptable.erase(iter);
		m_ptable.insert(std::make_pair(pathInfoIter->address, pathEntry));
	}
	fflush(stdout);
	
	//nf info
	std::vector<mync::CodeNumInfo> codeNum = m_codeNum.GetCodeNum();
	std::vector<mync::CodeNumInfo>::const_iterator codeNumIter;
	for(codeNumIter = codeNum.begin(); codeNumIter != codeNum.end(); codeNumIter++)
	{
		Ipv4Address dest = codeNumIter->address;
		Mac48Address dest_mac;
		double ratio = 0.0;
		NS_LOG_DEBUG("dest IP: "<<dest);
		dest_mac = m_ncProtocol->GetNeighbors().GetMACFromIP(dest); 
		if(m_txRatio)
			ratio = m_txRatio->Ratio(dest_mac);
		/*
		mync::MyncNeighbors neighbors = m_ncProtocol->GetNeighbors();
		dest_mac = neighbors.GetMACFromIP(dest);
		//if(dest_mac != NULL)
			ratio = m_txRatio->Ratio(dest_mac);
		*/
		NS_LOG_DEBUG("Value of ratio: "<<(double)ratio);
		uint16_t codeNum = codeNumIter->codeNum;
		uint32_t packetSize = codeNumIter->packetSize;
		uint64_t time = codeNumIter->time;
		NFMetric metric = m_self->GetMetric(dest);
		NS_LOG_LOGIC("Time: "<<time<<" PacketSize: "<<packetSize<<" Num encoded: "<<codeNum);
		if(ratio - 0.0 < std::numeric_limits<double>::epsilon())
		{
			NS_LOG_INFO("Ratio is 0 "<<(double)ratio);
			metric = m_confidence*time/packetSize/codeNum + (1-m_confidence) * metric;
		}
		else
		{
			NS_LOG_INFO("Ratio is not 0 "<<(double)ratio);
			metric = m_confidence*time/ratio/packetSize/codeNum + (1-m_confidence) * metric;
		}
		m_self->AddMetric(dest, metric);
	}
}

void
LinkStateRouting::Receive(Ptr<Socket> socket)
{
	NS_LOG_FUNCTION(this<<m_address);
	Ptr<Packet> receivedPacket;
	Address sourceAddress;
	receivedPacket = socket->RecvFrom(sourceAddress);

	InetSocketAddress inetSocketAddress = InetSocketAddress::ConvertFrom(sourceAddress);
	Ipv4Address sourceIp = inetSocketAddress.GetIpv4();
	Ipv4Address receiverIp = m_socketAddresses[socket].GetLocal();
	NS_ASSERT(receiverIp != Ipv4Address());
	NS_LOG_DEBUG("ETOX link state routing at "<<m_address<<" received an ETOX packet from "<<sourceIp << " to "<<receiverIp);
	NS_ASSERT(inetSocketAddress.GetPort() == ETOX_LINK_STATE_PORT);
	/*
	receivedPacket->Print(std::cout);
	std::cout<<std::endl;
	fflush(stdout);
	*/
	Ptr<Packet> packet = receivedPacket;
	HelloHeader helloHeader;
	packet->RemoveHeader(helloHeader);

	if(helloHeader.GetAddress() == m_address)
	{
		NS_LOG_LOGIC("From myself: ditch it!");
		return;
	}

	//Update database
	Ptr<LinkStateNode> node = m_network.Update(helloHeader);
	bool isOriginal = helloHeader.IsOriginal();
	if(isOriginal)
		node->SetNeighbor();
	//NFMetric metric = 0;
	//need metric here!!
	UpdateCodeNum();
	//m_self->AddMetric(sourceIp, metric);

	//Calculate routing table
	CalculateTable();

	//if ttl != 0, forward
	uint8_t ttl = helloHeader.GetTTL();
	if(ttl > 0)
	{
		helloHeader.SetTTL(--ttl);
		ForwardHello(helloHeader);
	}
}

void
LinkStateRouting::ForwardHello(HelloHeader &hello)
{
	NS_LOG_FUNCTION(this<<m_address<<hello.GetAddress());
	hello.UnsetOriginal();
	SendHelloHeader(hello);
}

void
LinkStateRouting::SendHelloHeader(HelloHeader &hello)
{
	NS_LOG_FUNCTION(this<<hello<<hello.GetSerializedSize());
	Ptr<Packet> packet = Create<Packet>();
	packet->AddHeader(hello);
	std::map<Ptr<Socket>, Ipv4InterfaceAddress>::const_iterator iter;
	for(iter = m_socketAddresses.begin(); iter != m_socketAddresses.end(); iter++)
	{
		Ipv4Address bcast = iter->second.GetLocal().GetSubnetDirectedBroadcast(iter->second.GetMask());
		iter->first->SendTo(packet, 0, InetSocketAddress(bcast, ETOX_LINK_STATE_PORT));
	}
}

Ptr<Ipv4Route>
LinkStateRouting::RouteOutput (Ptr<Packet> p, const Ipv4Header &header, Ptr<NetDevice> oif, Socket::SocketErrno &sockerr)
{
	NS_LOG_FUNCTION(this<<oif<<m_address<<header.GetDestination());
	Ptr<Ipv4Route> rentry;
	Ipv4Address dest = header.GetDestination();
	PathTableEntry pathEntry;
	NFTableEntry nfEntry, nfOutEntry;
	bool foundPathEntry = false, foundNFEntry = false;;
	//bool found = false;

	if(LookupPathTable(dest, pathEntry))
	{
		uint32_t interface = pathEntry.interface;
		if(oif && m_ipv4->GetInterfaceForDevice(oif) != static_cast<int>(interface))
		{
			NS_LOG_DEBUG("ETOXLinkStateRouting: oif inderface is "<<m_ipv4->GetInterfaceForDevice(oif)<<" and interface in outEntry "<<interface);
			NS_LOG_DEBUG("ETOXLinkStateRouting: oif device: "<<oif<<" from m_ipv4: "<<m_ipv4->GetNetDevice(m_ipv4->GetInterfaceForDevice(oif)));
			//sockerr = Socket::ERROR_NOROUTETOHOST;
			//return rentry;
		}
		else
			foundPathEntry = true;
	}
	//else 
	if (LookupNFTable(dest, nfEntry))
	{
		foundNFEntry = true;
		Ipv4Address gateway;
		if(foundPathEntry)
		{
			gateway = pathEntry.nexthop;
		}
		bool foundSendEntry = FindSendEntry(nfEntry, nfOutEntry);
		if(!foundSendEntry)
		{
			//NS_FATAL_ERROR("FindSendEntry failed");
			NS_LOG_DEBUG("FindSendEntry failed");
			if(!foundPathEntry)
				NS_FATAL_ERROR("FindSendEntry failed");
		}
		uint32_t interface = nfOutEntry.interface;
		if(oif && m_ipv4->GetInterfaceForDevice(oif) != static_cast<int>(interface))
		{
			NS_LOG_DEBUG("ETOXLinkStateRouting: oif inderface is "<<m_ipv4->GetInterfaceForDevice(oif)<<" and interface in outEntry "<<interface);
			NS_LOG_DEBUG("ETOXLinkStateRouting: oif device: "<<oif<<" from m_ipv4: "<<m_ipv4->GetNetDevice(m_ipv4->GetInterfaceForDevice(oif)));
			//sockerr = Socket::ERROR_NOROUTETOHOST;
			//return rentry;
			if(!foundPathEntry)
			{
				sockerr = Socket::ERROR_NOROUTETOHOST;
				return rentry;
			}
		}
		if(foundSendEntry && foundPathEntry)
		{
			NS_LOG_LOGIC("Found both PathEntry and SendEntry");
			NFMetric pathPrehopMetric = m_self->GetMetric(pathEntry.nexthop);
			NFMetric nfPrehopMetric = m_self->GetMetric(nfOutEntry.prehop);
			NS_LOG_DEBUG("Before assert: "<<pathEntry.nexthop);
			NS_ASSERT(pathPrehopMetric);
			NS_ASSERT(nfPrehopMetric);
			NS_LOG_LOGIC("PathEntry "<<pathEntry.destination<<" "<<pathEntry.nexthop<<" "<<pathEntry.p_etox<<" "<<pathPrehopMetric);
			NS_LOG_LOGIC("SendEntry "<<nfOutEntry.destination<<" "<<nfOutEntry.prehop<<" "<<nfOutEntry.nf_etox<<" "<<nfPrehopMetric);
			if(pathPrehopMetric < nfPrehopMetric)
				interface = pathEntry.interface;
			else
				gateway = nfOutEntry.prehop;
		}
		else if(foundSendEntry)
		{
			NS_LOG_LOGIC("Found SendEntry but not PathEntry");
			gateway = nfOutEntry.prehop;
		}
		else if (foundPathEntry)
		{
			NS_LOG_LOGIC("Found PathEntry but not SendEntry");
			interface = pathEntry.interface;
		}
		else
		{
			NS_FATAL_ERROR("Can't find route with both Path and NF");
			sockerr = Socket::ERROR_NOROUTETOHOST;
			return rentry;
		}
		rentry = Create<Ipv4Route>();
		rentry->SetDestination(dest);
		uint32_t numOifAddresses = m_ipv4->GetNAddresses(interface);
		NS_ASSERT(numOifAddresses > 0);
		Ipv4InterfaceAddress ifAddr;
		if (numOifAddresses == 1)
			ifAddr = m_ipv4->GetAddress(interface, 0);
		else
			NS_FATAL_ERROR("IP aliasing not implemented");
		rentry->SetSource(ifAddr.GetLocal());
		//rentry->SetGateway(nfOutEntry.prehop);
		rentry->SetGateway(gateway);
		rentry->SetOutputDevice(m_ipv4->GetNetDevice(interface));
		sockerr = Socket::ERROR_NOTERROR;
		//found = true;
	}
	//else
	if(!foundPathEntry && !foundNFEntry)
	{
		NS_LOG_DEBUG("ETOX link state routing: No route to host "<<dest);
		sockerr = Socket::ERROR_NOROUTETOHOST;
	}
	return rentry;
}

bool
LinkStateRouting::LookupPathTable(const Ipv4Address &dest, PathTableEntry & outEntry) const
{
	NS_LOG_FUNCTION_NOARGS();
	std::map<Ipv4Address, PathTableEntry>::const_iterator iter = m_ptable.find(dest);
	if(iter == m_ptable.end() || iter->second.expired)
		return false;
	outEntry = iter->second;
	return true;
}

bool
LinkStateRouting::LookupNFTable(const Ipv4Address &dest, NFTableEntry &outEntry) const
{
	NS_LOG_FUNCTION(this<<m_address);
	std::map<Ipv4Address, NFTableEntry>::const_iterator iter = m_nftable.find(dest);
	if(iter == m_nftable.end())
		return false;
	outEntry = iter->second;
	return true;
}

bool
LinkStateRouting::FindSendEntry(const NFTableEntry & entry, NFTableEntry & outEntry) const
{
	NS_LOG_FUNCTION(this<<entry.destination<<m_address);
	//PrintRoutingTable();
	outEntry = entry;
	while(outEntry.destination != outEntry.prehop)
	{
		if (!LookupNFTable(outEntry.prehop, outEntry))
		{
			return false;
		}
		NS_LOG_DEBUG("Loop?: "<<outEntry.prehop);
	}
	return true;
}

bool
LinkStateRouting::RouteInput(Ptr<const Packet> p, const Ipv4Header &header, Ptr<const NetDevice> idev, UnicastForwardCallback ucb, MulticastForwardCallback mcb, LocalDeliverCallback lcb, ErrorCallback ecb)
{
	NS_LOG_FUNCTION(this<<idev);
	UpdateCodeNum();
	Ipv4Address dest = header.GetDestination();
	Ipv4Address src = header.GetSource();

	//self-originated packets
	if(IsMyAddress(src))
		return true;

	//Local delivery
	NS_ASSERT(m_ipv4->GetInterfaceForDevice(idev)>=0);
	uint32_t iif = m_ipv4->GetInterfaceForDevice(idev);
	if(m_ipv4->IsDestinationAddress(dest, iif))
	{
		if(!lcb.IsNull())
		{
			lcb(p, header, iif);
			return true;
		}
		else
			return false;
	}

	//Forwarding
	Ptr<Ipv4Route> rtentry;
	PathTableEntry pathEntry;
	NFTableEntry nfEntry, nfOutEntry;

	bool foundPathEntry = false, foundNFEntry = false, foundSendEntry = false;
	Ipv4Address gateway;
	uint32_t interface;

	foundPathEntry = LookupPathTable(dest, pathEntry);
	foundNFEntry = LookupNFTable(dest, nfEntry);
	if(foundNFEntry)
	{
		foundSendEntry = FindSendEntry(nfEntry, nfOutEntry);
	}
	if(foundSendEntry && foundPathEntry)
	{
		NFMetric pathPrehopMetric = m_self->GetMetric(pathEntry.nexthop);
		NFMetric nfPrehopMetric = m_self->GetMetric(nfOutEntry.prehop);
		NS_LOG_DEBUG("Before assert: "<<pathEntry.nexthop);
		NS_ASSERT(pathPrehopMetric);
		NS_ASSERT(nfPrehopMetric);
		if(pathPrehopMetric < nfPrehopMetric)
		{
			gateway = pathEntry.nexthop;
			interface = pathEntry.interface;
		}
		else
		{
			gateway = nfOutEntry.prehop;
			interface = nfOutEntry.interface;
		}
	}
	else if (foundSendEntry)
	{
		gateway = nfOutEntry.prehop;
		interface = nfOutEntry.interface;

	}
	else if(foundPathEntry)
	{
		gateway = pathEntry.nexthop;
		interface = pathEntry.interface;
	}
	else
	{
		NS_LOG_DEBUG("ETOX Link State Routing: RouteInput for dest="<<dest<<" not found.");
		return false;
	}
	rtentry = Create<Ipv4Route>();
	rtentry->SetDestination(dest);
	uint32_t numOifAddresses = m_ipv4->GetNAddresses(interface);
	NS_ASSERT(numOifAddresses > 0);
	Ipv4InterfaceAddress ifAddr;
	if(numOifAddresses == 1)
		ifAddr = m_ipv4->GetAddress(interface, 0);
	else
		NS_FATAL_ERROR("IP aliasing not implemented");
	rtentry->SetSource(ifAddr.GetLocal());
	rtentry->SetGateway(gateway);
	rtentry->SetOutputDevice(m_ipv4->GetNetDevice(interface));
	ucb(rtentry, p, header);
	return true;

	/*
	if(LookupPathTable(dest, pathEntry))
	{
		return false;
	}
	else if (LookupNFTable(dest, nfEntry))
	{
		bool foundSendEntry = FindSendEntry(nfEntry, nfOutEntry);
		if(!foundSendEntry)
			NS_FATAL_ERROR("ETOXLinkStateRouting::RouteInput FindSendEntry failed");
		rtentry = Create<Ipv4Route>();
		rtentry->SetDestination(dest);
		uint32_t interface = nfOutEntry.interface;
		uint32_t numOifAddresses = m_ipv4->GetNAddresses(interface);
		NS_ASSERT(numOifAddresses > 0);
		Ipv4InterfaceAddress ifAddr;
		if(numOifAddresses == 1)
			ifAddr = m_ipv4->GetAddress(interface, 0);
		else
			NS_FATAL_ERROR("IP aliasing not implemented");
		rtentry->SetSource(ifAddr.GetLocal());
		rtentry->SetGateway(nfOutEntry.prehop);
		rtentry->SetOutputDevice(m_ipv4->GetNetDevice(interface));
		ucb(rtentry, p, header);
		return true;
	}
	else
	{
		NS_LOG_DEBUG("ETOX Link State Routing: RouteInput for dest="<<dest<<" not found.");
		return false;
	}
	*/
}

bool
LinkStateRouting::IsMyAddress(const Ipv4Address & addr)
{
	std::map<Ptr<Socket>, Ipv4InterfaceAddress>::const_iterator iter;
	for(iter = m_socketAddresses.begin(); iter!=m_socketAddresses.end(); iter++)
	{
		Ipv4InterfaceAddress iface = iter->second;
		if(iface.GetLocal() == addr)
		{
			return true;
		}
	}
	return false;
}

void 
LinkStateRouting::NotifyInterfaceUp (uint32_t i)
{}
void 
LinkStateRouting::NotifyInterfaceDown (uint32_t i)
{}
void 
LinkStateRouting::NotifyAddAddress (uint32_t interface, Ipv4InterfaceAddress address)
{}
void 
LinkStateRouting::NotifyRemoveAddress (uint32_t interface, Ipv4InterfaceAddress address)
{}

void
LinkStateRouting::PrintRoutingTable() const
{
	NS_LOG_LOGIC("NF Table at node:"<<m_address);
	NS_LOG_LOGIC("Destination\t\tPreHop\t\tInterface\t\tNF-ETOX");
	std::map<Ipv4Address, NFTableEntry>::const_iterator iter;
	for(iter = m_nftable.begin(); iter != m_nftable.end(); iter++)
	{
		//NS_LOG_LOGIC(""<<iter->first << "\t\t"<<iter->second.prehop << "\t\t");
		if(Names::FindName(m_ipv4->GetNetDevice(iter->second.interface)) != "")
			NS_LOG_LOGIC(""<<iter->first << "\t\t"<<iter->second.prehop << "\t\t"<<Names::FindName(m_ipv4->GetNetDevice(iter->second.interface)) << "\t\t"<<iter->second.nf_etox);
		else
			NS_LOG_LOGIC(""<<iter->first << "\t\t"<<iter->second.prehop << "\t\t"<<iter->second.interface<<"\t\t"<<iter->second.nf_etox);
		//NS_LOG_LOGIC(""<<iter->second.nf_etox);
	}

	NS_LOG_LOGIC("Path Table: ");
	NS_LOG_LOGIC("Destinationt\t\tNexthop\t\tInterface\t\tP-ETOX\t\tExpired");
	std::map<Ipv4Address, PathTableEntry>::const_iterator path_iter;
	for(path_iter = m_ptable.begin(); path_iter != m_ptable.end(); path_iter++)
	{
		//NS_LOG_LOGIC(""<<path_iter->first<<"\t\t"<<path_iter->second.nexthop<<"\t\t");
		if(Names::FindName(m_ipv4->GetNetDevice(path_iter->second.interface)) != "")
			NS_LOG_LOGIC(""<<path_iter->first<<"\t\t"<<path_iter->second.nexthop<<"\t\t"<<Names::FindName(m_ipv4->GetNetDevice(path_iter->second.interface)) << "\t\t"<< path_iter->second.p_etox<<"\t\t");
		else
			NS_LOG_LOGIC(""<<path_iter->first<<"\t\t"<<path_iter->second.nexthop<<"\t\t"<<path_iter->second.interface << "\t\t"<< path_iter->second.p_etox<<"\t\t");
		if(path_iter->second.expired)
			NS_LOG_LOGIC("True");
		else
			NS_LOG_LOGIC("False");
	}
}

void
LinkStateRouting::PrintRoutingTable(Ptr<OutputStreamWrapper> stream) const
{
	std::ostream *os = stream->GetStream();
	*os << "NF Table at node:"<<m_address<<"\n";
	*os << "Destination\t\tPreHop\t\tInterface\t\tNF-ETOX\n";
	std::map<Ipv4Address, NFTableEntry>::const_iterator iter;
	for(iter = m_nftable.begin(); iter != m_nftable.end(); iter++)
	{
		*os << iter->first << "\t\t";
		*os << iter->second.prehop << "\t\t";
		if(Names::FindName(m_ipv4->GetNetDevice(iter->second.interface)) != "")
			*os << Names::FindName(m_ipv4->GetNetDevice(iter->second.interface)) << "\t\t";
		else
			*os << iter->second.interface << "\t\t";
		*os << iter->second.nf_etox << "\n";
	}

	*os << "Path Table: \n";
	*os << "Destinationt\t\tNexthop\t\tInterface\t\tP-ETOX\t\tExpired\n";
	std::map<Ipv4Address, PathTableEntry>::const_iterator path_iter;
	for(path_iter = m_ptable.begin(); path_iter != m_ptable.end(); path_iter++)
	{
		*os << path_iter->first << "\t\t";
		*os << path_iter->second.nexthop << "\t\t";
		if(Names::FindName(m_ipv4->GetNetDevice(path_iter->second.interface)) != "")
			*os << Names::FindName(m_ipv4->GetNetDevice(path_iter->second.interface)) << "\t\t";
		else
			*os << path_iter->second.interface << "\t\t";
		*os << path_iter->second.p_etox<<"\t\t";
		if(path_iter->second.expired)
			*os << "True" << "\n";
		else
			*os << "False" << "\n";
	}
}

void
LinkStateRouting::CalculateTable()
{
	NS_LOG_FUNCTION(this<<m_address<<m_network.GetNNode());
	//uint32_t myIndex = m_network.Index(m_address);
	//std::set<Ipv4Address> DoneSet;
	//DistanceVector Distance;
	/*
	uint32_t **matrix;
	AllocMatrix(matrix, m_network.GetNNode());
	AdjMatrixGenerator(matrix);

	DeallocMatrix(matrix, m_network.GetNNode());
	*/
	/*
	Matrix<NFMetric> matrix(m_network.GetNNode());
	AdjMatrixGenerator(matrix);
	*/

	AdjList adjList;
	AdjListGenerator(adjList);

	NS_LOG_DEBUG("print out AdjList at: "<<m_address);
	//adjList.Print();

	Dijkstra dijkstra(adjList);
	dijkstra.Calculate();
	std::vector<uint32_t> prehops = dijkstra.GetPrehops();
	std::vector<NodeDistance> distances = dijkstra.GetDistance();
	NS_LOG_DEBUG("length of distances and prehops "<<prehops.size()<<" "<<distances.size());
	for(uint32_t i = 0; i<prehops.size(); i++)
	{
		uint32_t node = distances[i].node;
		if(node >= m_network.GetNNode())
		{
			NS_LOG_DEBUG(""<<m_address<<" cannot add due to node index OOB "<<node);
			continue;
		}
		NFMetric distance = distances[i].distance;
		Ipv4Address dest_addr = m_network.Get(node)->GetAddress();
		if(prehops[node] >= m_network.GetNNode())
		{
			NS_LOG_DEBUG(""<<m_address<<" cannot add "<<dest_addr<<" due to prehop OOB "<<prehops[node]);
			continue;
		}
		Ipv4Address pre_addr;
		if(prehops[node] == 0)
			pre_addr = m_network.Get(node)->GetAddress();
		else
			pre_addr = m_network.Get(prehops[node])->GetAddress();
		NFTableEntry entry = {dest_addr, pre_addr, 1, distance};
		std::map<Ipv4Address, NFTableEntry>::iterator iter = m_nftable.find(dest_addr);
		if(iter != m_nftable.end())
			m_nftable.erase(iter);
		m_nftable.insert(std::make_pair(dest_addr, entry));
	}

	//Print routing table after calculate
	/*
	std::stringstream filename;
	filename << "etox.calculate."<<Simulator::Now().GetMicroSeconds()<<m_address;
	//Ptr<OutputStreamWrapper> debugFile = Create<OutputStreamWrapper>("etox.lookup", std::ios::out);
	Ptr<OutputStreamWrapper> debugFile = Create<OutputStreamWrapper>(filename.str(), std::ios::out);
	PrintRoutingTable(debugFile);
	*/
	NS_LOG_DEBUG(""<<m_address<<" prints out routing table at: "<<Simulator::Now().GetMicroSeconds());
}

/*
   void
   LinkStateRouting::AllocMatrix(uint32_t **&matrix, uint32_t row)
   {
	matrix = new uint32_t*[row];
	for(uint32_t i = 0; i< row; i++)
	{
		matrix[i] = new uint32_t[row];
	}
}

void
LinkStateRouting::DeallocMatrix(uint32_t **matrix, uint32_t size)
{
	for(uint32_t i = 0; i<size; i++)
		delete [] matrix[i];
	delete [] matrix;
}

void
LinkStateRouting::AdjMatrixGenerator(uint32_t **matrix)
{
	NS_LOG_FUNCTION_NOARGS();
	uint32_t scale = m_network.GetNNode();
	for(uint32_t i = 0; i < scale; i++)
	{
		Ptr<LinkStateNode> node = m_network.Get(i);
		std::vector<NFPair> pairs = node->GetPairs();
		std::vector<NFPair>::const_iterator iter;
		for(iter = pairs.begin(); iter != pairs.end(); iter++)
		{
			Ipv4Address adjAddr = iter->address;
			NFMetric adjNF = iter->nf_etox;
			uint32_t index = m_network.Index(adjAddr);
			NS_ASSERT(index < scale);
			matrix[i][index] = adjNF;
		}

	}
}

void
LinkStateRouting::AdjMatrixGenerator(Matrix<NFMetric> & matrix)
{
	NS_LOG_FUNCTION_NOARGS();
	uint32_t scale = m_network.GetNNode();
	for(uint32_t i = 0; i<scale; i++)
	{
		Ptr<LinkStateNode> node = m_network.Get(i);
		std::vector<NFPair> pairs = node->GetPairs();
		std::vector<NFPair>::const_iterator iter;
		for(iter = pairs.begin(); iter != pairs.end(); iter++)
		{
			Ipv4Address adjAddr = iter->address;
			NFMetric adjNF = iter->nf_etox;
			uint32_t index = m_network.Index(adjAddr);
			NS_ASSERT(index < scale);
			matrix.Set(i, index, adjNF);
		}
	}
}
*/
void
LinkStateRouting::AdjListGenerator(AdjList & list)
{
	NS_LOG_FUNCTION_NOARGS();
	uint32_t scale = m_network.GetNNode();
	for(uint32_t i = 0; i<scale; i++)
	{
		Ptr<LinkStateNode> node = m_network.Get(i);
		uint64_t time = node->GetTime();
		std::vector<NFPair> pairs = node->GetPairs();
		std::vector<NFPair>::const_iterator iter;
		std::list<AdjNode> tempList;
		for(iter = pairs.begin(); iter!=pairs.end(); iter++)
		{
			Ipv4Address adjAddr = iter->address;
			NFMetric adjNF = iter->nf_etox;
			Ptr<LinkStateNode> adjNode = m_network.Search(adjAddr);
			if(adjNode)
			{
				uint64_t adjNodeTime = adjNode->GetTime();
				NFMetric metricFromAdjNode = adjNode->GetMetric(m_network.Get(i)->GetAddress());
				if(metricFromAdjNode)
				{
					if(time < adjNodeTime)
						adjNF = metricFromAdjNode;
				}
			}
			if(m_network.Index(adjAddr) < m_network.GetNNode())
			{
				AdjNode adjNode = {m_network.Index(adjAddr), adjNF};
				tempList.push_back(adjNode);
			}
		}
		list.AddList(tempList);
	}
}

void
LinkStateRouting::LinkStateBroadcast()
{
	NS_LOG_FUNCTION_NOARGS();
	//std::vector<Ptr<LinkStateNode> > neighbors = m_network.GetNeighbors();
	HelloHeader header;
	header.SetTTL(m_maxTTL);
	header.SetOriginal();
	header.SetAddress(m_address);
	/*
	std::vector<Ptr<LinkStateNode> >::const_iterator node_iter;
	for(node_iter = neighbors.begin(); node_iter != neighbors.end(); node_iter++)
	{
		std::vector<NFPair> pairs = (*node_iter)->GetPairs();
		std::vector<NFPair>::const_iterator pair_iter;
		for(pair_iter = pairs.begin(); pair_iter != pairs.end(); pair_iter++)
		{
			header.AddPair(pair_iter->address, pair_iter->nf_etox);
		}
	}
	*/
	std::vector<NFPair> pairs = m_self->GetPairs();
	std::vector<NFPair>::const_iterator iter;
	for(iter = pairs.begin(); iter != pairs.end(); iter++)
	{
		header.AddPair(iter->address, iter->nf_etox);
	}
	header.SetTime();
	SendHelloHeader(header);
	m_helloTimer.Schedule();
}

void
LinkStateRouting::SetTxRatio(Ptr<TxRatio> txRatio)
{
	NS_LOG_FUNCTION(this<<txRatio);
	m_txRatio = txRatio;
}

//========Matrix Seperator==========
/*
template<typename T>
Matrix<T>::Matrix(const uint32_t size)
	: m_size(size), m_data(new T[size*size])
{
}

template<typename T>
Matrix<T>::~Matrix()
{
	delete [] m_data;
}

template<typename T>
void
Matrix<T>::Set(uint32_t row, uint32_t col, const T & value)
{
	m_data[row*m_size+col] = value;
}

template<typename T>
T const &
Matrix<T>::operator() (uint32_t row, uint32_t col) const
{
	return m_data[row*m_size+col];
}

//============DistanceVector Seperator-------------

DistanceVector::DistanceVector()
{}

DistanceVector::~DistanceVector()
{}

void
DistanceVector::SetDistance(const Ipv4Address & address, const NFMetric & distance)
{
	std::vector<NFPair>::iterator iter;
	for(iter = m_pairs.begin(); iter != m_pairs.end(); iter++)
	{
		if(iter->address == address)
		{
			iter->nf_etox = distance;
			return;
		}
	}
	NFPair pair = {address, distance};
	m_pairs.push_back(pair);
}

NFMetric
DistanceVector::GetDistance(const Ipv4Address & address) const
{
	std::vector<NFPair>::const_iterator iter;
	for(iter = m_pairs.begin(); iter!=m_pairs.end(); iter++)
		if(iter->address == address)
			return iter->nf_etox;
	//return LinkStateRouting::INF/3;
	return INFINITE/3;
}
*/
}//namespace etox
}//namespace ns3
