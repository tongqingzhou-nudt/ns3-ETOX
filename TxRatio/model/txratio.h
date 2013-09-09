/*
 * Copyright (c) 2012 Yang CHI, CDMC, University of Cincinnati
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

#ifndef TXRATIO_H
#define TXRATIO_H

#include "ns3/object.h"
#include "ns3/mac48-address.h"
#include <vector>
#include <fstream>

namespace ns3 {

class DestTxRatio 
{
public:
	DestTxRatio();
	DestTxRatio(Mac48Address mac);
	~DestTxRatio();
	void AddSend();
	void AddAck();
	double GetRatio() const;
	Mac48Address GetAddress() const;
	void PrintToLog() const;

private:
	void Init();

private:
	Mac48Address m_address;
	uint32_t m_sends;
	uint32_t m_acks;
};

class TxRatio : public Object
{
public:
	TxRatio();
	~TxRatio();
	static TypeId GetTypeId();
	void SetAddress(Mac48Address addr);
	Mac48Address GetAddress() const;
	double Ratio(Mac48Address dest);
	void AddSendTo(Mac48Address mac);
	void AddAckFrom(Mac48Address mac);
private:
	//typedef std::vector<DestTxRatio>::iterator DestIter;
	std::vector<DestTxRatio>::iterator GetDest(Mac48Address mac);
	//DestIter GetDest(Mac48Address mac) const;
private:
	Mac48Address m_address;
	std::vector<DestTxRatio> m_dests;
};

}//namespace ns3

#endif
