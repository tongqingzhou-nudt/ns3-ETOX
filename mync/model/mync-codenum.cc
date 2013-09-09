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
 * Authors: Yang CHI <chiyg@mail.uc.edu>
 */

#include "mync-codenum.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE("MyncCodeNum");

namespace ns3{
namespace mync{

CodeNum::CodeNum()
{
	m_codeNum.clear();
}
CodeNum::~CodeNum(){}

bool
CodeNum::FindCodeNum(const Ipv4Address & addr, CodeNumInfo & info) const
{
	std::vector<CodeNumInfo>::const_iterator iter;
	for(iter = m_codeNum.begin(); iter != m_codeNum.end(); iter++)
	{
		if(iter->address == addr)
		{
			info = *iter;
			return true;
		}
	}
	return false;
}

void
CodeNum::AddCodeNum(const Ipv4Address & addr, const uint16_t num, const uint32_t pktSize, const uint64_t time)
{
	NS_LOG_FUNCTION(this<<addr<<num<<pktSize<<time);
	CodeNumInfo info;
	if(FindCodeNum(addr, info))
	{
		info.packetSize = pktSize;
		info.codeNum = num;
		info.time = time;
		return;
	}
	info.address = addr;
	info.codeNum = num;
	info.packetSize = pktSize;
	info.time = time;
	m_codeNum.push_back(info);
}

bool
CodeNum::FindPathInfo(const Ipv4Address & address, PathInfo & info) const
{
	std::vector<PathInfo>::const_iterator iter;
	for(iter = m_pathInfo.begin(); iter != m_pathInfo.end(); iter++)
	{
		if (iter->address == address)
		{
			info = *iter;
			return true;
		}
	}
	return false;
}

void
CodeNum::AddPathInfo(const Ipv4Address & address, const Ipv4Address & last, const uint32_t pktSize, const uint64_t time)
{
	NS_LOG_FUNCTION(this<<address<<last<<time);
	PathInfo info;
	if(FindPathInfo(address, info))
	{
		info.lastHop = last;
		info.time = time;
		info.packetSize = pktSize;
		return;
	}
	info.address = address;
	info.lastHop = last;
	info.packetSize = pktSize;
	info.time = time;
	m_pathInfo.push_back(info);
}

std::vector<PathInfo>
CodeNum::GetPathInfo() const
{
	return m_pathInfo;
}

std::vector<CodeNumInfo>
CodeNum::GetCodeNum() const
{
	return m_codeNum;
}

std::vector<LinkProbability>
CodeNum::GetProbability() const
{
	return m_probability;
}

void 
CodeNum::AddProbability(const Ipv4Address & address, const double probability)
{
	NS_LOG_FUNCTION_NOARGS();
	LinkProbability link;
	if(FindProbability(address, link))
	{
		link.probability = probability;
		return;
	}
	link.address = address;
	link.probability = probability;
	m_probability.push_back(link);
}

bool
CodeNum::FindProbability(const Ipv4Address & address, LinkProbability & link) const
{
	NS_LOG_FUNCTION_NOARGS();
	std::vector<LinkProbability>::const_iterator iter;
	for(iter = m_probability.begin(); iter != m_probability.end(); iter++)
	{
		if(iter->address == address)
		{
			link = *iter;
			return true;
		}
	}
	return false;
}

}
}
