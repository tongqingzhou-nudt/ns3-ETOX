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

#include "link-state-helper.h"
#include "ns3/link-state-routing-protocol.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE("ETOXLinkStateHelper");

namespace ns3{

/*
ETOXLinkStateHelper::ETOXLinkStateHelper(const double hello_interval, uint8_t ttl)
	: m_helloInterval(hello_interval), m_ttl(ttl)
{};
*/
ETOXLinkStateHelper::ETOXLinkStateHelper(const double hello_interval, uint8_t ttl, double alpha)
	: m_helloInterval(hello_interval), m_ttl(ttl), m_confidence(alpha)
{};

ETOXLinkStateHelper::~ETOXLinkStateHelper(){};

ETOXLinkStateHelper*
ETOXLinkStateHelper::Copy() const
{
	return new ETOXLinkStateHelper (*this);
}

Ptr<Ipv4RoutingProtocol>
ETOXLinkStateHelper::Create(Ptr<Node> node) const
{
	NS_LOG_FUNCTION(this<<m_confidence);
	Ptr<etox::LinkStateRouting> routing = CreateObject<etox::LinkStateRouting>(m_ttl, m_confidence);
	//routing->Start();
	routing->SetHelloInterval(m_helloInterval);
	node->AggregateObject (routing);
	return routing;
}
}
