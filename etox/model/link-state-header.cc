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

#include "link-state-header.h"
#include "ns3/simulator.h"
#include "ns3/address-utils.h"
#include "ns3/packet.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE("ETOXLinkStateHeader");

namespace ns3{
namespace etox{

HelloHeader::HelloHeader()
{
	Init();
}

HelloHeader::~HelloHeader()
{}

void
HelloHeader::Init()
{
	m_pairsNum = 0;
	m_pairs.clear();
	m_time = 0;
	m_original = ORIGINAL;
}

TypeId
HelloHeader::GetTypeId(void)
{
	static TypeId tid = TypeId ("ns3::etox::HelloHeader")
		.SetParent<Header>()
		.AddConstructor<HelloHeader>()
		;
	return tid;
}

TypeId
HelloHeader::GetInstanceTypeId(void) const
{
	return GetTypeId();
}

void
HelloHeader::Print(std::ostream &os) const
{
	os << "generator address: "<< m_address;
	os << ", Current Time: " << m_time;
	os << ", Number of neighbors: " << m_pairsNum;
	std::vector<NFPair>::const_iterator iter;
	for(iter = m_pairs.begin(); iter != m_pairs.end(); iter++)
	{
		os << ", " << iter->address << ", " <<iter->nf_etox;
	}
	os << " TTL: "<<(uint32_t)m_ttl<<" Original: ";
	if(m_original == ORIGINAL)
		os << "TRUE ";
	else
		os << "FALSE ";
}

void
HelloHeader::Serialize(Buffer::Iterator start) const
{
	NS_ASSERT(m_pairsNum == m_pairs.size());
	WriteTo(start, m_address);
	start.WriteHtonU64 (m_time);
	start.WriteHtonU32 (m_pairsNum);
	std::vector<NFPair>::const_iterator iter;
	for(iter = m_pairs.begin(); iter != m_pairs.end(); iter++)
	{
		WriteTo(start, iter->address);
		start.WriteHtonU64(iter->nf_etox);
	}
	start.WriteU8(m_ttl);
	start.WriteU8((uint8_t)m_original);
}

uint32_t
HelloHeader::GetSerializedSize() const
{
	NS_ASSERT(m_pairsNum == m_pairs.size());
	return 32/8 //m_address
		+ 64/8 //m_time
		+ 32/8 //m_pairsNum
		//+(32/8+sizeof(NFMetric)/8)*(uint32_t)(m_pairs.size()) //m+pairs
		+(32/8+64/8)*(uint32_t)m_pairsNum
		+ 1 + 1; //m_ttl and m_original
}

uint32_t
HelloHeader::Deserialize (Buffer::Iterator start)
{
	Buffer::Iterator bufIter = start;
	ReadFrom(bufIter, m_address);
	m_time = bufIter.ReadNtohU64();
	m_pairsNum = bufIter.ReadNtohU32();
	m_pairs.clear();
	Ipv4Address address;
	NFMetric nf_etox;
	for(uint32_t i = 0; i < m_pairsNum; i++)
	{
		ReadFrom(bufIter, address);
		nf_etox = bufIter.ReadNtohU64();
		NFPair pair = {address, nf_etox};
		m_pairs.push_back(pair);
	}
	NS_ASSERT(m_pairsNum == m_pairs.size());
	m_ttl = bufIter.ReadU8();
	m_original = (Original)(bufIter.ReadU8());
	uint32_t dist = bufIter.GetDistanceFrom(start);
	NS_ASSERT(dist == GetSerializedSize());
	return dist;
}

void
HelloHeader::SetTime()
{
	m_time = Simulator::Now().GetMilliSeconds();
}

uint64_t
HelloHeader::GetTime() const
{
	return m_time;
}

void
HelloHeader::SetTTL(uint8_t ttl)
{
	m_ttl = ttl;
}

uint8_t
HelloHeader::GetTTL() const
{
	return m_ttl;
}

uint32_t
HelloHeader::GetNPairs() const
{
	NS_ASSERT(m_pairsNum == m_pairs.size());
	return m_pairsNum;
}

void
HelloHeader::SetOriginal()
{
	m_original = ORIGINAL;
}
void 
HelloHeader::UnsetOriginal()
{
	m_original = FORWARDED;
}

bool
HelloHeader::IsOriginal() const
{
	return m_original;
}

void
HelloHeader::AddPair(const Ipv4Address & ip, NFMetric nf_etox)
{
	NS_LOG_FUNCTION(this<<m_pairs.size()<<m_pairsNum);
	NFPair pair = {ip, nf_etox};
	m_pairs.push_back(pair);
	m_pairsNum++;
	NS_ASSERT(m_pairsNum == m_pairs.size());
}

void
HelloHeader::SetAddress(const Ipv4Address & addr)
{
	m_address = addr;
}

Ipv4Address
HelloHeader::GetAddress() const
{
	return m_address;
}

std::vector<NFPair>
HelloHeader::GetPairs() const
{
	return m_pairs;
}

}
}
