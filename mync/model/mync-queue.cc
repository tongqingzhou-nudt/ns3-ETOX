/*
 * Copyright (c) 2010 Yang CHI, CDMC, University of Cincinnati
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

#include "mync-queue.h"

NS_LOG_COMPONENT_DEFINE("MyncQueue");

namespace ns3{
namespace mync{

MyncQueueEntry::MyncQueueEntry()
{
	Init();
	m_packet = Create<Packet>();
}
MyncQueueEntry::MyncQueueEntry(Ptr<Packet> packet)
{	
	Init();
	m_packet = packet->Copy();
}
MyncQueueEntry::~MyncQueueEntry(){}

void
MyncQueueEntry::Init()
{
	m_originalTime = 0;
	m_retry = 0;
}

MyncQueue::MyncQueue()
{
	m_queue.clear();
	m_max = 800;
}
MyncQueue::~MyncQueue()
{
}

bool
MyncQueue::EnqueueBack(const MyncQueueEntry & entry)
{
	if(m_queue.size() == m_max)
	{
		NS_LOG_FUNCTION(this<<"Queue full "<<m_queue.size());
		return false;
	}
	std::list<MyncQueueEntry>::iterator iter;
	for(iter = m_queue.begin(); iter != m_queue.end(); iter++)
		if(iter->GetPacketId() == entry.GetPacketId())
			return false;
	m_queue.push_back(entry);
	NS_LOG_FUNCTION(this<<m_queue.size()<<entry.GetPacketId());
	return true;
}

bool
MyncQueue::EnqueueFront(const MyncQueueEntry & entry)
{
	if(m_queue.size() == m_max)
	{
		NS_LOG_FUNCTION(this<<"Queue full "<<m_queue.size());
		return false;
	}
	std::list<MyncQueueEntry>::iterator iter;
	for(iter = m_queue.begin(); iter != m_queue.end(); iter++)
		if(iter->GetPacketId() == entry.GetPacketId())
			return false;
	m_queue.push_front(entry);
	NS_LOG_FUNCTION_NOARGS();
	return true;
}

MyncQueueEntry *
MyncQueue::LastPosition() 
{
	NS_LOG_FUNCTION_NOARGS();
	//return &(m_queue.at(m_queue.size() - 1));
	return &(m_queue.back());
}

MyncQueueEntry *
MyncQueue::FirstPosition() 
{
	NS_LOG_FUNCTION_NOARGS();
	//return &(m_queue.at(0));
	return &(m_queue.front());
}

MyncQueueEntry
MyncQueue::Dequeue()
{
	NS_LOG_FUNCTION_NOARGS();
	if(!m_queue.size())
		NS_FATAL_ERROR("Try dequeue from a empty list");
	MyncQueueEntry entry = m_queue.front();
	m_queue.pop_front();
	return entry;
}

MyncQueueEntry
MyncQueue::Front() const
{
	NS_LOG_FUNCTION_NOARGS();
	if(!m_queue.size())
		NS_FATAL_ERROR("Try dequeue from a empty list");
	return  m_queue.front();
}

bool
MyncQueue::Erase(uint32_t pid)
{
	NS_LOG_FUNCTION(this<<pid);
	//std::deque<MyncQueueEntry>::iterator iter;
	std::list<MyncQueueEntry>::iterator iter;
	for(iter = m_queue.begin(); iter!= m_queue.end(); iter++)
	{
		if(iter->GetPacketId() == pid)
		{
			m_queue.erase(iter);
			return true;
		}
	}
	return false;
}

/*
bool
MyncQueue::Erase(Ipv4Address ip, uint16_t ipSeq)
{
	NS_LOG_FUNCTION(this<<ip<<ipSeq);
	//std::deque<MyncQueueEntry>::iterator iter;
	std::list<MyncQueueEntry>::iterator iter;

	for(iter = m_queue.begin(); iter != m_queue.end(); iter++)
	{
		if(!iter->GetSequence())
			iter->SetSequence(iter->GetIpHeader().GetIdentification());
		if(iter->GetNexthop() == ip && iter->GetSequence() == ipSeq)
		{
			m_queue.erase(iter);
			return true;
		}
	}
	return false;
}

bool
MyncQueue::Erase(Mac48Address mac, uint16_t ipSeq)
{
	NS_LOG_FUNCTION(this<<mac<<ipSeq);
	//std::deque<MyncQueueEntry>::iterator iter;
	std::list<MyncQueueEntry>::iterator iter;

	for(iter = m_queue.begin(); iter != m_queue.end(); iter++)
	{
		if(iter->GetDestMac() == mac && iter->GetSequence() == ipSeq)
		{
			m_queue.erase(iter);
			return true;
		}
	}
	return false;
}

bool
MyncQueue::Erase(Ipv4Address ip, Mac48Address mac, uint16_t seq)
{
	NS_LOG_FUNCTION(this<<ip<<mac<<seq);
	//std::deque<MyncQueueEntry>::iterator iter;
	std::list<MyncQueueEntry>::iterator iter;

	for(iter = m_queue.begin(); iter != m_queue.end(); iter++)
	{
		if(iter->GetSequence() == seq && (iter->GetDestMac() == mac || iter->GetNexthop() == ip))
		{
			m_queue.erase(iter);
			return true;
		}
	}
	return false;
}
*/

/*
void
MyncQueue::Clean(Mac48Address mac, uint16_t seq)
{
	NS_LOG_FUNCTION(this<<mac<<seq<<m_queue.size());
	std::list<MyncQueueEntry>::iterator iter;
	for(iter = m_queue.begin(); iter != m_queue.end(); )
	{
		if(iter->GetDestMac() == mac && iter->GetSequence() < seq)
			iter = m_queue.erase(iter);
		else
			iter++;
	}
}

void
MyncQueue::Clean(Ipv4Address ip, uint16_t seq)
{
	NS_LOG_FUNCTION(this<<ip<<seq<<m_queue.size());
	std::list<MyncQueueEntry>::iterator iter;
	for(iter = m_queue.begin(); iter != m_queue.end(); )
	{
		if(iter->GetNexthop() == ip && iter->GetSequence() < seq)
			iter = m_queue.erase(iter);
		else
			iter++;
	}
}

void
MyncQueue::Clean(Ipv4Address ip, Mac48Address mac, uint16_t seq)
{
	NS_LOG_FUNCTION(this<<ip<<seq<<m_queue.size());
	std::list<MyncQueueEntry>::iterator iter;
	for(iter = m_queue.begin(); iter != m_queue.end(); )
	{
		if((iter->GetNexthop() == ip || iter->GetDestMac() == mac) && iter->GetSequence() < seq)
			iter = m_queue.erase(iter);
		else
			iter++;
	}
	NS_LOG_FUNCTION(this<<ip<<seq<<m_queue.size());
}
*/

/*
uint32_t
MyncQueueEntry::Hash(Ipv4Address address, uint32_t seq_no) const
{
	uint32_t addressInt = address.Get();
	uint64_t key = addressInt;
	key = (key << 32) + seq_no;
	key = (~key) + (key << 18);
	key = key ^ (key >> 31);
	key = key * 21;
	key = key ^ (key >> 11);
	key = key + (key << 6);
	key = key ^ (key >> 22);
	return (uint32_t) key;
}

uint32_t
MyncQueueEntry::Hash() const
{
	return Hash(m_ipHeader.GetSource(), m_ipSeqNo);
}
*/

void
MyncQueueEntry::SetSrcMac(const Mac48Address & addr)
{
	NS_LOG_FUNCTION(this);
	m_srcMac = addr;
}

void
MyncQueueEntry::SetDestMac(const Mac48Address & addr)
{
	NS_LOG_FUNCTION(this);
	m_destMac = addr;
}

Mac48Address
MyncQueueEntry::GetSrcMac() const
{
	return m_srcMac;
}

Mac48Address
MyncQueueEntry::GetDestMac() const
{
	return m_destMac;
}
Ipv4Address
MyncQueueEntry::GetIPSrc() const
{
	return m_ipAddress;
}

void
MyncQueueEntry::SetIPSrc(Ipv4Address addr)
{
	NS_LOG_FUNCTION(this);
	m_ipAddress = addr;
}

Ipv4Header
MyncQueueEntry::GetIpHeader() const
{
	return m_ipHeader;
}

void
MyncQueueEntry::SetIpHeader(const Ipv4Header & header)
{
	NS_LOG_FUNCTION(this);
	m_ipHeader = header;
	//m_ipSeqNo = header.GetIdentification();
	m_ipAddress = header.GetSource();
	m_ipDest = header.GetDestination();
}

void
MyncQueueEntry::SetNexthop()
{
	NS_LOG_FUNCTION(this);
	m_ipDest = m_ipHeader.GetDestination();
}

void
MyncQueueEntry::SetNexthop(const Ipv4Address & addr)
{
	m_ipDest = addr;
}

Ipv4Address
MyncQueueEntry::GetNexthop() const
{
	return m_ipDest;
}

Ptr<Packet>
MyncQueueEntry::GetPacket() const
{
	return m_packet;
}

/*
void
MyncQueueEntry::SetPacketId()
{
	NS_LOG_FUNCTION(this);
	m_packetId = Hash(m_ipAddress, m_ipSeqNo);
}
*/

void
MyncQueueEntry::SetPacketId(uint32_t id)
{
	NS_LOG_FUNCTION(this<<id);
	m_packetId = id;
}

void
MyncQueueEntry::SetHello()
{
	m_type = HELLO;
}

void
MyncQueueEntry::SetData()
{
	m_type = DATA;
}

bool
MyncQueueEntry::IsHello() const
{
	NS_LOG_FUNCTION_NOARGS();
	return (m_type == HELLO);
}

MyncQueueEntry&
MyncQueueEntry::operator= (const MyncQueueEntry& ent)
{
	this->m_packet = ent.m_packet;
	this->m_ipAddress= ent.m_ipAddress;
	this->m_ipDest = ent.m_ipDest;
	this->m_ipHeader = ent.m_ipHeader;
	this->m_packetId = ent.m_packetId;
	this->m_protocolNumber = ent.m_protocolNumber;
	this->m_srcMac = ent.m_srcMac;
	this->m_destMac = ent.m_destMac;
	this->m_iface = ent.m_iface;
	this->m_type = ent.m_type;
	this->m_retry = ent.m_retry;

	return *this;
}

bool
MyncQueueEntry::HitMax() const
{
	NS_LOG_FUNCTION(this<<(uint16_t)m_retry);
	return (m_retry == 3);
}

void
MyncQueueEntry::Retry()
{
	NS_LOG_FUNCTION(this<<(uint16_t)m_retry);
	m_retry++;
}

void
MyncQueue::Print(std::ostream &os) const
{
	NS_LOG_FUNCTION_NOARGS();
	os<<"Current queue size is "<<m_queue.size()<<std::endl;
	//std::deque<MyncQueueEntry>::const_iterator iter;
	std::list<MyncQueueEntry>::const_iterator iter;
	for(iter = m_queue.begin(); iter!= m_queue.end(); iter++)
	{
		os<<iter->GetPacketId()<<" "<<iter->GetDestMac()
			<< " "<<iter->GetNexthop()<<std::endl;
	}
}

/*
void
MyncQueueEntry::SetChannel(uint16_t channel)
{
	m_channel = channel;
}

uint16_t
MyncQueueEntry::GetChannel() const
{
	return m_channel;
}
*/

uint32_t
MyncQueueEntry::GetIface() const
{
	return m_iface;
}

void 
MyncQueueEntry::SetIface(uint32_t iface)
{
	NS_LOG_FUNCTION(this<<iface);
	m_iface = iface;
}

void
MyncQueue::SetMaxSize(uint32_t size)
{
	NS_LOG_FUNCTION(this<<size);
	m_max= size;
}

PMetric
MyncQueueEntry::GetOriginalTime() const
{
	return m_originalTime;
}

void
MyncQueueEntry::SetOriginalTime(PMetric time)
{
	m_originalTime = time;
}

}//namespace cope
}//namespace ns3
