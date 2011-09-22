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

#include <APPLICATIONS/session/server/WWW.hpp>

using namespace applications::session::server;

STATIC_FACTORY_REGISTER_WITH_CREATOR(applications::session::server::WWW,
				     applications::session::Session,
				     "server.WWW", wns::PyConfigViewCreator);

WWW::WWW(const wns::pyconfig::View& _pyco) :
  Session(_pyco),
  sizeOfEmbeddedObjectDistribution(NULL),
  sizeOfMainObjectDistribution(NULL),
  mainObject(true)
{
  wns::pyconfig::View sEOConfig(_pyco, "sizeOfEmbeddedObject");
  std::string sEOName = sEOConfig.get<std::string>("__plugin__");
  sizeOfEmbeddedObjectDistribution = wns::distribution::DistributionFactory::creator(sEOName)->create(sEOConfig);

  wns::pyconfig::View sMOConfig(_pyco, "sizeOfMainObject");
  std::string sMOName = sMOConfig.get<std::string>("__plugin__");
  sizeOfMainObjectDistribution = wns::distribution::DistributionFactory::creator(sMOName)->create(sMOConfig);

  settlingTime = _pyco.get<wns::simulator::Time>("settlingTime");

  /* only for probes */
  sessionType = www;

  packetFrom = "server.WWW";
}

WWW::~WWW()
{
  if(sizeOfEmbeddedObjectDistribution != NULL)
    delete sizeOfEmbeddedObjectDistribution;
  sizeOfEmbeddedObjectDistribution = NULL;

  if(sizeOfMainObjectDistribution != NULL)
    delete sizeOfMainObjectDistribution;
  sizeOfMainObjectDistribution = NULL;
}

void
WWW::onData(const wns::osi::PDUPtr& _pdu)
{
  state = running;

  assureType(_pdu.getPtr(), applications::session::PDU*);

  receivedPacketNumber = static_cast<applications::session::PDU*>(_pdu.getPtr())->getPacketNumber();

  MESSAGE_SINGLE(NORMAL, logger, "APPL: receivedPacketNumber = " << receivedPacketNumber << ".");

  applications::session::Session::incomingProbesCalculation(_pdu);

  lastPacket = static_cast<applications::session::PDU*>(_pdu.getPtr())->getLastPacket();

  if(receivedPacketNumber == 1)
    {
      firstPacketDelay = packetDelay;
    }

  onTimeout(statetimeout);
}

void
WWW::onTimeout(const Timeout& _t)
{
  if(_t == statetimeout)
    {
      if(mainObject == false)
	{
	  /* Sending embedded object. */

	  double distributedValue = (*sizeOfEmbeddedObjectDistribution)();
	  /* The distributed value is in byte, so it has to be converted to bit! */
	  packetSize = distributedValue * 8.0;

	  applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
	  applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());

	  ++packetNumber;
	  applicationPDU->setPacketNumber(packetNumber, packetFrom);
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

	  state = idle;

	  if(lastPacket == false)
	    {
	      wns::osi::PDUPtr pdu(applicationPDU);
          applications::session::Session::outgoingProbesCalculation(pdu);

	      connection->sendData(pdu);
	    }
	  else
	    {
	      /* Sending last embeddedobject and wait till session ends. */
	      applicationPDU->setLastPacket(true);
	      wns::osi::PDUPtr pdu(applicationPDU);
          applications::session::Session::outgoingProbesCalculation(pdu);

	      connection->sendData(pdu);
	    }
	}
      else
	{
	  /* Sending main object. */
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: Sending mainObject.");

	  double distributedValue = (*sizeOfMainObjectDistribution)();
	  /* The distributed value is in byte, so it has to be converted to bit! */
	  packetSize = distributedValue * 8.0;

	  applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
	  applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());

	  packetNumber = 1;
	  applicationPDU->setPacketNumber(packetNumber, packetFrom);
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

	  wns::osi::PDUPtr pdu(applicationPDU);
      applications::session::Session::outgoingProbesCalculation(pdu);

	  connection->sendData(pdu);

	  mainObject = false;
	  state = idle;
	}
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
