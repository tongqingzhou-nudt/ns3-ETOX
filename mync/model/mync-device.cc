/*
 * Copyright (c) 2010 Yang CHI, CDMC, University of Cincinnati
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


#include "mync-codenum.h"
#include "mync-device.h"
#include "ns3/log.h"
#include "ns3/uinteger.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/pointer.h"
#include "ns3/wifi-mac-queue.h"
#include "ns3/mac-low.h"
#include <algorithm>
#include <stdlib.h>

NS_LOG_COMPONENT_DEFINE ("MyncDevice");

namespace ns3{
namespace mync{

MyncDevice::MyncDevice()
{
}

MyncDevice::~MyncDevice(){}

TypeId
MyncDevice::GetTypeId()
{
	static TypeId tid = TypeId ("ns3::cope::MyncDevice")
		.SetParent<NetDevice>()
		.AddConstructor<MyncDevice> ()
		.AddAttribute ("Mtu", "The Mac-level Maximum Transimission Unit",
						UintegerValue(0xffff),
						MakeUintegerAccessor (&MyncDevice::SetMtu,
											  &MyncDevice::GetMtu),
						MakeUintegerChecker<uint16_t> () )
		.AddTraceSource("MyncRx",
						"Packet received by NC device. Being forwarded. Non-promisc.",
						MakeTraceSourceAccessor(&MyncDevice::m_copeRxTrace))
		.AddTraceSource("MyncPromiscRx",
						"Packet received by NC device. Being forwarded. Promisc mode.",
						MakeTraceSourceAccessor(&MyncDevice::m_copePromiscRxTrace))
		;
	return tid;
}

void
MyncDevice::SetMyncProtocol(Ptr<MyncProtocol> cope)
{
	NS_LOG_FUNCTION_NOARGS();
	m_cope = cope;
}

Ptr<MyncProtocol>
MyncDevice::GetMyncProtocol() const
{
	NS_LOG_FUNCTION_NOARGS();
	return m_cope;
}

void
MyncDevice::SetMyncIfIndex (const uint32_t index)
{
	NS_LOG_FUNCTION(this<<index);
	m_copeIfIndex = index;
}

uint32_t
MyncDevice::GetMyncIfIndex () const
{
	NS_LOG_FUNCTION_NOARGS ();
	return m_copeIfIndex;
}


void
MyncDevice::SetIfIndex (const uint32_t index)
{
	NS_LOG_FUNCTION_NOARGS ();
	m_ifIndex = index;
}

uint32_t
MyncDevice::GetIfIndex () const
{
	NS_LOG_FUNCTION_NOARGS ();
	return m_ifIndex;
}

Ptr<Channel>
MyncDevice::GetChannel() const
{
	NS_LOG_FUNCTION_NOARGS ();
	return m_channel;
}

bool
MyncDevice::IsQueueFull() const
{
	Ptr<WifiNetDevice> wifiNetDevice = m_iface->GetObject<WifiNetDevice>();
	Ptr<RegularWifiMac> wifiMac = wifiNetDevice->GetMac()->GetObject<RegularWifiMac>();
	//Ptr<DcaTxop> txop = wifiMac->GetDcaTxop();
	PointerValue ptr;
	Ptr<DcaTxop> txop;
	wifiMac->GetAttribute("DcaTxop", ptr);
	txop = ptr.Get<DcaTxop>();
	//return txop->IsQueueFull();
	Ptr<WifiMacQueue> wifiMacQueue = txop->GetQueue();
	//return wifiMacQueue->IsFull();
	return (wifiMacQueue->GetSize() == wifiMacQueue->GetMaxSize());
}

uint32_t
MyncDevice::GetMacQueueSize() const
{
	Ptr<WifiNetDevice> wifiNetDevice = m_iface->GetObject<WifiNetDevice>();
	Ptr<RegularWifiMac> wifiMac = wifiNetDevice->GetMac()->GetObject<RegularWifiMac>();
	//Ptr<DcaTxop> txop = wifiMac->GetDcaTxop();
	//return txop->GetQueueSize();
	PointerValue ptr;
	wifiMac->GetAttribute("DcaTxop", ptr);
	Ptr<DcaTxop> txop = ptr.Get<DcaTxop>();
	Ptr<WifiMacQueue> wifiMacQueue = txop->GetQueue();
	return wifiMacQueue->GetSize();
}

Address
MyncDevice::GetAddress() const
{
	NS_LOG_FUNCTION_NOARGS ();
	return m_mac;
}

Ipv4Address
MyncDevice::GetIP() const
{
	return m_ip;
}

void
MyncDevice::SetAddress (Address address)
{
	NS_LOG_FUNCTION(this<<"Why should one ever call this function?");
	m_mac= Mac48Address::ConvertFrom(address);
}

bool
MyncDevice::SetMtu(const uint16_t mtu)
{
	NS_LOG_FUNCTION_NOARGS();
	m_mtu = mtu;
	return true;
}

uint16_t
MyncDevice::GetMtu() const
{
	NS_LOG_FUNCTION_NOARGS();
	return m_mtu;
}

bool
MyncDevice::IsLinkUp () const
{
	NS_LOG_FUNCTION_NOARGS();
	return true;
}

void
MyncDevice::AddLinkChangeCallback (Callback<void> callback)
{
	//do nothing
}

bool
MyncDevice::IsBroadcast() const
{
	NS_LOG_FUNCTION_NOARGS();
	return true;
}

Address
MyncDevice::GetBroadcast() const
{
	NS_LOG_FUNCTION_NOARGS();
	return Mac48Address ("ff:ff:ff:ff:ff:ff");
}

bool
MyncDevice::IsMulticast() const
{
	NS_LOG_FUNCTION_NOARGS();
	return true;
}

Address
MyncDevice::GetMulticast (Ipv4Address multicastGroup) const
{
	NS_LOG_FUNCTION(this << multicastGroup);
	Mac48Address multicast = Mac48Address::GetMulticast (multicastGroup);
	return multicast;
}

Address
MyncDevice::GetMulticast(Ipv6Address addr) const
{
	NS_LOG_FUNCTION (this << addr);
	return Mac48Address::GetMulticast(addr);
}

bool
MyncDevice::IsPointToPoint () const
{
	NS_LOG_FUNCTION_NOARGS();
	return false;
}

bool
MyncDevice::IsBridge() const
{
	NS_LOG_FUNCTION_NOARGS();
	return false;
}

bool
MyncDevice::Send(Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{
	NS_LOG_FUNCTION_NOARGS();
	const Mac48Address dest_mac = Mac48Address::ConvertFrom(dest);

	if(protocolNumber != Ipv4L3Protocol::PROT_NUMBER && protocolNumber != MyncProtocol::PROT_NUMBER)
	{
		m_iface->Send(packet, dest, protocolNumber);
		return true;
	}
	else
		return m_cope->Enqueue(packet, m_mac, dest_mac, protocolNumber, m_copeIfIndex, DATA);
}

bool
MyncDevice::SendFrom(Ptr<Packet> packet, const Address& src, const Address& dest, uint16_t protocolNumber)
{
	NS_LOG_FUNCTION(dest);
	const Mac48Address src_mac = Mac48Address::ConvertFrom(src);
	const Mac48Address dest_mac = Mac48Address::ConvertFrom(dest);
	if(protocolNumber != Ipv4L3Protocol::PROT_NUMBER && protocolNumber != MyncProtocol::PROT_NUMBER)
	{
		m_iface->Send(packet, dest, protocolNumber);
		return true;
	}
	else
		return m_cope->Enqueue(packet, src_mac, dest_mac, protocolNumber, m_copeIfIndex, DATA);
}

void
MyncDevice::ForwardDown(Ptr<Packet> packet, const Mac48Address & dest, uint16_t protocolNumber)
{
	NS_LOG_FUNCTION_NOARGS();
	m_iface->Send(packet, dest, protocolNumber);
}

Ptr<Node>
MyncDevice::GetNode() const
{
	NS_LOG_FUNCTION_NOARGS();
	return m_node;
}

void
MyncDevice::SetNode(Ptr<Node> node)
{
	NS_LOG_FUNCTION(this<<node);
	m_node = node;
}

bool
MyncDevice::NeedsArp() const
{
	NS_LOG_FUNCTION_NOARGS ();
	return true;
}

void
MyncDevice::SetReceiveCallback(NetDevice::ReceiveCallback cb)
{
	NS_LOG_FUNCTION_NOARGS();
	m_rxCallback = cb;
}

void
MyncDevice::SetPromiscReceiveCallback(NetDevice::PromiscReceiveCallback cb)
{
	NS_LOG_FUNCTION_NOARGS();
	m_promiscRxCallback = cb;
}

bool
MyncDevice::SupportsSendFrom() const
{
	NS_LOG_FUNCTION_NOARGS();
	return false;
}

void
MyncDevice::SetInterface (Ptr<NetDevice> iface)
{
	NS_LOG_FUNCTION_NOARGS();
	NS_ASSERT(iface != this);
	if(!Mac48Address::IsMatchingType(iface->GetAddress()))
		NS_FATAL_ERROR ("EUI 48 Address support required.");
	SetAddress(iface->GetAddress());
	m_cope->AddMac(m_mac);
	Ptr<WifiNetDevice> wifiNetDevice = iface->GetObject<WifiNetDevice>();
	if(wifiNetDevice == 0)
		NS_FATAL_ERROR("Device is not a Wifi NIC");

	Ptr<RegularWifiMac> wifiMac = wifiNetDevice->GetMac()->GetObject<RegularWifiMac>();
	//Ptr<DcaTxop> txop = wifiMac->GetDcaTxop();
	//txop->SetMaxQueueSize(10);
	PointerValue ptr;
	wifiMac->GetAttribute("DcaTxop", ptr);
	Ptr<DcaTxop> txop = ptr.Get<DcaTxop>();
	Ptr<WifiMacQueue> wifiMacQueue = txop->GetQueue();
	wifiMacQueue->SetMaxSize(10);

	Ptr<WifiPhy> phy = wifiNetDevice->GetPhy();
	m_channelNumber = phy->GetChannelNumber();
	m_node -> RegisterProtocolHandler (MakeCallback (&MyncDevice::ReceiveFromDevice, this), 0x0, iface, true);
	NS_LOG_FUNCTION_NOARGS();

	//m_ifaces.push_back(iface);
	m_iface = iface;
	//m_cope->Init();
}

void
MyncDevice::SetTxRatio(Ptr<TxRatio> txRatio)
{
	Ptr<WifiNetDevice> wifiNetDevice = m_iface->GetObject<WifiNetDevice>();
	if(!wifiNetDevice)
		NS_FATAL_ERROR("MyncDevice does not have a wifi device.");
	Ptr<RegularWifiMac> wifiMac = wifiNetDevice->GetMac()->GetObject<RegularWifiMac>();
	PointerValue ptr;
	wifiMac->GetAttribute("DcaTxop", ptr);
	Ptr<DcaTxop> txop = ptr.Get<DcaTxop>();
	Ptr<MacLow> macLow = txop->GetLow();
	macLow->SetTxRatio(txRatio);
}

uint16_t
MyncDevice::GetChannelNumber() const
{
	return m_channelNumber;
}

Ptr<NetDevice>
MyncDevice::GetInterface() const
{
	return m_iface;
}

void
MyncDevice::ReceiveFromDevice (Ptr<NetDevice> device, Ptr<const Packet> packet, uint16_t protocol, const Address& source, const Address& dest, PacketType packetType)
{
	NS_LOG_FUNCTION(this<<source<<dest<<protocol);
	/*
	packet->Print(std::cout);
	std::cout<<std::endl;
	fflush(stdout);
	*/
	Ptr<Packet> p = packet->Copy();
	if(protocol != Ipv4L3Protocol::PROT_NUMBER && protocol != MyncProtocol::PROT_NUMBER)
		ForwardUp(p, protocol, Mac48Address::ConvertFrom(source), Mac48Address::ConvertFrom(dest), packetType);
	else
		m_cope->Recv(device, p, protocol, source, dest, packetType, m_copeIfIndex);

}

void
MyncDevice::ForwardUp(Ptr<Packet> packet, uint16_t protocol, const Mac48Address & src, const Mac48Address & dest, PacketType packetType)
{
	NS_LOG_FUNCTION(this);
	if(protocol == Ipv4L3Protocol::PROT_NUMBER)
	{
		Ipv4Header ipHeader;
		packet->PeekHeader(ipHeader);
		/*
		ipHeader.Print(std::cout);
		std::cout<<std::endl;
		fflush(stdout);
		*/
	}
	std::vector<Mac48Address>::iterator iter;
	enum NetDevice::PacketType type;
	Ptr<Packet> packet_copy = packet->Copy();
	NS_LOG_FUNCTION(this<<src<<dest<<protocol<<packetType);

	if(dest.IsBroadcast())
		type = NetDevice::PACKET_BROADCAST;
	else if (dest.IsGroup())
		type = NetDevice::PACKET_MULTICAST;
	else if (dest == m_mac)
		type = NetDevice::PACKET_HOST;
	else
		type = NetDevice::PACKET_OTHERHOST;

	if(type != NetDevice::PACKET_OTHERHOST)
	{
		NS_LOG_LOGIC("MyncDevice recv callback");
		//NotifyRx(packet);
		m_rxCallback(this, packet_copy, protocol, src);

	}
	if(!m_promiscRxCallback.IsNull())
	{
		NS_LOG_LOGIC("MyncDevice promisc recv callback");
		//NotifyPromiscRx(packet);
		m_promiscRxCallback(this, packet_copy, protocol, src, dest, type);
	}
}

void
MyncDevice::NotifyRx(Ptr<const Packet> packet)
{
	m_copeRxTrace (packet);
}

void
MyncDevice::NotifyPromiscRx(Ptr<const Packet> packet)
{
	m_copePromiscRxTrace(packet);
}

void 
MyncDevice::SetUpIpFromWifi()
{
	NS_LOG_FUNCTION_NOARGS();
	Ptr<Ipv4> ipv4 = m_node->GetObject<Ipv4>();
	Ipv4Mask mask;
	int32_t interface = ipv4->GetInterfaceForDevice(m_iface);
	Ipv4InterfaceAddress ipv4InterAddr = ipv4->GetAddress(interface, 0);
	Ipv4Address ipv4Addr = ipv4InterAddr.GetLocal();
	mask = ipv4InterAddr.GetMask();
	m_ip = ipv4Addr;
	m_cope->AddIP(m_ip);
	SetIpv4Mask(mask);
}

void 
MyncDevice::SetUpIp()
{
	NS_LOG_FUNCTION_NOARGS();
	Ptr<Ipv4> ipv4 = m_node->GetObject<Ipv4>();
	Ipv4Mask mask;
	int32_t interface = ipv4->GetInterfaceForDevice(this);
	SetIpv4Interface(interface);
	Ipv4InterfaceAddress ipv4InterAddr = ipv4->GetAddress(interface, 0);
	Ipv4Address ipv4Addr = ipv4InterAddr.GetLocal();
	mask = ipv4InterAddr.GetMask();
	m_ip = ipv4Addr;
	m_cope->AddIP(m_ip);
	//m_cope->PrintAllAddress();
	SetIpv4Mask(mask);
	NS_LOG_DEBUG("mac: "<<m_mac<<" index in ipv4: "<<GetIfIndex());
	NS_LOG_DEBUG("ip: "<<ipv4Addr<<" interface index: "<<interface);
}

void 
MyncDevice::SetIpv4Mask(const Ipv4Mask & mask)
{
	m_mask = mask;
	m_cope->SetIpv4Mask(mask);
}

void
MyncDevice::SetIpv4Interface(uint32_t ipv4Interface)
{
	m_ipv4Interface = ipv4Interface;
}

uint32_t
MyncDevice::GetIpv4Interface() const
{
	return m_ipv4Interface;
}

CodeNum
MyncDevice::GetCodeNum() const
{
	return m_cope->GetCodeNum();
}

}//namespace cope
}//namespace ns3
