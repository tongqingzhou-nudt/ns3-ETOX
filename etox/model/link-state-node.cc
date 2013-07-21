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

#include "link-state-node.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE("ETOXLinkStateNode");

namespace ns3{
namespace etox{

LinkStateNode::LinkStateNode(const double neighbor_interval)
	: m_neighborTimer(Timer::CANCEL_ON_DESTROY)
{
	m_time = 0;
	m_pairs.clear();
	m_neighborTimerInterval = Seconds(neighbor_interval);
	Init();
}

LinkStateNode::LinkStateNode(const double neighbor_interval, const Ipv4Address & addr)
	: m_address(addr), m_neighborTimer(Timer::CANCEL_ON_DESTROY)
{
	m_time = 0;
	m_pairs.clear();
	m_neighborTimerInterval = Seconds(neighbor_interval);
	Init();
}

LinkStateNode::LinkStateNode(const double neighbor_interval, const Ipv4Address & addr, uint64_t time)
	: m_address(addr), m_time(time), m_neighborTimer(Timer::CANCEL_ON_DESTROY)
{
	m_neighborTimerInterval = Seconds(neighbor_interval);
	Init();
}

LinkStateNode::~LinkStateNode()
{}

void
LinkStateNode::Init()
{
	m_neighbor = false;
	m_neighborTimer.SetDelay(m_neighborTimerInterval);
	m_neighborTimer.SetFunction(&LinkStateNode::NeighborTimerExpire, this);
}

TypeId
LinkStateNode::GetTypeId()
{
	static TypeId tid = TypeId("ns3::etox::LinkStateNode")
		.SetParent<Object>();
	return tid;
}

void
LinkStateNode::NeighborTimerExpire()
{
	NS_LOG_FUNCTION_NOARGS();
	m_neighbor = false;
	//m_neighborTimer.Schedule(m_neighborTimerInterval);
}

void
LinkStateNode::SetNeighbor()
{
	NS_LOG_FUNCTION_NOARGS();
	m_neighbor = true;
	m_neighborTimer.Cancel();
	m_neighborTimer.Schedule(m_neighborTimerInterval);
}

bool
LinkStateNode::IsNeighbor() const
{
	NS_LOG_FUNCTION(this<<m_neighbor);
	return m_neighbor;
}

bool
LinkStateNode::IsDuplicate(const HelloHeader & hello)
{
	NS_LOG_FUNCTION_NOARGS();
	return (m_time < hello.GetTime());
}

void
LinkStateNode::Update(const HelloHeader & hello)
{
	NS_LOG_FUNCTION_NOARGS();
	m_pairs.clear();
	m_pairs = hello.GetPairs();
	NS_ASSERT(m_pairs.size() == hello.GetNPairs());
	m_time = hello.GetTime();
}

Ipv4Address
LinkStateNode::GetAddress() const
{
	return m_address;
}

void
LinkStateNode::SetAddress(const Ipv4Address & addr)
{
	m_address = addr;
}

uint64_t
LinkStateNode::GetTime() const
{
	return m_time;
}

std::vector<NFPair>
LinkStateNode::GetPairs() const
{
	return m_pairs;
}

NFMetric
LinkStateNode::GetMetric(const Ipv4Address & address) const
{
	std::vector<NFPair>::const_iterator iter;
	for(iter = m_pairs.begin(); iter != m_pairs.end(); iter++)
	{
		if(iter->address == address)
			return iter->nf_etox;
	}
	return 0;
}

void
LinkStateNode::AddMetric(const Ipv4Address & address, const NFMetric metric)
{
	NS_LOG_FUNCTION(this<<m_pairs.size()<<address<<metric);
	std::vector<NFPair>::iterator iter;
	for(iter = m_pairs.begin(); iter != m_pairs.end(); iter++)
	{
		if(iter->address == address)
		{
			iter->nf_etox = metric;
			return;
		}
	}
	NFPair pair = {address, metric};
	m_pairs.push_back(pair);
}

//========Network Seperator=================

LinkStateNetwork::LinkStateNetwork()
{
	Init();
}

LinkStateNetwork::~LinkStateNetwork()
{}

void
LinkStateNetwork::Init()
{
	m_nodes.clear();
	m_neighborInterval = 500;
}

std::vector<Ptr<LinkStateNode> >
LinkStateNetwork::GetNeighbors() const
{
	NS_LOG_FUNCTION_NOARGS();
	std::vector<Ptr<LinkStateNode> > neighborSet;
	std::vector<Ptr<LinkStateNode> >::const_iterator iter;
	for(iter = m_nodes.begin(); iter != m_nodes.end(); iter++)
	{
		Ptr<LinkStateNode> node = *iter;
		if(node->IsNeighbor())
			neighborSet.push_back(node);
	}
	return neighborSet;
}

bool
LinkStateNetwork::Have(const Ipv4Address & ip) const
{
	std::vector<Ptr<LinkStateNode> >::const_iterator iter;
	for(iter = m_nodes.begin(); iter != m_nodes.end(); iter++)
	{
		if((*iter)->GetAddress() == ip)
			return true;
	}
	return false;
}

bool
LinkStateNetwork::Have(const HelloHeader & hello) const
{
	return Have(hello.GetAddress());
}

bool
LinkStateNetwork::AddNode(const Ipv4Address & address)
{
	NS_LOG_FUNCTION_NOARGS();
	if(Have(address))
		return false;
	Ptr<LinkStateNode> node = CreateObject<LinkStateNode>(m_neighborInterval, address);
	m_nodes.push_back(node);
	return true;
}

bool
LinkStateNetwork::AddNode(const HelloHeader & hello)
{
	NS_LOG_FUNCTION_NOARGS();
	if(Have(hello.GetAddress()))
		return false;
	//LinkStateNode node(hello.GetAddress(), hello.GetTime());
	//node.Update(hello);
	Ptr<LinkStateNode> node = CreateObject<LinkStateNode>(m_neighborInterval, hello.GetAddress(), hello.GetTime());
	node->Update(hello);
	m_nodes.push_back(node);
	return true;
}

bool
LinkStateNetwork::Remove(const Ipv4Address & addr)
{
	if(!Have(addr))
		return false;
	std::vector<Ptr<LinkStateNode> >::iterator iter;
	for(iter = m_nodes.begin(); iter != m_nodes.end(); )
	{
		if((*iter)->GetAddress() == addr)
		{
			iter = m_nodes.erase(iter);
			return true;
		}
		else
			iter++;
	}
	return false;
}

Ptr<LinkStateNode>
LinkStateNetwork::Update(const HelloHeader & hello)
{
	NS_LOG_FUNCTION(this<<" entering Network::Update "<<m_nodes.size());
	Ipv4Address senderAddr = hello.GetAddress();
	uint64_t sendingTime = hello.GetTime();
	//NodeIterator iter = Search(senderAddr);
	Ptr<LinkStateNode> node = Search(senderAddr);
	if (!node) //not found
	{
		AddNode(hello);
		node = Search(senderAddr);
		NS_ASSERT(node != NULL);
	}
	else //found, update if needed
	{
		if(sendingTime > node->GetTime())
			node->Update(hello);
	}
	NS_LOG_FUNCTION(this<<" exiting Network::Update "<<m_nodes.size());
	//PrintNodes();
	return node;
}

//LinkStateNetwork::NodeIterator
Ptr<LinkStateNode>
LinkStateNetwork::Search(const Ipv4Address & ip) 
{
	std::vector<Ptr<LinkStateNode> >::iterator iter;
	for(iter = m_nodes.begin(); iter != m_nodes.end(); iter++)
	{
		if((*iter)->GetAddress() == ip)
			return *iter;
	}
	return NULL;
}

void
LinkStateNetwork::PrintNodes() const
{
	NS_LOG_FUNCTION(this<<Simulator::Now().GetSeconds());
	std::vector<Ptr<LinkStateNode> >::const_iterator iter;
	for(iter = m_nodes.begin(); iter != m_nodes.end(); iter++)
	{
		NS_LOG_LOGIC("Address: "<<(*iter)->GetAddress());
		Ptr<LinkStateNode> node = *iter;
		std::vector<NFPair> pairs = node->GetPairs();
		std::vector<NFPair>::const_iterator pairIter;
		for(pairIter = pairs.begin(); pairIter != pairs.end(); pairIter++)
		{
			NS_LOG_LOGIC("\t"<<pairIter->address<<" "<<pairIter->nf_etox);
		}
	}
}

/*
LinkStateNetwork::NodeIterator
LinkStateNetwork::End() 
{
	return m_nodes.end();
}
*/

uint32_t
LinkStateNetwork::GetNNode() const
{
	return m_nodes.size();
}

Ptr<LinkStateNode>
LinkStateNetwork::Get(uint32_t index) const
{
	if(index >= m_nodes.size())
		return NULL;
	return m_nodes[index];
}

uint32_t
LinkStateNetwork::Index(const Ipv4Address & addr) const
{
	uint32_t index;
	for(index = 0; index < m_nodes.size(); index++)
	{
		if(m_nodes[index]->GetAddress() == addr)
			break;
	}
	return index;
}

//==========Neighbor Seperator=============

/*
LinkStateNeighbor::LinkStateNeighbor()
{
	Init();
}

LinkStateNeighbor::~LinkStateNeighbor()
{}

void
LinkStateNeighbor::Init()
{
	m_nf = 0;
}

TypeId
LinkStateNeighbor::GetTypeId()
{
	static TypeId tid = TypeId ("ns3::etox::LinkStateNeighbor")
		.SetParent<Object>();
	return tid;
}

NFMetric
LinkStateNeighbor::GetNF() const
{
	return m_nf;
}

Ipv4Address
LinkStateNeighbor::GetAddress() const
{
	return m_address;
}

void
LinkStateNeighbor::SetNF(NFMetric nf)
{
	m_nf = nf;
}

//==========Neighborhood Seperator =========

LinkStateNeighborhood::LinkStateNeighborhood()
{
	Init();
}

LinkStateNeighborhood::~LinkStateNeighborhood()
{}

void
LinkStateNeighborhood::Init()
{
	m_neighbors.clear();
}

uint32_t
LinkStateNeighborhood::GetNNeighbor() const
{
	return m_neighbors.size();
}

Ptr<LinkStateNeighbor>
LinkStateNeighborhood::Get(uint32_t index) const
{
	return m_neighbors[index];
}

Ptr<LinkStateNeighbor>
LinkStateNeighborhood::Search(const Ipv4Address & add) const
{
	std::vector<Ptr<LinkStateNeighbor> >::const_iterator iter;
	for(iter = m_neighbors.begin(); iter != m_neighbors.end(); iter++)
	{
		if ((*iter)->GetAddress() == add)
			return *iter;
	}
	return NULL;
}

void
LinkStateNeighborhood::Add(Ptr<LinkStateNeighbor> neighbor)
{
	m_neighbors.push_back(neighbor);
}
*/

}
}
