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

#include <APPLICATIONS/session/server/Email.hpp>

using namespace applications::session::server;

STATIC_FACTORY_REGISTER_WITH_CREATOR(applications::session::server::Email,
				     applications::session::Session,
				     "server.Email", wns::PyConfigViewCreator);

Email::Email(const wns::pyconfig::View& _pyco) :
  Session(_pyco),
  emailSizeChoiceDistribution(NULL),
  largeEmailSizeDistribution(NULL),
  smallEmailSizeDistribution(NULL),
  lastEmailSent(false),
  request(false)
{
  wns::pyconfig::View nOEConfig(_pyco, "numberOfEmails");
  std::string nOEName = nOEConfig.get<std::string>("__plugin__");
  numberOfEmailsDistribution = wns::distribution::DistributionFactory::creator(nOEName)->create(nOEConfig);

  wns::pyconfig::View eSCConfig(_pyco, "emailChoice");
  std::string eSCName = eSCConfig.get<std::string>("__plugin__");
  emailSizeChoiceDistribution = wns::distribution::DistributionFactory::creator(eSCName)->create(eSCConfig);

  wns::pyconfig::View lESConfig(_pyco, "largeEmailSize");
  std::string lESName = lESConfig.get<std::string>("__plugin__");
  largeEmailSizeDistribution = wns::distribution::DistributionFactory::creator(lESName)->create(lESConfig);

  wns::pyconfig::View sESConfig(_pyco, "smallEmailSize");
  std::string sESName = sESConfig.get<std::string>("__plugin__");
  smallEmailSizeDistribution = wns::distribution::DistributionFactory::creator(sESName)->create(sESConfig);

  settlingTime = _pyco.get<wns::simulator::Time>("settlingTime");

  emailState = sendemail; //sendpop3ok; If session is started with POP3!
  state = idle;

  numberOfEmails = (*numberOfEmailsDistribution)();

  MESSAGE_BEGIN(NORMAL, logger, m, "APPL: Number of emails client has to receive is ");
  m << numberOfEmails << ".";
  MESSAGE_END();

  /* only for probing */
  sessionType = email;

  packetFrom = "server.Email";
}

Email::~Email()
{
  if (numberOfEmailsDistribution != NULL)
    delete numberOfEmailsDistribution;
  numberOfEmailsDistribution = NULL;

  if(emailSizeChoiceDistribution != NULL)
    delete emailSizeChoiceDistribution;
  emailSizeChoiceDistribution = NULL;

  if(smallEmailSizeDistribution !=NULL)
    delete smallEmailSizeDistribution;
  smallEmailSizeDistribution = NULL;

  if(largeEmailSizeDistribution != NULL)
    delete largeEmailSizeDistribution;
  largeEmailSizeDistribution = NULL;
}

void
Email::onData(const wns::osi::PDUPtr& _pdu)
{
  assureType(_pdu.getPtr(), applications::session::PDU*);

  receivedPacketNumber = static_cast<applications::session::PDU*>(_pdu.getPtr())->getPacketNumber();

  MESSAGE_SINGLE(NORMAL, logger, "APPL: receivedPacketNumber = " << receivedPacketNumber << ".");

  request = static_cast<applications::session::PDU*>(_pdu.getPtr())->getRequest();
  lastPacket = static_cast<applications::session::PDU*>(_pdu.getPtr())->getLastPacket();

  applications::session::Session::incomingProbesCalculation(_pdu);

  if((receivedPacketNumber == 1) && (state != sessionended))
    {
      state = running;

      firstPacketDelay = packetDelay;
    }

  if(lastPacket == true)
    {
      /* Wait till session ends if all emails received AND all emails were sent. */

      MESSAGE_SINGLE(NORMAL, logger, "APPL: All emails were sent and received. Now wait till session ends.");
    }
  else if(lastEmailSent == true && request == true)
    {
      /* Client received all emails. Now client wants to send emails.*/
      // onTimeout(sendtimeout);
    }
  else if(lastEmailSent == false && request == true && state != sessionended)
    {
      state = running;

      onTimeout(receivetimeout);
    }
}

void
Email::onTimeout(const Timeout& _t)
{
  if(_t == receivetimeout)
    {
      if(emailState == sendemail)
	{
	  /* Request for email received. Now sending email.*/
	  double emailSizeChoice = (*emailSizeChoiceDistribution)();
	  if(emailSizeChoice <= 0.8)
	    {
	      double distributedValue = (*smallEmailSizeDistribution)();
	      /* The distributed value is in Kbyte, so it has to be converted to bit! */
	      packetSize = distributedValue * 8.0 * 1024.0;;
	    }
	  else
	    {
	      double distributedValue = (*largeEmailSizeDistribution)();
	      /* The distributed value is in Kbyte, so it has to be converted to bit! */
	      packetSize = distributedValue * 8.0 * 1024.0;
	    }

	  applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
	  applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());

	  ++packetNumber;
	  applicationPDU->setPacketNumber(packetNumber, packetFrom);
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

	  state = idle;

	  if(numberOfEmails == 1)
	    {
	      /* sending last email */
	      applicationPDU->setLastPacket(true);
	      wns::osi::PDUPtr pdu(applicationPDU);
          applications::session::Session::outgoingProbesCalculation(pdu);

	      connection->sendData(pdu);

	      lastEmailSent = true;
	    }
	  else
	    {
	      numberOfEmails = numberOfEmails - 1;
	      wns::osi::PDUPtr pdu(applicationPDU);
          applications::session::Session::outgoingProbesCalculation(pdu);

	      connection->sendData(pdu);
	    }
	}
      else if(emailState == sendpop3ok)
	{
	  /* Sending response POP3 OK.*/
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: Sending POP3 OK!");

	  packetSize = 648;
	  applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
	  applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());

	  ++packetNumber;
	  applicationPDU->setPacketNumber(packetNumber, packetFrom);
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

	  wns::osi::PDUPtr pdu(applicationPDU);
      applications::session::Session::outgoingProbesCalculation(pdu);

	  connection->sendData(pdu);

	  emailState = sendusernameok;
	  state = idle;
	}
      else if(emailState == sendusernameok)
	{
	  /* Username received. Sending username OK.*/
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: Sending username OK!");

	  packetSize = 440;
	  applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
	  applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());

	  ++packetNumber;
	  applicationPDU->setPacketNumber(packetNumber, packetFrom);
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

	  wns::osi::PDUPtr pdu(applicationPDU);
      applications::session::Session::outgoingProbesCalculation(pdu);

	  connection->sendData(pdu);

	  emailState = sendpasswordok;
	  state = idle;
	}
      else if(emailState == sendpasswordok)
	{
	  /* Password received. Sending password OK.*/
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: Sending password OK!");

	  packetSize = 440;
	  applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
	  applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());

	  ++packetNumber;
	  applicationPDU->setPacketNumber(packetNumber, packetFrom);
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

	  wns::osi::PDUPtr pdu(applicationPDU);
      applications::session::Session::outgoingProbesCalculation(pdu);

	  connection->sendData(pdu);

	  emailState = sendemail;
	  state = idle;
	}
    }
  else if(_t == sendtimeout)
    {
      /* Now sending SMTP OK. */
      MESSAGE_SINGLE(NORMAL, logger, "APPL: Sending SMTP OK");

      packetSize = 648;
      applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
      applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());

      packetNumber = 1;
      applicationPDU->setPacketNumber(packetNumber, packetFrom);
      MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

      applicationPDU->setLastPacket(true);

      wns::osi::PDUPtr pdu(applicationPDU);
      applications::session::Session::outgoingProbesCalculation(pdu);

      connection->sendData(pdu);
      state = idle;
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
