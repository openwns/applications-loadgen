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

#include <APPLICATIONS/session/server/FTP.hpp>

using namespace applications::session::server;

STATIC_FACTORY_REGISTER_WITH_CREATOR(applications::session::server::FTP,
				     applications::session::Session,
				     "server.FTP", wns::PyConfigViewCreator);

FTP::FTP(const wns::pyconfig::View& _pyco):
  Session(_pyco),
  request(false)
{
  settlingTime = _pyco.get<wns::simulator::Time>("settlingTime");

  ftpState = waitfordatarequest; //sendftpok;
  state = idle;

  /* only for probing */
  sessionType = ftp;

  packetFrom = "server.FTP";
}

FTP::~FTP()
{
}

void
FTP::onData(const wns::osi::PDUPtr& _pdu)
{
  assureType(_pdu.getPtr(), applications::session::PDU*);

  receivedPacketNumber = static_cast<applications::session::PDU*>(_pdu.getPtr())->getPacketNumber();

  MESSAGE_SINGLE(NORMAL, logger, "APPL: Received packetNumber " << receivedPacketNumber << ".");

  packetSize = static_cast<applications::session::PDU*>(_pdu.getPtr())->getFileLength();

  applications::session::Session::incomingProbesCalculation(_pdu);

  if(receivedPacketNumber == 1 && state != sessionended)
    {
      state = running;

      firstPacketDelay = packetDelay;
    }

  lastPacket = static_cast<applications::session::PDU*>(_pdu.getPtr())->getLastPacket();
  request = static_cast<applications::session::PDU*>(_pdu.getPtr())->getRequest();

  if(request == false)
    {
      /* Client uploaded file. Send acknowledgement massage!*/
      packetSize = 16;

      applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
      applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());
      applicationPDU->setRequest(false);

      ++packetNumber;
      applicationPDU->setPacketNumber(packetNumber, packetFrom);
      MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

      wns::osi::PDUPtr pdu(applicationPDU);
      applications::session::Session::outgoingProbesCalculation(pdu);

      connection->sendData(pdu);
    }
  else if((lastPacket == false) && (state != sessionended))
    {
      state = running;

      onTimeout(statetimeout);
    }
  else
    {
      /* Sending last requested data and than wait till session ends! */
      applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
      applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());
      applicationPDU->setRequest(true);
      applicationPDU->setLastPacket(true);

      ++packetNumber;
      applicationPDU->setPacketNumber(packetNumber, packetFrom);
      MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

      wns::osi::PDUPtr pdu(applicationPDU);
      applications::session::Session::outgoingProbesCalculation(pdu);

      connection->sendData(pdu);
    }
}

void
FTP::onTimeout(const Timeout& _t)
{
  if(_t == statetimeout)
    {
      if(ftpState == waitfordatarequest)
	{
	  /* Sending requested data! */
	  applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
	  applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());
	  applicationPDU->setRequest(true);

	  ++packetNumber;
	  applicationPDU->setPacketNumber(packetNumber, packetFrom);
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

	  wns::osi::PDUPtr pdu(applicationPDU);
      applications::session::Session::outgoingProbesCalculation(pdu);

	  connection->sendData(pdu);

	  state = idle;
	}
      else if(ftpState == sendftpok)
	{
	  /* Sending FTP OK to client. */
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: Sending FTP OK.");

	  packetSize = 648;
	  applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
	  applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());
	  applicationPDU->setRequest(true);

	  packetNumber = 1;
	  applicationPDU->setPacketNumber(packetNumber, packetFrom);
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

	  wns::osi::PDUPtr pdu(applicationPDU);
      applications::session::Session::outgoingProbesCalculation(pdu);

	  connection->sendData(pdu);

	  ftpState = sendusernameok;
	  state = idle;
	}
      else if(ftpState == sendusernameok)
	{
	  /* Sending username ok to client. */
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: Sending username OK.");

	  packetSize = 440;
	  applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
	  applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());
	  applicationPDU->setRequest(true);

	  ++packetNumber;
	  applicationPDU->setPacketNumber(packetNumber, packetFrom);
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

	  wns::osi::PDUPtr pdu(applicationPDU);
      applications::session::Session::outgoingProbesCalculation(pdu);

	  connection->sendData(pdu);

	  ftpState = sendpasswordok;
	  state = idle;
	}
      else if(ftpState == sendpasswordok)
	{
	  /* Sending password ok.*/
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: Sending password OK!");

	  packetSize = 440;
	  applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
	  applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());
	  applicationPDU->setRequest(true);

	  ++packetNumber;
	  applicationPDU->setPacketNumber(packetNumber, packetFrom);
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

	  wns::osi::PDUPtr pdu(applicationPDU);
      applications::session::Session::outgoingProbesCalculation(pdu);

	  connection->sendData(pdu);

	  ftpState = waitfordatarequest;
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
