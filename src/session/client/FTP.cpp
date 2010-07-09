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

#include <APPLICATIONS/session/client/FTP.hpp>

using namespace applications::session::client;

STATIC_FACTORY_REGISTER_WITH_CREATOR(applications::session::client::FTP,
				     applications::session::Session,
				     "client.FTP", wns::PyConfigViewCreator);

FTP::FTP(const wns::pyconfig::View& _pyco) :
  Session(_pyco),
  amountOfDataDistribution(NULL),
  readingTimeDistribution(NULL),
  numberOfFiles(2),
  filePacketSize(0),
  request(true)
{
  wns::pyconfig::View aDDConfig(_pyco, "amountOfData");
  std::string aDDName = aDDConfig.get<std::string>("__plugin__");
  amountOfDataDistribution = wns::distribution::DistributionFactory::creator(aDDName)->create(aDDConfig);

  wns::pyconfig::View rTDConfig(_pyco, "readingTime");
  std::string rTDName = rTDConfig.get<std::string>("__plugin__");
  readingTimeDistribution = wns::distribution::DistributionFactory::creator(rTDName)->create(rTDConfig);

  ftpState = waitforpasswordok;//requestftpok;

  settlingTime = _pyco.get<wns::simulator::Time>("settlingTime");

  sessionDelay = (*sessionDelayDistribution)();
  MESSAGE_SINGLE(NORMAL, logger, "APPL: Delay before session starts: " << sessionDelay << ".\n");

  setTimeout(connectiontimeout, sessionDelay);
  setTimeout(probetimeout, windowSize);

  state = running;

  /* only for probing */
  sessionType = ftp;
}

FTP::~FTP()
{
  if(amountOfDataDistribution != NULL)
    delete amountOfDataDistribution;
  amountOfDataDistribution = NULL;

  if(readingTimeDistribution != NULL)
    delete readingTimeDistribution;
  readingTimeDistribution = NULL;
}

void
FTP::onData(const wns::osi::PDUPtr& _pdu)
{
  assureType(_pdu.getPtr(), applications::session::PDU*);

  receivedPacketNumber = static_cast<applications::session::PDU*>(_pdu.getPtr())->getPacketNumber();

  MESSAGE_SINGLE(NORMAL, logger, "APPL: Received packetNumber " << receivedPacketNumber << ".");

  applications::session::Session::incomingProbesCalculation(_pdu);

  lastPacket = static_cast<applications::session::PDU*>(_pdu.getPtr())->getLastPacket();
  request = static_cast<applications::session::PDU*>(_pdu.getPtr())->getRequest();

  if(request == false)
    {
      MESSAGE_SINGLE(NORMAL, logger, "APPL: Acknowledgement about upload received!");
    }
  else if((lastPacket == false) && (state != sessionended))
    {
      onTimeout(statetimeout);

      state = running;
    }
  else
    {
      /* Last file received. First read this file and than wait till session ends. */

      MESSAGE_SINGLE(NORMAL, logger, "APPL: Received last requested file.");

      wns::simulator::Time readingTime = (*readingTimeDistribution)();
    }
}

void
FTP::onTimeout(const Timeout& _t)
{
  if(_t == statetimeout)
    {
      if(ftpState == requestdata)
	{
	  /* Uploaded one file. Now sending request for data. */
	  packetSize = 1600;
	  applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
	  applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());
	  applicationPDU->setRequest(true);

	  applications::session::Session::outgoingProbesCalculation();

	  ++packetNumber;
	  applicationPDU->setPacketNumber(packetNumber, packetFrom);
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

	  double distributedValue = (*amountOfDataDistribution)();
	  /* The distributed value is in Mbyte, so it has to be converted to bit! */
	  filePacketSize = distributedValue * 8.0 * 1024.0 * 1024.0;

	  MESSAGE_SINGLE(NORMAL, logger, "APPL: Requesting file with a size of " << filePacketSize << " bits.");

	  applicationPDU->setFileLength(filePacketSize);

	  if(numberOfFiles == 1)
	    {
	      /* Sending request for last data. */
	      applicationPDU->setLastPacket(true);
	    }

	  wns::osi::PDUPtr pdu(applicationPDU);

	  numberOfFiles = numberOfFiles - 1;

	  connection->sendData(pdu);

	  ftpState = reading;
	  state = idle;
	}
      else if(ftpState == reading)
	{
	  /* Data received. Now reading it */
	  wns::simulator::Time readingTime = (*readingTimeDistribution)();

	  MESSAGE_SINGLE(NORMAL, logger, "APPL: Received requested file.");

	  setTimeout(statetimeout, readingTime);

	  ftpState = requestdata;
	  state = running;
	}
      else if(ftpState == waitforpasswordok)
	{
	  double distributedValue = (*amountOfDataDistribution)();
	  /* The distributed value is in Mbyte, so it has to be converted to bit! */
	  packetSize = distributedValue * 8.0 * 1024.0 * 1024.0;

	  MESSAGE_SINGLE(NORMAL, logger, "APPL: Uploading file with packetSize = " << packetSize);

	  applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
	  applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());

	  applications::session::Session::outgoingProbesCalculation();

	  ++packetNumber;
	  applicationPDU->setPacketNumber(packetNumber, packetFrom);
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

	  wns::osi::PDUPtr pdu(applicationPDU);

	  connection->sendData(pdu);

	  ftpState = requestdata;
	  state = idle;

	  onTimeout(statetimeout);
	}
      else if(ftpState == waitforusernameok)
	{
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: Username ok received. Now sending password ok request.");

	  /* Received username OK. Sending request for password OK. */
	  packetSize = 240;
	  applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
	  applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());
	  applicationPDU->setRequest(true);

	  applications::session::Session::outgoingProbesCalculation();

	  ++packetNumber;
	  applicationPDU->setPacketNumber(packetNumber, packetFrom);
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

	  wns::osi::PDUPtr pdu(applicationPDU);

	  connection->sendData(pdu);

	  ftpState = waitforpasswordok;
	  state = idle;
	}
      else if(ftpState == waitforftpok)
	{
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: FTP OK received. Now sending username ok request.");

	  /* Received FTP OK. Sending request for username OK. */
	  packetSize = 240;
	  applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
	  applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());
	  applicationPDU->setRequest(true);

	  applications::session::Session::outgoingProbesCalculation();

	  ++packetNumber;
	  applicationPDU->setPacketNumber(packetNumber, packetFrom);
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

	  wns::osi::PDUPtr pdu(applicationPDU);

	  connection->sendData(pdu);

	  ftpState = waitforusernameok;
	  state = idle;
	}
      else if(ftpState == requestftpok)
	{
	  /* Sending request for FTP OK. */
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: Start sending FTP OK request.");

	  packetSize = 648;
	  applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
	  applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());
	  applicationPDU->setRequest(true);

	  applications::session::Session::outgoingProbesCalculation();

	  packetNumber = 1;
	  applicationPDU->setPacketNumber(packetNumber, packetFrom);
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

	  wns::osi::PDUPtr pdu(applicationPDU);

	  connection->sendData(pdu);

	  ftpState = waitforftpok;
	  state = idle;
	}
    }
  else if(_t == connectiontimeout)
    {
      packetFrom = "client.FTP";

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
FTP::onConnectionEstablished(wns::service::tl::Connection* _connection)
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

  onTimeout(statetimeout);
}
