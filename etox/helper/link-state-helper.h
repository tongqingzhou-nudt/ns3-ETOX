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

#ifndef ELSHELPER_H
#define ELSHELPER_H

#include "ns3/ipv4-routing-helper.h"

namespace ns3{

class ETOXLinkStateHelper : public Ipv4RoutingHelper
{
public:
	//ETOXLinkStateHelper(const double hello_interval, uint8_t ttl);
	ETOXLinkStateHelper(const double hello_interval, uint8_t ttl, double alpha = 0.8);
	virtual ~ETOXLinkStateHelper();
	ETOXLinkStateHelper* Copy() const;
	virtual Ptr<Ipv4RoutingProtocol> Create (Ptr<Node> node) const;

private:
	ETOXLinkStateHelper &operator= (const ETOXLinkStateHelper &lsh2);
private:
	double m_helloInterval;
	uint8_t m_ttl;
	double m_confidence;
};

}

#endif 
