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

#ifndef MYNCCODNUM_H
#define MYNCCODNUM_H

#include "mync-repo.h"
#include <vector>

namespace ns3{
namespace mync{

class CodeNum
{
public:
	CodeNum();
	~CodeNum();

	void AddCodeNum(const Ipv4Address & addr, const uint16_t num, const uint32_t pktSize, const uint64_t time);
	bool FindCodeNum(const Ipv4Address & addr, CodeNumInfo & info) const;
	void AddPathInfo(const Ipv4Address & addr, const Ipv4Address & last, const uint32_t pktSize, const uint64_t time);
	bool FindPathInfo(const Ipv4Address & address, PathInfo & info) const;
	void AddProbability(const Ipv4Address & address, const double probability);
	bool FindProbability(const Ipv4Address & address, LinkProbability & link) const;
	std::vector<PathInfo> GetPathInfo() const;
	std::vector<CodeNumInfo> GetCodeNum() const;
	std::vector<LinkProbability> GetProbability() const;
private:
	std::vector<CodeNumInfo> m_codeNum;
	std::vector<PathInfo> m_pathInfo;
	std::vector<LinkProbability> m_probability;
};

}
}

#endif
