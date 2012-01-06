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
  sessionDelay(0.0),
  establishedAt(0.0)
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

  /* 
  First condition checks if L2 connection was established.
  Second one checks if at least one PDU got through to create
  the server session.
  */
  if(connection != NULL && receivedPacketNumber > 0)
  {
    binding->releaseBinding(connection);
    if(establishedAt > settlingTime)
    {
      MESSAGE_SINGLE(NORMAL, logger, "APPL: Connection established but after probing started!");
      connectionProbe->put(0, 
            boost::make_tuple("Appl.CellId", getCellId(senderId)));
    }
    else
    {
      MESSAGE_SINGLE(NORMAL, logger, "APPL: Connection was succesfully established!");
      connectionProbe->put(1, 
            boost::make_tuple("Appl.CellId", getCellId(senderId)));
    }        
  }
  else
  {
      MESSAGE_SINGLE(NORMAL, logger, "APPL: No connection was established at the end of the session!");
      connectionProbe->put(0);
  }

  applications::session::Session::sessionProbesCalculation();
}

void
Session::onConnectionEstablished(wns::service::tl::Connection* _connection)
{
  /* Connection is ready, so start sending after session start delay. */
  connection = _connection;
  establishedAt = wns::simulator::getEventScheduler()->getTime();

  MESSAGE_SINGLE(NORMAL, logger, "APPL: Connection established!");

  onTimeout(statetimeout);
}
