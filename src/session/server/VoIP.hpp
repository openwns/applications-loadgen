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

#ifndef APPLICATIONS_SESSION_SERVER_VOIP_HPP
#define APPLICATIONS_SESSION_SERVER_VOIP_HP

#include <APPLICATIONS/session/server/Session.hpp>

namespace applications { namespace session { namespace server {

      /* VoIPStates are used to see Sessionphase */
      enum VoIPState
	{
	  active = 1,
	  inactive = 2
	};

      class VoIP :
    public applications::session::server::Session
      {
      public:
	VoIP(const wns::pyconfig::View& _pyco);
	~VoIP();

	void
	onData(const wns::osi::PDUPtr& _pdu);

	void
	onTimeout(const Timeout& _t);

      protected:
	bool comfortNoise;
	Bit voicePacketSize;
	Bit comfortNoisePacketSize;

      private:
	/* According to "IMT-Adv. ITU-R M.2135" every 20ms it is decided if a state transition takes place. */
	wns::distribution::Distribution* stateTransitionDistribution;
	int cnCounter;

	applications::session::server::VoIPState voIPState;

	bool firstPacketNumber;
    wns::simulator::Time trafficStartDelay;

      };
    } // namespace server
  } // namespace session
} // namespace applications

#endif //APPLICATIONS_SESSION_SERVER_VOIP_HPP
