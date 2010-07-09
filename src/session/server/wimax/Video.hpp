/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 5, D-52074 Aachen, Germany
 * phone: ++49-241-80-27910,
 * fax: ++49-241-80-22242
 * email: info@openwns.org
 * www: http://www.openwns.org
 * _____________________________________________________________________________
 *
 * openWNS is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License version 2 as published by the
 * Free Software Foundation;
 *
 * openWNS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

/* Video model according to "IEEE 802.16m Evaluation Methodology Document(2009)".
   This model does not vary in frame types(I-,B- and P-frame). */

#ifndef APPLICATIONS_SESSION_SERVER_WIMAX_VIDEO_HPP
#define APPLICATIONS_SESSION_SERVER_WIMAX_VIDEO_HPP

#include <APPLICATIONS/session/server/Session.hpp>

namespace applications { namespace session { namespace server { namespace wimax {
	class Video :
    public applications::session::server::Session
	{
	public:
	  Video(const wns::pyconfig::View& _pyco);
	  ~Video();

	  void
	  onData(const wns::osi::PDUPtr& _pdu);

	  void
	  onTimeout(const Timeout& _t);

	private:
	  /* According to "IEEE 802.16m Evaluation Methodology Document(2009), Page 116-117,
	     section 10.4" the packetsizes are Pareto distributed with a mean of 800 bit and
	     a max of 2000 bit. */
	  wns::distribution::Distribution* packetSizeDistribution;

	  /* According to "IEEE 802.16m Evaluation Methodology Document(2009), Page 116-117,
	     section 10.4" the inter arrival time between the packets are Pareto distributed
	     with a mean of 6 ms and a max of 12.5 ms. */
	  wns::distribution::Distribution* packetIatDistribution;

	  double videoFrameRate;
	  wns::simulator::Time videoFrameIat;
	  double numberOfPacketsPerFrame;

	  int framePacketCounter;
	  bool firstPacketNumber;

	};
      }// namespace wimax
    } // namespace client
  } // namespace session
} // namespace applications

#endif //APPLICATIONS_SESSION_SERVER_WIMAX_VIDEO_HPP
