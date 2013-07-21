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

#include "Dijkstra.h"
#include "ns3/log.h"
#include "ns3/simulator.h"

NS_LOG_COMPONENT_DEFINE("Dijkstra");

namespace ns3{
namespace etox{

Dijkstra::Dijkstra(const AdjList & adjList)
{
	m_adjList = adjList;
	m_prehop.clear();
}

Dijkstra::~Dijkstra()
{
	delete [] m_graph;
}

void
Dijkstra::Initialize()
{
	NS_LOG_FUNCTION_NOARGS();
	uint32_t size = m_adjList.GetSize();
	m_graph = new NodeDistance[size];
	for(uint32_t i = 0; i<size; i++)
	{
		m_prehop.push_back(size);
		m_graph[i].distance = INFINITE/3;
		m_graph[i].node = i;
		m_graph[i].handler = NULL;
	}
	//m_distance[0] = 0;
	m_graph[0].distance = 0;
}

void
Dijkstra::Relax(const uint32_t nodeU, const uint32_t nodeV, const NFMetric weight)
{
	NS_LOG_FUNCTION(this<<nodeU<<nodeV<<weight<<m_adjList.GetSize());
	if(m_graph[nodeV].distance > m_graph[nodeU].distance+weight)
	{
		NS_LOG_DEBUG("distances and weight: "<<m_graph[nodeV].distance<<" "<<m_graph[nodeU].distance<<" "<<weight);
		m_graph[nodeV].distance = m_graph[nodeU].distance+weight;
		if(m_graph[nodeV].handler)
		{
			m_graph[nodeV].handler->distance = m_graph[nodeV].distance;
		}
		else
			NS_FATAL_ERROR("Handler should not be NULL");
		m_prehop[nodeV] = nodeU;
	}
}

void
Dijkstra::Calculate()
{
	NS_LOG_FUNCTION_NOARGS();
	Initialize();
	BinaryHeap heap(m_graph, m_adjList.GetSize());
	heap.Build();
	while(!heap.IsEmpty())
	{
		uint32_t currentMin = heap.ExtractMin();
		NS_LOG_LOGIC("Extracted Min: "<<currentMin);
		NodeList currentList = m_adjList.GetList(currentMin);
		NodeList::const_iterator iter;
		for(iter = currentList.begin(); iter!= currentList.end(); iter++)
		{
			NFMetric weight = iter->weight;
			Relax(currentMin, iter->node, weight);
			//heap.Heapify(0);
			heap.Rebuild();
		}
		//heap.Print();
	}
}

std::vector<uint32_t>
Dijkstra::GetPrehops() const
{
	return m_prehop;
}

std::vector<NodeDistance>
Dijkstra::GetDistance() const
{
	std::vector<NodeDistance> result(m_graph, m_graph+m_adjList.GetSize());
	/*
	for(uint32_t i = 0; i<m_adjList.GetSize(); i++)
	{
		result.push_back(m_graph[i]);
	}*/
	return result;
}

//==========AdjNode Seperator============

/*
AdjNode::AdjNode(Ipv4Address addr, NFMetric metrix)
	: m_address(addr), m_weight(metric)
{}

AdjNode::~AdjNode()
{}
*/

//==========AdjList Seperator============

AdjList::AdjList()
{
	m_lists.clear();
}

AdjList::~AdjList(){}

void
AdjList::AddList(NodeList list)
{
	m_lists.push_back(list);
}

uint32_t
AdjList::GetSize() const
{
	return m_lists.size();
}

NodeList
AdjList::GetList(uint32_t index) const
{
	return m_lists[index];
}

void
AdjList::Print(Ptr<OutputStreamWrapper> stream) const
{
	std::ostream *os = stream->GetStream();
	std::vector<NodeList>::const_iterator iter;
	uint32_t counter = 0;
	*os << "Total number in AdjList: "<<m_lists.size()<<std::endl;
	for(iter = m_lists.begin(); iter != m_lists.end(); iter++)
	{
		*os << "Node "<<counter<<" have "<<iter->size()<<" neighbors : \n";
		NodeList list = *iter;
		NodeList::const_iterator listIter;
		for(listIter = list.begin(); listIter != list.end(); listIter++)
		{
			*os << "\t node: "<<listIter->node<<" weight: "<<listIter->weight<<std::endl;
		}
		counter++;
	}
}

void
AdjList::Print() const
{
	NS_LOG_FUNCTION_NOARGS();
	uint32_t counter = 0;
	std::vector<NodeList>::const_iterator iter;
	for(iter = m_lists.begin(); iter != m_lists.end(); iter++)
	{
		NS_LOG_LOGIC("Node "<<counter<<" have "<<iter->size()<<" neighbors:");
		NodeList list = *iter;
		NodeList::const_iterator listIter;
		for(listIter = list.begin(); listIter != list.end(); listIter++)
		{
			NS_LOG_LOGIC("\tnode: "<<listIter->node<<" weight: "<<listIter->weight);
		}
		counter++;
	}
}

}
}
