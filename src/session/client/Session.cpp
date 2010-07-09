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

#include <APPLICATIONS/session/client/Session.hpp>

using namespace applications::session::client;

STATIC_FACTORY_REGISTER_WITH_CREATOR(applications::session::client::Session,
				     applications::session::Session,
				     "client.Session", wns::PyConfigViewCreator);

Session::Session(const wns::pyconfig::View& _pyco):
  session::Session(_pyco),
  sessionDelayDistribution(NULL),
  sessionDelay(0.0)
{
  wns::pyconfig::View sDDConfig(_pyco, "sessionDelay");
  std::string sDDName = sDDConfig.get<std::string>("__plugin__");
  sessionDelayDistribution = wns::distribution::DistributionFactory::creator(sDDName)->create(sDDConfig);

  stationType = CLIENT;
}

Session::~Session()
{
  if(sessionDelayDistribution != NULL)
    delete sessionDelayDistribution;
  sessionDelayDistribution = NULL;
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

  if(connection != NULL)
    {
      binding->releaseBinding(connection);
    }
  else
    {
      MESSAGE_SINGLE(NORMAL, logger, "APPL: No connection was established at the end of the session!");
    }

  applications::session::Session::sessionProbesCalculation();
}
