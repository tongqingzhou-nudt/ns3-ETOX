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

#ifndef ELSREPO_H
#define ELSREPO_H

#include "ns3/uinteger.h"
#include "ns3/ipv4-address.h"
#include <limits>
#include <list>

namespace ns3
{
namespace etox
{

typedef uint64_t NFMetric;
typedef uint64_t PMetric;

struct AddressNFEtoxStruct
{
	Ipv4Address address;
	NFMetric nf_etox;

	bool operator== (const struct AddressNFEtoxStruct& S2) const
	{
		return (address == S2.address);
	}
};
typedef struct AddressNFEtoxStruct NFPair;

const NFMetric INFINITE = std::numeric_limits<NFMetric>::max();

struct AdjNodeStruct
{
	uint32_t node;
	NFMetric weight;
};
typedef struct AdjNodeStruct AdjNode;

typedef std::list<AdjNode> NodeList;

//struct NodeDistanceHandlerStruct;

struct NodeDistancePairStruct
{
	uint32_t node;
	NFMetric distance;
	struct NodeDistancePairStruct * handler;

	bool operator< (const struct NodeDistancePairStruct & S2) const
	{
		return (distance < S2.distance);
	}

	bool operator> (const struct NodeDistancePairStruct & S2) const
	{
		return (distance > S2.distance);
	}

	struct NodeDistancePairStruct & operator= (NFMetric weight)
	{
		distance = weight;
		return *this;
	}

	struct NodeDistancePairStruct & operator= (struct NodeDistancePairStruct another)
	{
		distance = another.distance;
		node = another.node;
		handler = another.handler;
		return *this;
	}
};
typedef struct NodeDistancePairStruct NodeDistance;

/*
struct NodeDistanceHandlerStruct
{
	NodeDistance node_distance;
	NodeDistance * handler;

	struct NodeDistanceHandlerStruct & operator= (struct NodeDistanceHandlerStruct another)
	{
		node_distance = another.node_distance;
		handler = another.handler;
		return *this;
	}

	struct NodeDistanceHandlerStruct & operator= (NFMetric weight)
	{
		node_distance.distance = weight;
		return *this;
	}

	bool operator< (const struct NodeDistanceHandlerStruct & H2) const
	{
		return (node_distance < H2.node_distance);
	}

	bool operator> (const struct NodeDistanceHandlerStruct &H2) const
	{
		return (node_distance > H2.node_distance);
	}
};

typedef struct NodeDistanceHandlerStruct NDHandler;
*/
}//namespace etox
}//namsepace ns3
#endif
