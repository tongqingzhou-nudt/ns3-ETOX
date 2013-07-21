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

#ifndef ELSNODE_H
#define ELSNODE_H

#include "ns3/object.h"
#include "ns3/timer.h"

#include "link-state-repo.h"
#include "link-state-header.h"

#include <list>
#include <vector>

namespace ns3{
namespace etox{

class LinkStateNode : public Object
{
public:
	LinkStateNode(const double neighbor_interval);
	LinkStateNode(const double neighbor_interval, const Ipv4Address & addr);
	LinkStateNode(const double neighbor_interval, const Ipv4Address & addr, uint64_t time);
	~LinkStateNode();
	static TypeId GetTypeId();
	bool IsDuplicate(const HelloHeader & hello);
	void Update(const HelloHeader & hello);
	Ipv4Address GetAddress() const;
	void SetAddress(const Ipv4Address & address);
	uint64_t GetTime() const;
	std::vector<NFPair> GetPairs() const;
	void SetNeighbor();
	bool IsNeighbor() const;
	void AddMetric(const Ipv4Address & address, const NFMetric metric);
	NFMetric GetMetric(const Ipv4Address & address) const;
private:
	void Init();
	void NeighborTimerExpire();
private:
	Ipv4Address m_address;
	uint64_t m_time; //this is the time that m_pairs get updated
	std::vector<NFPair> m_pairs;
	Time m_neighborTimerInterval;
	Timer m_neighborTimer;
	bool m_neighbor;
};

/*
class LinkStateNeighbor : public Object
{
public:
	LinkStateNeighbor();
	~LinkStateNeighbor();
	static TypeId GetTypeId();

	NFMetric GetNF() const;
	Ipv4Address GetAddress() const;
	void SetNF(NFMetric nf);
private:
	void Init();
private:
	Ipv4Address m_address;
	NFMetric m_nf;
};

class LinkStateNeighborhood
{
public:
	LinkStateNeighborhood();
	~LinkStateNeighborhood();

	uint32_t GetNNeighbor() const;
	Ptr<LinkStateNeighbor> Get(uint32_t index) const;
	void Add(Ptr<LinkStateNeighbor> neighbor);
	Ptr<LinkStateNeighbor> Search(const Ipv4Address & add) const;
private:
	void Init();
private:
	std::vector<Ptr<LinkStateNeighbor> > m_neighbors;
};
*/

class LinkStateNetwork
{
public:
	LinkStateNetwork();
	~LinkStateNetwork();
	//typedef std::vector<LinkStateNode>::iterator NodeIterator;
	bool Have(const Ipv4Address & ip) const;
	bool Have(const HelloHeader & hello) const;
	//NodeIterator Search(const Ipv4Address & ip);
	Ptr<LinkStateNode> Search(const Ipv4Address & ip);
	bool AddNode(const Ipv4Address & address);//, uint64_t time);
	bool AddNode(const HelloHeader & hello);
	bool Remove(const Ipv4Address & addr);
	Ptr<LinkStateNode> Update(const HelloHeader & hello);
	//NodeIterator End();
	uint32_t GetNNode() const;
	Ptr<LinkStateNode> Get(uint32_t index) const;
	uint32_t Index(const Ipv4Address & addr) const;
	std::vector<Ptr<LinkStateNode> > GetNeighbors() const;
private:
	void Init();
	void PrintNodes() const;
private:
	std::vector<Ptr<LinkStateNode> > m_nodes;
	double m_neighborInterval;
};

}//namespace etox
}//namespace ns3

#endif
