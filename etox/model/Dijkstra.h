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

#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "BHeap.h"
#include "link-state-repo.h"
#include "ns3/output-stream-wrapper.h"
#include <vector>

namespace ns3{
namespace etox{

//class AdjList;
/*
class AdjNode
{
public:
	AdjNode(Ipv4Address addr, NFMetric metric);
	~AdjNode();
private:
	Ipv4Address m_address;
	NFMetric m_weight;
};
*/

class AdjList
{
public:
	AdjList();
	~AdjList();
	void AddList(NodeList list);
	uint32_t GetSize() const;
	NodeList GetList(uint32_t index) const;
	void Print(Ptr<OutputStreamWrapper> stream) const;
	void Print() const;
private:
	std::vector<NodeList> m_lists;
};


class Dijkstra
{
public:
	Dijkstra(const AdjList & adjList);
	~Dijkstra();
	void Calculate();
	std::vector<NodeDistance> GetDistance() const;
	std::vector<uint32_t> GetPrehops() const;
private:
	void Initialize();
	void Relax(const uint32_t nodeU, const uint32_t nodeV, const NFMetric weight);

private:
	//std::vector<NFMetric> m_distance;
	std::vector<uint32_t> m_prehop;
	AdjList m_adjList;
	NodeDistance * m_graph;
};

}
}

#endif
