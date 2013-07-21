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

#ifndef BHEAP_H
#define BHEAP_H

#include "ns3/uinteger.h"
#include "link-state-repo.h"
#include <vector>

namespace ns3{
namespace etox{

//This is a Min-Heap, not a Max-Heap!!
class BinaryHeap
{
public:
	//static const NFMetric INFINITE;
	BinaryHeap(NodeDistance * data, uint32_t length);
	~BinaryHeap();

	void Heapify(const uint32_t i);
	void Build();
	void Rebuild();
	uint32_t Parent(const uint32_t i) const { return (i-1)/2; }
	inline uint32_t Left(const uint32_t i) const { return i*2+1; }
	inline uint32_t Right(const uint32_t i) const { return (i+1)*2; }
	NFMetric Minimum() const;
	uint32_t ExtractMin();
	void Decrease(const uint32_t index, const NFMetric key);
	void Insert(const NFMetric key);
	bool IsEmpty() const; //examine if m_heapSize is zero
	void Print() const;
private:
	void Swap(const uint32_t i, const uint32_t j);
private:
	//std::vector<NDHandler> m_handlers;
	NodeDistance * m_handlers;
	uint32_t m_heapSize;
	uint32_t m_length;
};

}
}
#endif
