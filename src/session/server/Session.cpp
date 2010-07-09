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

#include <APPLICATIONS/session/server/Session.hpp>

using namespace applications::session::server;

STATIC_FACTORY_REGISTER_WITH_CREATOR(applications::session::server::Session,
				     applications::session::Session,
				     "server.Session", wns::PyConfigViewCreator);

Session::Session(const wns::pyconfig::View& _pyco):
  session::Session(_pyco),
  firstPacketDelay(0.0)
{
  stationType = SERVER;
  setTimeout(probetimeout, windowSize);
}

Session::~Session()
{
}

void
Session::onData(const wns::osi::PDUPtr& _pdu)
{
}

void
Session::onShutdown()
{
  /* Session ended, because simulationtime is over. */

  state = sessionended;

  cancelAllTimeouts();

  applications::session::Session::sessionProbesCalculation();

  listener->releaseBinding(connection);
}
