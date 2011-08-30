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

#include <APPLICATIONS/session/client/Email.hpp>

using namespace applications::session::client;

STATIC_FACTORY_REGISTER_WITH_CREATOR(applications::session::client::Email,
				     applications::session::Session,
				     "client.Email", wns::PyConfigViewCreator);

Email::Email(const wns::pyconfig::View& _pyco) :
  Session(_pyco),
  numberOfEmailsDistribution(NULL),
  emailSizeChoiceDistribution(NULL),
  largeEmailSizeDistribution(NULL),
  smallEmailSizeDistribution(NULL),
  emailWritingTimeDistribution(NULL),
  emailReadingTimeDistribution(NULL),
  lastEmailSent(false)
{
  wns::pyconfig::View nOEConfig(_pyco, "numberOfEmails");
  std::string nOEName = nOEConfig.get<std::string>("__plugin__");
  numberOfEmailsDistribution = wns::distribution::DistributionFactory::creator(nOEName)->create(nOEConfig);

  wns::pyconfig::View eSCConfig(_pyco, "emailSizeChoice");
  std::string eSCName = eSCConfig.get<std::string>("__plugin__");
  emailSizeChoiceDistribution = wns::distribution::DistributionFactory::creator(eSCName)->create(eSCConfig);

  wns::pyconfig::View lESConfig(_pyco, "largeEmailSize");
  std::string lESName = lESConfig.get<std::string>("__plugin__");
  largeEmailSizeDistribution = wns::distribution::DistributionFactory::creator(lESName)->create(lESConfig);

  wns::pyconfig::View sESConfig(_pyco, "smallEmailSize");
  std::string sESName = sESConfig.get<std::string>("__plugin__");
  smallEmailSizeDistribution = wns::distribution::DistributionFactory::creator(sESName)->create(sESConfig);

  wns::pyconfig::View eWTConfig(_pyco, "emailWritingTime");
  std::string eWTName = eWTConfig.get<std::string>("__plugin__");
  emailWritingTimeDistribution = wns::distribution::DistributionFactory::creator(eWTName)->create(eWTConfig);

  wns::pyconfig::View eRTConfig(_pyco, "emailReadingTime");
  std::string eRTName = eRTConfig.get<std::string>("__plugin__");
  emailReadingTimeDistribution = wns::distribution::DistributionFactory::creator(eRTName)->create(eRTConfig);

  numberOfEmails = (*numberOfEmailsDistribution)();

  settlingTime = _pyco.get<wns::simulator::Time>("settlingTime");

  MESSAGE_BEGIN(NORMAL, logger, m, "APPL: Number of emails client wants to send is ");
  m << numberOfEmails << ".";
  MESSAGE_END();

  emailState1 = waitforpasswordok; //requestpop3ok; if a session has to be started with POP3!
  emailState2 = readlastemail;

  sessionDelay = (*sessionDelayDistribution)();
  MESSAGE_SINGLE(NORMAL, logger, "APPL: Delay before session starts: " << sessionDelay << ".\n");
  setTimeout(connectiontimeout, sessionDelay);
  setTimeout(probetimeout, windowSize);

  state = running;

  /* only for probing */
  sessionType = email;
}

Email::~Email()
{
  if(numberOfEmailsDistribution != NULL)
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

  if(emailWritingTimeDistribution != NULL)
    delete emailWritingTimeDistribution;
  emailWritingTimeDistribution = NULL;

  if(emailReadingTimeDistribution != NULL)
    delete emailReadingTimeDistribution;
  emailReadingTimeDistribution = NULL;
}

void
Email::onData(const wns::osi::PDUPtr& _pdu)
{
  assureType(_pdu.getPtr(), applications::session::PDU*);

  receivedPacketNumber = static_cast<applications::session::PDU*>(_pdu.getPtr())->getPacketNumber();

  MESSAGE_SINGLE(NORMAL, logger, "APPL: receivedPacketNumber = " << receivedPacketNumber << ".");

  applications::session::Session::incomingProbesCalculation(_pdu);

  lastPacket = static_cast<applications::session::PDU*>(_pdu.getPtr())->getLastPacket();

  if((lastPacket == true) && (state != sessionended))
    {
      state = running;

      /* Received all emails. Start with sending new emails after read last one. */
      onTimeout(sendtimeout);
    }
  else if(lastPacket == false && state != sessionended)
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
      if(emailState1 == receiving)
	{
	  /* Received email and now reading it.*/
	  wns::simulator::Time readingTime = (*emailReadingTimeDistribution)();

	  MESSAGE_SINGLE(NORMAL, logger, "APPL: Received email. Readingtime = " << readingTime << ".");

	  if(lastPacket == false)
	    {
	      setTimeout(receivetimeout, readingTime);

	      emailState1 = reading;
	      state = running;
	    }
	}
      else if(emailState1 == reading)
	{
	  /* Sending request for next email.*/
	  packetSize = 176;
	  applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
	  applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());
	  applicationPDU->setRequest(true);

	  applications::session::Session::outgoingProbesCalculation();

	  ++packetNumber;
	  applicationPDU->setPacketNumber(packetNumber, packetFrom);
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

	  wns::osi::PDUPtr pdu(applicationPDU);

	  connection->sendData(pdu);

	  emailState1 = receiving;
	  state = idle;
	}
      else if(emailState1 == requestpop3ok)
	{
	  /* Start with sending POP3 request.*/
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: Start sending POP3 request!");

	  packetSize = 2800;
	  applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
	  applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());
	  applicationPDU->setRequest(true);

	  applications::session::Session::outgoingProbesCalculation();

	  ++packetNumber;
	  applicationPDU->setPacketNumber(packetNumber, packetFrom);
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

	  wns::osi::PDUPtr pdu(applicationPDU);

	  connection->sendData(pdu);

	  emailState1 = waitforpop3ok;
	  state = idle;
	}
      else if(emailState1 == waitforpop3ok)
	{
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: Received POP3 OK! Now requesting username OK.");

	  /* Received POP3 OK. Sending request for username OK.*/
	  packetSize = 232;
	  applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
	  applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());
	  applicationPDU->setRequest(true);

	  applications::session::Session::outgoingProbesCalculation();

	  ++packetNumber;
	  applicationPDU->setPacketNumber(packetNumber, packetFrom);
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

	  wns::osi::PDUPtr pdu(applicationPDU);

	  connection->sendData(pdu);

	  emailState1 = waitforusernameok;
	  state = idle;
	}
      else if(emailState1 == waitforusernameok)
	{
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: Received username OK! Now requesting Password OK.");

	  /* Received username OK. Sending request for password OK.*/
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

	  emailState1 = waitforpasswordok;
	  state = idle;
	}
      else if(emailState1 == waitforpasswordok)
	{
	  MESSAGE_SINGLE(NORMAL, logger,"APPL: Requesting first Email.");

	  /* Received passwordok OK. Sending request for the first email.*/
	  packetSize = 176;
	  applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
	  applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());
	  applicationPDU->setRequest(true);

	  applications::session::Session::outgoingProbesCalculation();

	  ++packetNumber;
	  applicationPDU->setPacketNumber(packetNumber, packetFrom);
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

	  wns::osi::PDUPtr pdu(applicationPDU);

	  connection->sendData(pdu);

	  emailState1 = receiving;
	  state = idle;
	}
    }
  else if(_t == sendtimeout)
    {
      if(emailState2 == waitforsmtpok || emailState2 == sending)
	{
	  /* Starting to write email. */
	  wns::simulator::Time writingTime = (*emailWritingTimeDistribution)();

	  MESSAGE_SINGLE(NORMAL, logger, "APPL: Writing email. Writingtime = " << writingTime << ".");

	  setTimeout(sendtimeout, writingTime);

	  emailState2 = writing;
	  state = running;
	}
      else if(emailState2 == writing)
	{
	  /* Wrote email and now send it. */
	  double emailSizeChoice = (*emailSizeChoiceDistribution)();
	  if(emailSizeChoice <= 0.8)
	    {
	      double distributedValue = (*smallEmailSizeDistribution)();
	      /* The distributed value is in Kbyte, so it has to be converted to bit! */
	      packetSize = distributedValue * 8.0 * 1024.0;
	    }
	  else
	    {
	      double distributedValue = (*largeEmailSizeDistribution)();
	      /* The distributed value is in Kbyte, so it has to be converted to bit! */
	      packetSize = distributedValue * 8.0 * 1024.0;
	    }

	  applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
	  applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());

	  applications::session::Session::outgoingProbesCalculation();

	  ++packetNumber;
	  applicationPDU->setPacketNumber(packetNumber, packetFrom);
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

	  if(numberOfEmails == 1)
	    {
	      /* sending last packet */
	      applicationPDU->setLastPacket(true);
	      wns::osi::PDUPtr pdu(applicationPDU);

	      connection->sendData(pdu);

	      lastEmailSent = true;

	      /* Wait till session ends. */
	      if(lastPacket == true)
		{
		  MESSAGE_SINGLE(NORMAL, logger, "APPL: All emails were sent and received. Now wait till session ends!");
		}
	    }
	  else
	    {
	      numberOfEmails = numberOfEmails - 1;

	      wns::osi::PDUPtr pdu(applicationPDU);

	      connection->sendData(pdu);

	      emailState2 = sending;

	      /* Start writing next email! */
	      onTimeout(sendtimeout);
	    }
	}
      else if(emailState2 == requestsmtpok)
	{
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: Sending SMTP OK request!");

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

	  emailState2 = waitforsmtpok;
	  state = idle;
	}
      else if(emailState2 == readlastemail)
	{
	  /* Reading last received email.*/
	  wns::simulator::Time readingTime = (*emailReadingTimeDistribution)();

	  MESSAGE_SINGLE(NORMAL, logger, "APPL: Received last email. Readingtime = " << readingTime << ".");

	  setTimeout(sendtimeout, readingTime);

	  emailState2 = sending; //requestsmtpok;
	}
    }
  else if(_t == probetimeout)
    {
      applications::session::Session::onTimeout(_t);
    }
  else if(_t == connectiontimeout)
    {
      packetFrom = "client.Email";

      /* Open connection */
      binding->initBinding();
    }
  else
    {
      assure(false, "APPL: Unknown timout type =" << _t);
    }
}

void
Email::onConnectionEstablished(wns::service::tl::Connection* _connection)
{
  /* Connection is ready, so start sending after session start delay. */
  connection = _connection;

  MESSAGE_SINGLE(NORMAL, logger, "APPL: Connection established!");

  onTimeout(receivetimeout);
}
