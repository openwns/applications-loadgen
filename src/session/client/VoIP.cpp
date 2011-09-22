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

#include <APPLICATIONS/session/client/VoIP.hpp>

using namespace applications::session::client;

STATIC_FACTORY_REGISTER_WITH_CREATOR(applications::session::client::VoIP,
				     applications::session::Session,
				     "client.VoIP", wns::PyConfigViewCreator);

VoIP::VoIP(const wns::pyconfig::View& _pyco) :
  Session(_pyco),
  stateTransitionDistribution(NULL),
  cnCounter(0)
{
  wns::pyconfig::View sTVConfig(_pyco, "stateTransition");
  std::string sTVName = sTVConfig.get<std::string>("__plugin__");
  stateTransitionDistribution = wns::distribution::DistributionFactory::creator(sTVName)->create(sTVConfig);

  settlingTime = _pyco.get<wns::simulator::Time>("settlingTime");

  iat = _pyco.get<wns::simulator::Time>("packetIat");
  voicePacketSize = _pyco.get<Bit>("voicePacketSize");
  /* Packet size includes 12 bytes RTP header. */
  voicePacketSize = voicePacketSize + 96;

  comfortNoisePacketSize = _pyco.get<Bit>("comfortNoisePacketSize");
  /* Packet size includes 12 bytes RTP header. */
  comfortNoisePacketSize = comfortNoisePacketSize + 96;

  comfortNoise = _pyco.get<bool>("comfortNoise");

  MESSAGE_BEGIN(NORMAL, logger, m, "APPL: New VoIP session started with: ");
  m << "voicePacketSize = " << voicePacketSize;
  m << ", iat = " << iat;
  m << ", comfortNoisePacketSize = " << comfortNoisePacketSize;
  MESSAGE_END();

  maxDelay = _pyco.get<wns::simulator::Time>("maxDelay");
  maxLossRatio = _pyco.get<double>("maxLossRatio");

  state = running;

  /* only for probing */
  sessionType = voip;

  sessionDelay = (*sessionDelayDistribution)();
  MESSAGE_SINGLE(NORMAL, logger, "APPL: Delay before session starts: " << sessionDelay << ".\n");

  setTimeout(connectiontimeout, sessionDelay);
  setTimeout(probetimeout, windowSize);
}


VoIP::~VoIP()
{
  if (stateTransitionDistribution != NULL)
    delete stateTransitionDistribution;
  stateTransitionDistribution = NULL;
}


void
VoIP::onData(const wns::osi::PDUPtr& _pdu)
{
  assureType(_pdu.getPtr(), applications::session::PDU*);

  receivedPacketNumber = static_cast<applications::session::PDU*>(_pdu.getPtr())->getPacketNumber();

  MESSAGE_SINGLE(NORMAL, logger, "APPL: receivedPacketNumber = " << receivedPacketNumber << ".");

  applications::session::Session::incomingProbesCalculation(_pdu);

  if((receivedPacketNumber == 1) && (state != sessionended))
    {
      state = running;

      onTimeout(sendtimeout);
      voIPState = active;
      setTimeout(statetransitiontimeout, 0.02);
    }
}


void
VoIP::onTimeout(const Timeout& _t)
{
  if(_t == sendtimeout)
    {
      MESSAGE_SINGLE(NORMAL, logger, "APPL: Sending voicepacket!");

      applications::session::Session::iatProbesCalculation();

      packetSize = voicePacketSize;

      applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(voicePacketSize), pyco);
      applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());

      ++packetNumber;
      applicationPDU->setPacketNumber(packetNumber, packetFrom);
      MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

      wns::osi::PDUPtr pdu(applicationPDU);
      applications::session::Session::outgoingProbesCalculation(pdu);

      connection->sendData(pdu);
    }
  else if(_t == receivetimeout)
    {
      /* Comfort noise packets will be send to fill the silence */
      MESSAGE_SINGLE(NORMAL, logger, "APPL: Sending comfort noise packet!");

      applications::session::Session::iatProbesCalculation();

      packetSize = comfortNoisePacketSize;

      applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(comfortNoisePacketSize), pyco);
      applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());

      ++packetNumber;
      applicationPDU->setPacketNumber(packetNumber, packetFrom);
      MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

      wns::osi::PDUPtr pdu(applicationPDU);

      applications::session::Session::outgoingProbesCalculation(pdu);
      connection->sendData(pdu);
    }
  else if(_t == statetransitiontimeout)
    {
      /* Transition between active (speech) state and inactive (silent/pause) state. */
      double stateTransitionValue = (*stateTransitionDistribution)();

      MESSAGE_SINGLE(NORMAL, logger, "APPL: State transition. TransitionValue = " << stateTransitionValue << ".");

      if(stateTransitionValue <= 0.01 && voIPState == inactive)
	{
	  cnCounter = 0;
	  onTimeout(sendtimeout);

	  voIPState = active;
	}
      else if(stateTransitionValue > 0.01 && voIPState == inactive)
	{
	  if(cnCounter == 8)
	    {
	      cnCounter = 1;
	      onTimeout(receivetimeout);
	    }
	  else
	    {
	      cnCounter += 1;
	    }
	}
      else if(stateTransitionValue <= 0.01 && voIPState == active)
	{
	  cnCounter = 1;
	  onTimeout(receivetimeout);

	  voIPState = inactive;
	}
      else if(stateTransitionValue > 0.01 && voIPState == active)
	{
	  onTimeout(sendtimeout);
	}

      setTimeout(statetransitiontimeout, 0.02);
    }
  else if(_t == connectiontimeout)
    {
      MESSAGE_SINGLE(NORMAL, logger, "APPL: Opening connection.");
      packetFrom = "client.VoIP";

      /* Open connection */
      binding->initBinding();
    }
  else if(_t == probetimeout)
    {
      applications::session::Session::onTimeout(_t);
    }
  else if(_t == statetimeout)
    {
      /* Start with calling the server. */
      MESSAGE_SINGLE(NORMAL, logger, "APPL: Calling the server!");

      applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(comfortNoisePacketSize), pyco);
      applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());

      packetSize = comfortNoisePacketSize;

      packetNumber = 1;
      applicationPDU->setPacketNumber(packetNumber, packetFrom);
      MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

      wns::osi::PDUPtr pdu(applicationPDU);

      applications::session::Session::outgoingProbesCalculation(pdu);
      connection->sendData(pdu);

      state = idle;
    }
  else
    {
      assure(false, "APPL: Unknown timout type =" << _t);
    }
}



