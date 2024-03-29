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

#ifndef APPLICATIONS_SESSION_SERVER_SESSION_HPP
#define APPLICATIONS_SESSION_SERVER_SESSION_HPP

#include <APPLICATIONS/session/Session.hpp>

namespace applications {
  namespace service { namespace server {
      class ConnectionHandler;
    } //namespace service
  } // namespace server
  namespace session { namespace server {
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

      protected:
	/* Serversessions start after first packet arrived, so the delay
	   of the first packet has to be added to the session Duration. */
	wns::simulator::Time firstPacketDelay;

      };
    } // namespace server
  } // namespace session
} // namespace applications

#endif //APPLICATIONS_SESSION_SERVER_SESSION_HPP
