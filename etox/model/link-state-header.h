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

#ifndef ELSHEADER_H
#define ELSHEADER_H

#include "ns3/header.h"
#include "ns3/ipv4-address.h"
#include "link-state-repo.h"

#include <vector>

namespace ns3{
namespace etox{

/*
struct AddressNFEtoxStruct
{
	Ipv4Address address;
	uint32_t nf_etox;
};

typedef AddressNFEtoxStruct NFPair;
*/

/*------------
 * Header Format:
 * | Address | Time | TTL | Original | NumOfPairs | NFPair | NFPair | ... | NFPair |
 */

enum Original{
	ORIGINAL = 1,
	FORWARDED = 0,
};

class HelloHeader : public Header
{
public:
	HelloHeader();
	virtual ~HelloHeader();
	static TypeId GetTypeId();
	virtual TypeId GetInstanceTypeId() const;
	virtual void Print(std::ostream &os) const;
	virtual void Serialize (Buffer::Iterator start) const;
	virtual uint32_t Deserialize (Buffer::Iterator start);
	virtual uint32_t GetSerializedSize (void) const;

	void SetTime();
	uint64_t GetTime() const;
	void SetTTL(uint8_t ttl);
	uint8_t GetTTL() const;
	void SetOriginal();
	void UnsetOriginal();
	bool IsOriginal() const;
	uint32_t GetNPairs() const;
	void AddPair(const Ipv4Address & ip, NFMetric nf_etox);
	void SetAddress(const Ipv4Address & addr);
	Ipv4Address GetAddress() const;
	std::vector<NFPair> GetPairs() const;

private:
	void Init();

private:
	std::vector<NFPair> m_pairs;
	uint32_t m_pairsNum;
	uint64_t m_time;
	Ipv4Address m_address;
	uint8_t m_ttl;
	Original m_original;
};

}//namespace etox
}//namespace ns3

#endif
