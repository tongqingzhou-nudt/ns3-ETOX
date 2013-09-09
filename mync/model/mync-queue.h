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

#ifndef MYNCQUEUE_H
#define MYNCQUEUE_H

#include "ns3/packet.h"
#include "ns3/wifi-mac-header.h"
#include "ns3/ipv4-routing-protocol.h"
#include <deque>
#include <list>
#include "mync-header.h"
#include "mync-hash.h"
#include "mync-repo.h"
#include "ns3/log.h"

namespace ns3{
namespace mync{

class MyncQueueEntry
{
public:
	MyncQueueEntry();
	MyncQueueEntry(Ptr<Packet> packet);
	~MyncQueueEntry();
	void SetPacketId(uint32_t id);
	inline uint32_t GetPacketId() const { return m_packetId; }
	inline uint32_t Size() const {return m_packet->GetSize(); }
	inline void SetPacket(Ptr<Packet> packet) { m_packet = packet->Copy(); }
	Ptr<Packet> GetPacket() const;
	inline bool operator< (const MyncQueueEntry& entry) const { return m_packetId < entry.m_packetId; }
	MyncQueueEntry& operator= (const MyncQueueEntry& ent);
	inline void SetProtocolNumber(const uint16_t num) { m_protocolNumber = num; }
	inline uint16_t GetProtocolNumber() const { return m_protocolNumber; }

	void SetSrcMac(const Mac48Address & addr);
	Mac48Address GetSrcMac() const; 
	void SetDestMac(const Mac48Address & addr);
	Mac48Address GetDestMac() const;
	Ipv4Address GetIPSrc() const;
	void SetIPSrc(Ipv4Address addr);
	Ipv4Header GetIpHeader() const;
	void SetIpHeader(const Ipv4Header & header);
	void SetNexthop(const Ipv4Address & addr);
	void SetNexthop();
	Ipv4Address GetNexthop() const;

	/*
	void SetChannel(uint16_t channel);
	uint16_t GetChannel() const;
	*/
	void SetIface(uint32_t iface);
	uint32_t GetIface() const;
	void SetHello();
	void SetData();
	bool IsHello() const;
	bool HitMax() const;
	void Retry();
	PMetric GetOriginalTime() const;
	void SetOriginalTime(PMetric time);

private:
	void Init();
private:
	Ptr<Packet> m_packet;
	Mac48Address m_srcMac;
	Mac48Address m_destMac;
	Ipv4Address m_ipAddress; // src ip addr
	Ipv4Address m_ipDest; //dest ip addr
	Ipv4Header m_ipHeader;
	//uint16_t m_ipSeqNo;
	uint32_t m_packetId;
	uint16_t m_protocolNumber;
	//uint16_t m_channel;
	uint32_t m_iface;
	MessageType m_type;
	uint8_t m_retry; //number of rertansmission
	PMetric m_originalTime;
};

class MyncQueue
{
public:
	MyncQueue();
	~MyncQueue();
	bool EnqueueBack(const MyncQueueEntry & entry);
	bool EnqueueFront(const MyncQueueEntry & entry);
	bool Erase(uint32_t pid);
	/*
	bool Erase(Ipv4Address ip, uint16_t ipSeq);
	bool Erase(Mac48Address mac, uint16_t ipSeq);
	bool Erase(Ipv4Address ip, Mac48Address mac, uint16_t seq);
	void Clean(Mac48Address mac, uint16_t seq);
	void Clean(Ipv4Address ip, uint16_t seq);
	void Clean(Ipv4Address ip, Mac48Address mac, uint16_t seq);
	*/
	MyncQueueEntry Dequeue();
	MyncQueueEntry Front() const;
	inline uint32_t Size() const { return m_queue.size(); }
	MyncQueueEntry* LastPosition() ;
	MyncQueueEntry* FirstPosition() ;
	void Print(std::ostream &os) const;
	void SetMaxSize(uint32_t size);
private:
	std::list<MyncQueueEntry> m_queue;
	uint32_t m_max;
};

}//namespace cope
}//namespace ns3

#endif
