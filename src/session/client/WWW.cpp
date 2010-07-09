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

#include <APPLICATIONS/session/client/WWW.hpp>

using namespace applications::session::client;

STATIC_FACTORY_REGISTER_WITH_CREATOR(applications::session::client::WWW,
				     applications::session::Session,
				     "client.WWW", wns::PyConfigViewCreator);

WWW::WWW(const wns::pyconfig::View& _pyco) :
  Session(_pyco),
  readingTimeDistribution(NULL),
  parsingTimeDistribution(NULL),
  embeddedObjectsPerPageDistribution(NULL),
  readingTime(0.0),
  parsingTime(0.0),
  mainObject(true)
{
  wns::pyconfig::View rTDConfig(_pyco, "readingTime");
  std::string rTDName = rTDConfig.get<std::string>("__plugin__");
  readingTimeDistribution = wns::distribution::DistributionFactory::creator(rTDName)->create(rTDConfig);

  wns::pyconfig::View pTDConfig(_pyco, "parsingTime");
  std::string pTDName = pTDConfig.get<std::string>("__plugin__");
  parsingTimeDistribution = wns::distribution::DistributionFactory::creator(pTDName)->create(pTDConfig);

  wns::pyconfig::View eOPPConfig(_pyco, "embeddedObjectsPerPage");
  std::string eOPPName = eOPPConfig.get<std::string>("__plugin__");
  embeddedObjectsPerPageDistribution = wns::distribution::DistributionFactory::creator(eOPPName)->create(eOPPConfig);

  embeddedObjectsPerPage = (*embeddedObjectsPerPageDistribution)();

  MESSAGE_SINGLE(NORMAL, logger, "APPL: Number of embedded objects per page = " << embeddedObjectsPerPage  << ".");

  settlingTime = _pyco.get<wns::simulator::Time>("settlingTime");

  state = running;

  /* only for probing */
  sessionType = www;

  sessionDelay = (*sessionDelayDistribution)();
  MESSAGE_SINGLE(NORMAL, logger, "APPL: Delay before session starts: " << sessionDelay << ".\n");

  setTimeout(connectiontimeout, sessionDelay);
  setTimeout(probetimeout, windowSize);
}

WWW::~WWW()
{
  if (readingTimeDistribution != NULL)
    delete readingTimeDistribution;
  readingTimeDistribution = NULL;

  if (parsingTimeDistribution != NULL)
    delete parsingTimeDistribution;
  parsingTimeDistribution = NULL;

  if(embeddedObjectsPerPageDistribution != NULL)
    delete embeddedObjectsPerPageDistribution;
  embeddedObjectsPerPageDistribution = NULL;
}

void
WWW::onData(const wns::osi::PDUPtr& _pdu)
{
  assureType(_pdu.getPtr(), applications::session::PDU*);

  receivedPacketNumber = static_cast<applications::session::PDU*>(_pdu.getPtr())->getPacketNumber();

  MESSAGE_SINGLE(NORMAL, logger, "APPL: receivedPacketNumber = " << receivedPacketNumber << ".");

  applications::session::Session::incomingProbesCalculation(_pdu);

  lastPacket = static_cast<applications::session::PDU*>(_pdu.getPtr())->getLastPacket();

  if(receivedPacketNumber == 1 && state != sessionended)
    {
      state = running;

      onTimeout(receivetimeout);
    }
  else if(lastPacket == false && state != sessionended)
    {
      state = running;

      /* Embedded object received. First read the page and than transit to an other page.*/
      readingTime = (*readingTimeDistribution)();

      MESSAGE_SINGLE(NORMAL, logger, "APPL: Embedded object received. Readingtime = " << readingTime << ".");

      setTimeout(sendtimeout, readingTime);
    }
  else
    {
      /* Last object received. First read the page and than wait till session ends. */
      readingTime = (*readingTimeDistribution)();
      MESSAGE_SINGLE(NORMAL, logger, "APPL: Last embedded object received. Readingtime = " << readingTime << ".");
    }
}

void
WWW::onTimeout(const Timeout& _t)
{
  if(_t == sendtimeout)
    {
      if(mainObject == false)
	{
	  state = idle;

	  /* Requesting embedded objects. */
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: Requesting embedded object.");
	  embeddedObjectsPerPage = embeddedObjectsPerPage - 1;

	  /* Acording to "IEEE 802.16m Evaluation Methodology Document(2009), Page 109, section 10.1"
	     the HTTP request packet has a constant size of 350 bytes(=2800 bit). */
	  packetSize = 2800;
	  applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
	  applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());

	  applications::session::Session::outgoingProbesCalculation();

	  ++packetNumber;
	  applicationPDU->setPacketNumber(packetNumber, packetFrom);
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

	  if(embeddedObjectsPerPage == 0)
	    {
	      state = sessionended;

	      /* Requesting LAST embeddedObject. */
	      applicationPDU->setLastPacket(true);
	    }
	  wns::osi::PDUPtr pdu(applicationPDU);

	  connection->sendData(pdu);
	}
      else
	{
	  /* Requesting main object. */
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: Requesting main object.");

	  /* Acording to "IEEE 802.16m Evaluation Methodology Document(2009), Page 109, section 10.1"
	     the HTTP request packet has a constant size of 350 bytes(=2800 bit). */
	  packetSize = 2800;
	  applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
	  applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());
	  applicationPDU->setRequest(true);

	  applications::session::Session::outgoingProbesCalculation();

	  packetNumber = 1;
	  applicationPDU->setPacketNumber(packetNumber, packetFrom);
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

	  wns::osi::PDUPtr pdu(applicationPDU);

	  connection->sendData(pdu);

	  mainObject = false;

	  state = idle;
	}
    }
  else if(_t == receivetimeout)
    {
      /* Main object received. After a parsingtime for the main page the embedded objects will be requested. */
      parsingTime = (*parsingTimeDistribution)();

      MESSAGE_SINGLE(NORMAL, logger, "APPL: Main object received.");

      setTimeout(sendtimeout, parsingTime);
    }
  else if(_t == connectiontimeout)
    {
      packetFrom = "client.WWW";

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
WWW::onConnectionEstablished(wns::service::tl::Connection* _connection)
{
  /* Connection is ready, so start sending after session start delay. */
  connection = _connection;

  MESSAGE_SINGLE(NORMAL, logger, "APPL: Connection established!");

  now = wns::simulator::getEventScheduler()->getTime();

  if(now >= settlingTime)
    {
      connectionProbe->put(1);
    }
  else
    {
      connectionProbe->put(0);
    }

  onTimeout(sendtimeout);
}
