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

#ifndef APPLICATIONS_SESSION_SERVER_WIMAX_VIDEOTELEPHONY_HPP
#define APPLICATIONS_SESSION_SERVER_WIMAX_VIDEOTELEPHONY_HPP

#include <APPLICATIONS/session/server/Session.hpp>

namespace applications { namespace session { namespace server { namespace wimax {

	/* VideoTelephonyStates are used to see the voice Sessionphase */
	enum WiMAXVideoTelephonyState
	  {
	    active = 1,
	    inactive = 2
	  };

	class VideoTelephony :
    public applications::session::server::Session
	{
	public:
	  VideoTelephony(const wns::pyconfig::View& _pyco);
	  ~VideoTelephony();

	  void
	  onData(const wns::osi::PDUPtr& _pdu);

	  void
	  onTimeout(const Timeout& _t);

	private:
	  /* Voice parameters */
	  bool comfortNoise;
	  Bit voicePacketSize;
	  Bit comfortNoisePacketSize;
	  wns::simulator::Time voicePacketIat;

	  wns::simulator::Time videoPacketIat;
	  int videoFrameRate;

	private:
	  /* According to "IEEE 802.16m Evaluation Methodology Document(2009), Page 115, section 10.5",
	     the mean I frame size is 4742 byte. The values are Weibull distributed. */
	  wns::distribution::Distribution* iFramePacketSizeDistribution;
	  /* The distributed value has to be shifted. */
	  double shiftI;

	  /* According to "IEEE 802.16m Evaluation Methodology Document(2009), Page 115, section 10.5",
	     the mean B frame size is 259 byte. The values are Lognormal distributed. */
	  wns::distribution::Distribution* bFramePacketSizeDistribution;

	  /* According to "IEEE 802.16m Evaluation Methodology Document(2009), Page 115, section 10.5",
	     the mean B frame size is 147 byte. The values are Lognormal distributed. */
	  wns::distribution::Distribution* pFramePacketSizeDistribution;


	  /* According to "IMT-Adv. ITU-R M.2135" every 20ms it is decided if a state transition takes place. */
	  wns::distribution::Distribution* stateTransitionDistribution;
	  int cnCounter;

	  applications::session::server::wimax::WiMAXVideoTelephonyState voiceState;

	  int bFrameCounter;
	  int gopCounter;
	  bool newGOP;
	  bool iFrame;
	  bool bFrame;
	  bool pFrame;

	};
      }// namespace wimax
    } // namespace server
  } // namespace session
} // namespsce applications

#endif //APPLICATIONS_SESSION_CLIENT_WIMAX_VIDEOTELEPHONY_HPP
