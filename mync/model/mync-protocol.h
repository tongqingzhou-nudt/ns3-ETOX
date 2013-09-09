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

#include "mync-repo.h"
#include "mync-codenum.h"
#include "mync-header.h"
#include "mync-packet-info.h"
#include "mync-queue.h"
#include "mync-neighbor.h"
#include "mync-packet-pool.h"
#include "mync-device.h"
#include <set>
#include <vector>
#include <map>
#include <cstdlib>
#include "ns3/mac48-address.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv4-l3-protocol.h"
#include "ns3/timer.h"
#include "ns3/net-device.h"

#include "ns3/txratio.h" //add TxRatio

#ifndef MYNCPROTOCOL_H
#define MYNCPROTOCOL_H

namespace ns3{
namespace mync{

/*
struct NICStruct
{
	Mac48Address mac;
	uint16_t channel;
};

typedef struct NICStruct NIC;
*/

class MyncDevice;

class MyncProtocol : public Object
{
public:
	static const uint16_t PROT_NUMBER;
	static TypeId GetTypeId();
	MyncProtocol();
	/*
	MyncProtocol(double rttime, double hello); 
	MyncProtocol(bool arq);
	MyncProtocol(bool arq, double rttime, double hello);
	*/
	MyncProtocol(bool arq, double rttime, double retrytime, double hello);
	~MyncProtocol();
	bool Encode(MyncQueueEntry & entry, Ptr<Packet> & packet, MyncHeader & myncHeader);
	Ptr<Packet> XOR(Ptr<const Packet> p1, Ptr<const Packet> p2);
	int64_t Decode(const MyncHeader & header, Ptr<Packet> & packet);
	void Retransmit();
	bool Enqueue(Ptr<Packet> packet, const Mac48Address& src, const Mac48Address& dest, const uint16_t protocolNumber, const uint32_t index, const MessageType type);

	std::vector<Ptr<MyncDevice> > GetDevices()  const;
	uint32_t CreateNDevices(uint32_t n);
	Ptr<MyncDevice> GetDevice(uint32_t index) const;
	void Recv(Ptr<NetDevice> netDevice, Ptr<const Packet> pkt, uint16_t protocol, const Address & sender, const Address & receiver, NetDevice::PacketType packetType, uint32_t index);
	
	void SetIpv4Mask(const Ipv4Mask & mask);
	bool IsUnicast(const Ipv4Address & ipaddr, const Ipv4Mask & mask) const;
	bool IsUnicast(const Ipv4Address & ipaddr) const;
	void AddIP(Ipv4Address & ip);
	Ipv4Address GetIP(uint32_t index) const;
	Ipv4Address GetIP() const;
	void DoSend(); 
	void TrySend();
	void TrySendSchedule() ;
	void Init();
	void AddAck(AckBlock ack);
	void AddMac(const Mac48Address & mac);
	void SetNode(Ptr<Node> node);
	Ptr<Node> GetNode() const;
	void PrintAllAddress() const;
	void DoSendEnd();
	void StartHello();
	CodeNum GetCodeNum() const;
	void SaySth() const;
	double GetProbability() const;
	MyncNeighbors GetNeighbors() const;

	//For TxRatio:
	void SetTxRatio(Ptr<TxRatio> txRatio);
	Ptr<TxRatio> GetTxRatio() const;

private:
	uint32_t Index(const Mac48Address & src) const;
	uint32_t Index(uint16_t channel) const;
	void SendHello();
	void HelloTimerExpire();
private:
	std::vector<Ptr<MyncDevice> > m_devices;
	std::vector<Ipv4Address> m_ips;
	std::vector<Mac48Address> m_macs;
	MyncNeighbors m_neighbors;
	MyncQueue m_queue; //Output queue
	MyncQueue m_rtqueue; //Retransmission queue
	Timer m_timer;
	Time m_rtimeout;
	Timer m_try;
	Time m_ttimeout;
	Timer m_helloTimer;
	Time m_helloInterval;
	MyncPacketInfo m_packetInfo;
	MyncPacketPool m_pool;
	std::vector<AckBlock> m_ackBlockList;
	bool m_isSending;
	Ipv4Mask m_mask;
	//bool m_arq;
	double m_rttime;
	bool m_mr;
	Ptr<Node> m_node;
	//std::vector<NIC> m_neighborNICs;
	std::vector<uint32_t> m_devicesIf;
	std::deque<uint32_t> m_recps;
	uint16_t m_maxReports;
	CodeNum m_codeNum;
	std::map<uint32_t, PMetric> m_originalTimes;

	//For TxRatio:
	Ptr<TxRatio> m_txRatio;
};


}//namespace mync
}//namesapce ns3

#endif
