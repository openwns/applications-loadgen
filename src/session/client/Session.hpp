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

#ifndef APPLICATIONS_SESSION_CLIENT_SESSION_HPP
#define APPLICATIONS_SESSION_CLIENT_SESSION_HPP

#include <WNS/service/nl/Address.hpp>
#include <APPLICATIONS/session/Session.hpp>

namespace applications { namespace session { namespace client {
      class Session :
    public applications::session::Session
      {
      public:
	Session(const wns::pyconfig::View& _pyco);
	virtual
	~Session();

	virtual void
	onData(const wns::osi::PDUPtr& _pdu);

	virtual void
	onShutdown();

    virtual void
    onConnectionEstablished(wns::service::tl::Connection* _connection);

      protected:

	/* Used to avoid that all applications will start parallel. */
	wns::distribution::Distribution* sessionDelayDistribution;
	wns::simulator::Time sessionDelay;
    wns::simulator::Time establishedAt;

    /* 
    Used to delay traffic start after connection establishment
    so other nodes can establish their connection without distraction.
    This only delays traffic from the client, server could be already
    sending. This doeas not help if a shared full-duplex medium is used 
    (e.g. 802.11 WLAN).
    */
	wns::distribution::Distribution* trafficDelayDistribution;

      };
    } // namespace client
  } // namespace session
} // namespace applicatons

#endif //APPLICATIONS_SESSION_CLIENT_SESSION_HPP
