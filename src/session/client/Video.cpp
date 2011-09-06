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

#include <APPLICATIONS/session/client/Video.hpp>

using namespace applications::session::client;

STATIC_FACTORY_REGISTER_WITH_CREATOR(applications::session::client::Video,
				     applications::session::Session,
				     "client.Video", wns::PyConfigViewCreator);

Video::Video(const wns::pyconfig::View& _pyco) :
  Session(_pyco)
{
  settlingTime = _pyco.get<wns::simulator::Time>("settlingTime");

  /* only for probing */
  sessionType = video;

  state = running;

  sessionDelay = (*sessionDelayDistribution)();
  MESSAGE_SINGLE(NORMAL, logger, "APPL: Delay before session starts: " << sessionDelay << ".\n");

  setTimeout(connectiontimeout, sessionDelay);
  setTimeout(probetimeout, windowSize);
}

Video::~Video()
{
}

void
Video::onData(const wns::osi::PDUPtr& _pdu)
{
  state = running;

  assureType(_pdu.getPtr(), applications::session::PDU*);

  receivedPacketNumber = static_cast<applications::session::PDU*>(_pdu.getPtr())->getPacketNumber();

  MESSAGE_SINGLE(NORMAL, logger, "APPL: receivedPacketNumber = " << receivedPacketNumber << ".");

  applications::session::Session::incomingProbesCalculation(_pdu);
}

void
Video::onTimeout(const Timeout& _t)
{
  if(_t == statetimeout)
    {
      /* Sending request for Video. */
      /* 1600 bit is the size of an URL */
      packetSize = 1600;
      applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);

      applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());

      applications::session::Session::outgoingProbesCalculation();

      packetNumber = 1;
      applicationPDU->setPacketNumber(packetNumber, packetFrom);
      MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

      wns::osi::PDUPtr pdu(applicationPDU);

      connection->sendData(pdu);

      state = idle;
    }
  else if(_t == connectiontimeout)
    {
      packetFrom = "client.Video";

      /* Open connection */
      binding->initBinding();
    }
  else if(_t == probetimeout)
    {
      applications::session::Session::onTimeout(_t);
    }
  else
    {
      assure(false, "APPL: Unknown timout type =" << _t);
    }
}


void
Video::onConnectionEstablished(wns::service::tl::Connection* _connection)
{
  /* Connection is ready, so start sending after session start delay. */
  connection = _connection;
  establishedAt = wns::simulator::getEventScheduler()->getTime();

  MESSAGE_SINGLE(NORMAL, logger, "APPL: Connection established!");

  onTimeout(statetimeout);
}
