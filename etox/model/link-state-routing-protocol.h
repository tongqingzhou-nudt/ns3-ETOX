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

#ifndef ELSROUTING_H
#define ELSROUTING_H

#include <vector>
#include <list>

#include "ns3/ipv4.h"
#include "ns3/ipv4-routing-protocol.h"
#include "ns3/node.h"
#include "ns3/object.h"
#include "ns3/socket.h"
#include "ns3/timer.h"
#include "ns3/mync-device.h"
#include "ns3/mync-protocol.h"

#include "ns3/txratio.h"

#include "link-state-header.h"
#include "link-state-node.h"
#include "link-state-repo.h"
#include "Dijkstra.h"

namespace ns3{
namespace etox{

struct NFTableEntryStruct
{
	Ipv4Address destination;
	Ipv4Address prehop;
	uint32_t interface;
	NFMetric nf_etox;
};

typedef struct NFTableEntryStruct NFTableEntry;

struct PathTableEntryStruct
{
	Ipv4Address destination;
	Ipv4Address nexthop;
	uint32_t interface;
	PMetric p_etox;
	bool expired;
};

typedef struct PathTableEntryStruct PathTableEntry;

class LinkStateRouting;

/*
template <typename T>
class Matrix
{
public:
	Matrix(const uint32_t size);
	~Matrix();

	void Set(uint32_t row, uint32_t col, const T & value);
	T const & operator() (uint32_t row, uint32_t col) const;
private:
	const uint32_t m_size;
	T * m_data;
};
*/

/*
class DistanceVector
{
public:
	DistanceVector();
	~DistanceVector();
	void SetDistance(const Ipv4Address & address, const NFMetric & distance);
	NFMetric GetDistance(const Ipv4Address & address) const;
private:
	std::vector<NFPair> m_pairs;
};
*/

class LinkStateRouting : public Ipv4RoutingProtocol
{
public:
	static const uint32_t ETOX_LINK_STATE_PORT;
	//static const NFMetric INF;
	LinkStateRouting();
	//LinkStateRouting(const double hello_interval, uint8_t ttl);
	//LinkStateRouting(uint8_t ttl);
	LinkStateRouting(uint8_t ttl, double alpha = 0.8);
	virtual ~LinkStateRouting();
	static TypeId GetTypeId();

	//From Ipv4RoutingProtocol
	virtual Ptr<Ipv4Route> RouteOutput (Ptr<Packet> p, const Ipv4Header &header, Ptr<NetDevice> oif, Socket::SocketErrno &sockerr) ;
	virtual bool RouteInput  (Ptr<const Packet> p, const Ipv4Header &header, Ptr<const NetDevice> idev, 
                            UnicastForwardCallback ucb, MulticastForwardCallback mcb,
                            LocalDeliverCallback lcb, ErrorCallback ecb) ;
	virtual void NotifyInterfaceUp (uint32_t interface) ;
	virtual void NotifyInterfaceDown (uint32_t interface) ;
	virtual void NotifyAddAddress (uint32_t interface, Ipv4InterfaceAddress address) ;
	virtual void NotifyRemoveAddress (uint32_t interface, Ipv4InterfaceAddress address) ;
	virtual void SetIpv4 (Ptr<Ipv4> ipv4) ;
	virtual void PrintRoutingTable (Ptr<OutputStreamWrapper> stream) const ;

	//Not From Ipv4RoutingProtocol
	//void DoDispose();
	void PrintRoutingTable() const;
	bool LookupPathTable(const Ipv4Address &dest, PathTableEntry &outEntry) const;
	bool LookupNFTable(const Ipv4Address &dest, NFTableEntry &outEntry) const;
	bool FindSendEntry(const NFTableEntry & entry, NFTableEntry & outEntry) const;
	void SetHelloInterval(const double interval);

	//For TxRatio:
	void SetTxRatio(Ptr<TxRatio> txRatio);

protected:
	virtual void DoStart();//this is from Object. 
private:
	void Init();
	void Receive(Ptr<Socket> socket);
	bool IsMyAddress(const Ipv4Address & addr);
	void CalculateTable();
	//void AdjMatrixGenerator(uint32_t **matrix);
	//void AdjMatrixGenerator(Matrix<NFMetric> & matrix);
	void AdjListGenerator(AdjList & list);
	//void AllocMatrix(uint32_t **&matrix, uint32_t row);
	//void DeallocMatrix(uint32_t **matrix, uint32_t row);
	void LinkStateBroadcast();
	void ForwardHello(HelloHeader &hello);
	void SendHelloHeader(HelloHeader &hello);
	void UpdateCodeNum();

private:
	//std::vector<NFTableEntry> m_nftable;
	//std::vector<PathTableEntry> m_ptable;
	std::map<Ipv4Address, NFTableEntry> m_nftable;
	std::map<Ipv4Address, PathTableEntry> m_ptable;
	Ptr<Ipv4> m_ipv4;
	Ipv4Address m_address;
	std::map<Ptr<Socket>, Ipv4InterfaceAddress> m_socketAddresses;
	LinkStateNetwork m_network;
	Timer m_helloTimer;
	Time m_helloTimerInterval;
	uint8_t m_maxTTL;
	Ptr<mync::MyncProtocol> m_ncProtocol;
	Ptr<LinkStateNode> m_self;
	mync::CodeNum m_codeNum;
	//static double m_confidence;
	double m_confidence;

	//For TxRatio:
	Ptr<TxRatio> m_txRatio;
};

}//namespace etox
}//namespace ns3

#endif
