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

#ifndef MYNCREPO_H
#define MYNCREPO_H

#include "ns3/ipv4-address.h"
#include "ns3/link-state-repo.h"

namespace ns3{
namespace mync{

typedef etox::PMetric PMetric;
typedef etox::NFMetric NFMetric;

struct CodeNumInfoStruct
{
	Ipv4Address address;
	uint16_t codeNum;
	uint32_t packetSize;
	uint64_t time;
};

struct PathInfoStruct
{
	Ipv4Address address;
	Ipv4Address lastHop;
	uint32_t packetSize;
	uint64_t time;
};

struct LinkProbabilityStruct
{
	Ipv4Address address;
	double probability;
};

typedef struct CodeNumInfoStruct CodeNumInfo;
typedef struct PathInfoStruct PathInfo;
typedef struct LinkProbabilityStruct LinkProbability;

}
}

#endif
