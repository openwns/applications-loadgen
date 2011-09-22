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

#include <APPLICATIONS/session/client/CBR.hpp>

using namespace applications::session::client;

STATIC_FACTORY_REGISTER_WITH_CREATOR(applications::session::client::CBR,
				     applications::session::Session,
				     "client.CBR", wns::PyConfigViewCreator);

CBR::CBR(const wns::pyconfig::View& _pyco) :
  session::client::Session(_pyco),
  packetSizeDistribution(NULL),
  bitRateDistribution(NULL)
{
  wns::pyconfig::View pSDConfig(_pyco, "packetSize");
  std::string pSDName = pSDConfig.get<std::string>("__plugin__");
  packetSizeDistribution = wns::distribution::DistributionFactory::creator(pSDName)->create(pSDConfig);

  wns::pyconfig::View bRDConfig(_pyco, "bitRate");
  std::string bRDName = bRDConfig.get<std::string>("__plugin__");
  bitRateDistribution = wns::distribution::DistributionFactory::creator(bRDName)->create(bRDConfig);

  state = running;

  settlingTime = _pyco.get<wns::simulator::Time>("settlingTime");

  /* Delay before session starts, otherwise all sessions will start at the same time. */
  sessionDelay = (*sessionDelayDistribution)();
  MESSAGE_SINGLE(NORMAL, logger, "APPL: Delay before session starts: " << sessionDelay << ".\n");

  setTimeout(connectiontimeout, sessionDelay);
  setTimeout(probetimeout, windowSize);

  /* only for probing */
  sessionType = cbr;
}

CBR::~CBR()
{
  if(packetSizeDistribution != NULL)
    delete packetSizeDistribution;
  packetSizeDistribution = NULL;

  if(bitRateDistribution != NULL)
    delete bitRateDistribution;
  bitRateDistribution = NULL;
}

void
CBR::onData(const wns::osi::PDUPtr& _pdu)
{
  state = running;

  assureType(_pdu.getPtr(), applications::session::PDU*);

  receivedPacketNumber = static_cast<applications::session::PDU*>(_pdu.getPtr())->getPacketNumber();

  MESSAGE_SINGLE(NORMAL, logger, "APPL: Received packetNumber " << receivedPacketNumber << ".");

  applications::session::Session::incomingProbesCalculation(_pdu);

  lastPacket = static_cast<applications::session::PDU*>(_pdu.getPtr())->getLastPacket();

  state = idle;
}

void
CBR::onTimeout(const Timeout& _t)
{
  if(_t == statetimeout)
    {
      packetSize = (*packetSizeDistribution)();
      bitRate = (*bitRateDistribution)();
      iat = (double(packetSize) / double(bitRate));

      MESSAGE_BEGIN(NORMAL, logger, m, "APPL: New running Client CBR Session: ");
      m << "iat = " << iat;
      m << ", packetSize = " << packetSize;
      m << ", bitRate = " << bitRate;
      MESSAGE_END();

      applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);

      applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());

      packetNumber = 1;
      applicationPDU->setPacketNumber(packetNumber, packetFrom);
      MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << " from " << packetFrom << ".");

      wns::osi::PDUPtr pdu(applicationPDU);

      MESSAGE_SINGLE(NORMAL, logger, "APPL: Generated pdu with " << packetSize << " bits.");

      applications::session::Session::outgoingProbesCalculation(pdu);

      connection->sendData(pdu);

      state = idle;
    }
  else if(_t == connectiontimeout)
    {
      packetFrom = "client.CBR" + packetFromID;

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


