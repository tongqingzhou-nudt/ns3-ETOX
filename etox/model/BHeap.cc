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

#include "BHeap.h"
#include <limits>
#include <stdio.h>
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE("BinaryHeap");

namespace ns3{
namespace etox{

//const uint32_t BinaryHeap::INFINITE = std::numeric_limits<NFMetric>::max();

BinaryHeap::BinaryHeap(NodeDistance * data, uint32_t length)
	: m_length(length)
{
	/*
	m_handlers.clear();
	for(uint32_t i = 0; i < length; i++)
	{
		NodeDistance node_distance = {data[i].node, data[i].distance};
		NDHandler handler = {node_distance, (data+i)};
		m_handlers.push_back(handler);
	}
	*/
	m_handlers = new NodeDistance[length];
	for(uint32_t i = 0; i<length; i++)
	{
		m_handlers[i].distance = data[i].distance;
		m_handlers[i].node = data[i].node;
		m_handlers[i].handler = data+i;
		data[i].handler = m_handlers+i;
	}
}

BinaryHeap::~BinaryHeap()
{
	for(uint32_t i = 0; i<m_length; i++)
	{
		m_handlers[i].handler->handler = NULL;
	}
	delete [] m_handlers;
}

void
BinaryHeap::Heapify(uint32_t i)
{
	NS_LOG_FUNCTION(this<<i);
	uint32_t left = Left(i);
	uint32_t right = Right(i);
	uint32_t least;
	if((left < m_heapSize) && (m_handlers[left] < m_handlers[i]))
		least = left;
	else
		least = i;
	if ((right < m_heapSize) && (m_handlers[right] < m_handlers[least]))
		least = right;
	if (least!= i)
	{
		Swap(i, least);
		Heapify(least);
	}
}

void
BinaryHeap::Build()
{
	NS_LOG_FUNCTION(this<<m_length);
	m_heapSize = m_length;
	for(int32_t i = m_length/2; i>=0; i--)
		Heapify(i);
}

void
BinaryHeap::Rebuild()
{
	NS_LOG_FUNCTION(this<<m_heapSize<<m_length);
	for(int32_t i = m_heapSize/2; i>=0; i--)
		Heapify(i);
}

void
BinaryHeap::Print() const
{
	std::cout<<"Current length: "<<m_length<<" and heapSize: "<<m_heapSize<<std::endl;
	for(uint32_t i = 0; i<m_length; i++)
	{
		std::cout<<"NodeDistance "<<i<<": "<<m_handlers[i].node<<"/"<<m_handlers[i].distance<<" and handler: "<<m_handlers[i].handler->node<<"/"<<m_handlers[i].handler->distance<<std::endl;
	}
	fflush(stdout);
}

void
BinaryHeap::Swap(const uint32_t i, const uint32_t j)
{
	NS_LOG_FUNCTION(this<<i<<j);
	m_handlers[i].handler->handler = m_handlers+j;
	m_handlers[j].handler->handler = m_handlers+i;
	NodeDistance temp = m_handlers[i];
	m_handlers[i] = m_handlers[j];
	m_handlers[j] = temp;
}

NFMetric 
BinaryHeap::Minimum() const
{
	NS_LOG_FUNCTION_NOARGS();
	return m_handlers[0].distance;
}

uint32_t
BinaryHeap::ExtractMin()
{
	NS_LOG_FUNCTION(this<<m_heapSize<<m_length);
	if (m_heapSize < 1)
		//handle error
		return m_length;
	NodeDistance min = m_handlers[0];
	m_handlers[0] = m_handlers[--m_heapSize];
	Heapify(0);
	return min.node;
}

bool
BinaryHeap::IsEmpty() const
{
	NS_LOG_FUNCTION_NOARGS();
	return not m_heapSize;
}

void
BinaryHeap::Decrease(const uint32_t index, const NFMetric key)
{
	NS_LOG_FUNCTION_NOARGS();
	uint32_t current = index;
	if(key > m_handlers[current].distance)
		//handle error
		return;
	m_handlers[current] = key;
	while((current > 0) && (m_handlers[Parent(current)] > m_handlers[current]))
	{
		Swap(current, Parent(current));
		current = Parent(current);
	}
}

void
BinaryHeap::Insert(const NFMetric key)
{
	NS_LOG_FUNCTION_NOARGS();
	m_handlers[m_heapSize++] = INFINITE/3;
	Decrease(m_heapSize-1, key);
}

}
}
