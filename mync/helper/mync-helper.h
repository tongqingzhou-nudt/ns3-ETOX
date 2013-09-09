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

#ifndef MYNC_HELPER_H
#define MYNC_HELPER_H

#include "ns3/net-device-container.h"
#include "ns3/node-container.h"
#include "ns3/mync-device.h"
#include "ns3/mync-protocol.h"
#include <vector>

#include "ns3/txratio.h" //For txRatio

namespace ns3{

class MyncHelper
{
public:
	MyncHelper();
	~MyncHelper();
	NetDeviceContainer InstallProtocol(NodeContainer nodes, bool mr, double rttime, double retry, double hello) ;
	//NetDeviceContainer InstallProtocol(NodeContainer nodes, bool mr, double rttime, double hello) ;
	void StartProtocol();
	void SetTxRatio();

private:
	std::vector<Ptr<mync::MyncProtocol> > m_protocols;
};

}//namespace ns3
#endif
