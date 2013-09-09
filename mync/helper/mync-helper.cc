/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
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
 * Author: Yang CHI <chiyg@mail.uc.edu>
 */

#include "mync-helper.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("MyncHelper");

namespace ns3{
MyncHelper::MyncHelper(){}
MyncHelper::~MyncHelper(){}

NetDeviceContainer
MyncHelper::InstallProtocol(NodeContainer nodes, bool mr, double rttime, double retry, double hello) 
//MyncHelper::InstallProtocol(NodeContainer nodes, bool mr, double rttime, double hello) 
{
	NetDeviceContainer devices;
	for(NodeContainer::Iterator iter = nodes.Begin(); iter!= nodes.End(); iter++)
	{
		Ptr<Node> node = *iter;
		Ptr<mync::MyncProtocol> protocol = CreateObject<mync::MyncProtocol>(mr, rttime, retry, hello);
		protocol->SetNode(node);
		node->AggregateObject(protocol);
		uint32_t ifNum = node->GetNDevices();
		uint32_t iface;
		protocol->CreateNDevices(ifNum);
		for(iface = 0; iface < ifNum; iface++)
		{
			Ptr<mync::MyncDevice> copeDevice = protocol->GetDevice(iface);
			copeDevice->SetInterface(node->GetDevice(iface));
			devices.Add(copeDevice);
		}
		for(iface = 0; iface < ifNum; iface++)
		{
			node->AddDevice(protocol->GetDevice(iface));
		}
		m_protocols.push_back(protocol);
	}
	return devices;
}

void
MyncHelper::StartProtocol()
{
	std::vector<Ptr<mync::MyncProtocol> >::iterator iter;
	for(iter = m_protocols.begin(); iter != m_protocols.end(); iter++)
	{
		(*iter)->StartHello();
	}
	//SetTxRatio();
}

void
MyncHelper::SetTxRatio()
{
	NS_LOG_FUNCTION_NOARGS();
	std::vector<Ptr<mync::MyncProtocol> >::iterator iter;
	for(iter = m_protocols.begin(); iter != m_protocols.end(); iter++)
	{
		Ptr<TxRatio> txRatio = Create<TxRatio>();
		(*iter)->SetTxRatio(txRatio);
	}
}

}//namespace ns3
