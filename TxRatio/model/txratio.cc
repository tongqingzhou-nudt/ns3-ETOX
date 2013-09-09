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

#include "txratio.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE("TxRatio");

namespace ns3{

DestTxRatio::DestTxRatio()
{
	Init();
}
DestTxRatio::DestTxRatio(Mac48Address mac)
	: m_address(mac)
{
	Init();
}
DestTxRatio::~DestTxRatio(){}

void
DestTxRatio::Init()
{
	m_sends = 0;
	m_acks = 0;
}

void
DestTxRatio::AddSend()
{
	NS_LOG_FUNCTION_NOARGS();
	m_sends++;
	NS_ASSERT(m_sends >= m_acks);
	PrintToLog();
}

void
DestTxRatio::AddAck()
{
	NS_LOG_FUNCTION_NOARGS();
	m_acks++;
	NS_ASSERT(m_sends >= m_acks);
	PrintToLog();
}

double
DestTxRatio::GetRatio() const
{
	NS_LOG_FUNCTION(this<<' '<<m_acks<<' '<<m_sends<<' '<<(double)m_acks/m_sends);
	return (double)m_acks/m_sends;
}

Mac48Address
DestTxRatio::GetAddress() const
{
	return m_address;
}

void
DestTxRatio::PrintToLog() const
{
	std::fstream logfile;
	logfile.open("txratio.log", std::fstream::app);
	logfile<<m_sends<<' '<<m_acks<<std::endl;
	if(m_sends < m_acks)
		logfile<<"Sth is seriously wrong."<<std::endl;
	logfile.close();
}

//------------ TxRatio -----------

TxRatio::TxRatio()
{
	std::fstream logfile;
	logfile.open("txratio.log", std::fstream::out);
	logfile.close();
}
TxRatio::~TxRatio(){}

TypeId
TxRatio::GetTypeId()
{
	static TypeId tid = TypeId("ns3::TxRatio")
		.SetParent<Object>();
	return tid;
}

void
TxRatio::SetAddress(Mac48Address mac)
{
	m_address = mac;
}

Mac48Address
TxRatio::GetAddress() const
{
	return m_address;
}

std::vector<DestTxRatio>::iterator
TxRatio::GetDest(Mac48Address mac)
{
	NS_LOG_FUNCTION(this<<mac);
	std::vector<DestTxRatio>::iterator iter;
	for(iter = m_dests.begin(); iter != m_dests.end(); iter++)
	{
		if(iter->GetAddress() == mac)
			return iter;
	}
	return m_dests.end();
}

double
TxRatio::Ratio(Mac48Address dest)
{
	NS_LOG_FUNCTION(this<<dest);
	std::vector<DestTxRatio>::iterator iter = GetDest(dest);
	if(iter != m_dests.end())
	{
		NS_LOG_DEBUG("Found dest, and ratio is "<<(double)iter->GetRatio());
		return iter->GetRatio();
	}
	else
	{
		NS_LOG_DEBUG("Cannot found dest");
		return 0;
	}
}

void
TxRatio::AddSendTo(Mac48Address mac)
{
	NS_LOG_FUNCTION_NOARGS();
	std::vector<DestTxRatio>::iterator iter;
	for(iter = m_dests.begin(); iter != m_dests.end(); iter++)
	{
		if(iter->GetAddress() == mac)
		{
			iter->AddSend();
			return;
		}
	}
	DestTxRatio dest(mac);
	dest.AddSend();
	m_dests.push_back(dest);
}

void
TxRatio::AddAckFrom(Mac48Address mac)
{
	NS_LOG_FUNCTION_NOARGS();
	std::vector<DestTxRatio>::iterator iter;
	for(iter = m_dests.begin(); iter != m_dests.end(); iter++)
	{
		if(iter->GetAddress() == mac)
		{
			iter->AddAck();
			return;
		}
	}
	DestTxRatio dest(mac);
	dest.AddAck();
	m_dests.push_back(dest);
}

}//namespace ns3
