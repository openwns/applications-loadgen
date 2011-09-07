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

#include <APPLICATIONS/session/client/wimax/VideoTelephony.hpp>

using namespace applications::session::client::wimax;

STATIC_FACTORY_REGISTER_WITH_CREATOR(applications::session::client::wimax::VideoTelephony,
				     applications::session::Session,
				     "client.WiMAXVideoTelephony", wns::PyConfigViewCreator);

VideoTelephony::VideoTelephony(const wns::pyconfig::View& _pyco) :
  applications::session::client::Session(_pyco),
  stateTransitionDistribution(NULL),
  iFramePacketSizeDistribution(NULL),
  bFramePacketSizeDistribution(NULL),
  pFramePacketSizeDistribution(NULL),
  bFrameCounter(1),
  gopCounter(1),
  newGOP(true),
  cnCounter(0)
{
  wns::pyconfig::View sTVConfig(_pyco, "stateTransition");
  std::string sTVName = sTVConfig.get<std::string>("__plugin__");
  stateTransitionDistribution = wns::distribution::DistributionFactory::creator(sTVName)->create(sTVConfig);

  wns::pyconfig::View iPSConfig(_pyco, "iFramePacketSize");
  std::string iPSName = iPSConfig.get<std::string>("__plugin__");
  iFramePacketSizeDistribution = wns::distribution::DistributionFactory::creator(iPSName)->create(iPSConfig);

  wns::pyconfig::View bPSConfig(_pyco, "bFramePacketSize");
  std::string bPSName = bPSConfig.get<std::string>("__plugin__");
  bFramePacketSizeDistribution = wns::distribution::DistributionFactory::creator(bPSName)->create(bPSConfig);

  wns::pyconfig::View pPSConfig(_pyco, "pFramePacketSize");
  std::string pPSName = pPSConfig.get<std::string>("__plugin__");
  pFramePacketSizeDistribution = wns::distribution::DistributionFactory::creator(pPSName)->create(pPSConfig);

  settlingTime = _pyco.get<wns::simulator::Time>("settlingTime");

  voicePacketIat = _pyco.get<wns::simulator::Time>("voicePacketIat");
  voicePacketSize = _pyco.get<Bit>("voicePacketSize");
  /* Packet size includes 12 bytes RTP header. */
  voicePacketSize = voicePacketSize + 96;

  comfortNoisePacketSize = _pyco.get<Bit>("comfortNoisePacketSize");
  /* Packet size includes 12 bytes RTP header. */
  comfortNoisePacketSize = comfortNoisePacketSize + 96;

  comfortNoise = _pyco.get<bool>("comfortNoise");

  MESSAGE_BEGIN(NORMAL, logger, m, "APPL: Starting new session with voiceparameters: ");
  m << "voicePacketSize = " << voicePacketSize;
  m << ", voicePacketIat = " << voicePacketIat;
  m << ", comfortNoisePacketSize = " << comfortNoisePacketSize;
  MESSAGE_END();

  videoFrameRate = _pyco.get<int>("frameRate");
  videoPacketIat = 1 / double(videoFrameRate);
  shiftI = _pyco.get<double>("shiftI");

  MESSAGE_BEGIN(NORMAL, logger, m, "APPL: Starting new session with videoparameters: ");
  m << "videoFrameRate = " << videoFrameRate;
  m << ", videoPacketIat = " << videoPacketIat;
  MESSAGE_END();

  state = running;

  /* only for probing */
  sessionType = wimaxvideotelephony;

  sessionDelay = (*sessionDelayDistribution)();
  MESSAGE_SINGLE(NORMAL, logger, "APPL: Delay before session starts: " << sessionDelay << ".\n");

  setTimeout(connectiontimeout, 0.07);
  setTimeout(probetimeout, windowSize);
}


VideoTelephony::~VideoTelephony()
{
  if (stateTransitionDistribution != NULL)
    delete stateTransitionDistribution;
  stateTransitionDistribution = NULL;

  if(iFramePacketSizeDistribution != NULL)
    delete iFramePacketSizeDistribution;
  iFramePacketSizeDistribution = NULL;

  if(bFramePacketSizeDistribution != NULL)
    delete bFramePacketSizeDistribution;
  bFramePacketSizeDistribution = NULL;

  if(pFramePacketSizeDistribution != NULL)
    delete pFramePacketSizeDistribution;
  pFramePacketSizeDistribution = NULL;
}

void
VideoTelephony::onData(const wns::osi::PDUPtr& _pdu)
{
  assureType(_pdu.getPtr(), applications::session::PDU*);

  receivedPacketNumber = static_cast<applications::session::PDU*>(_pdu.getPtr())->getPacketNumber();

  MESSAGE_SINGLE(NORMAL, logger, "APPL: receivedPacketNumber = " << receivedPacketNumber << ".");

  applications::session::Session::incomingProbesCalculation(_pdu);

  if((receivedPacketNumber == 1) && (state != sessionended))
    {
      /* Voice */
      onTimeout(sendtimeout);

      /* Video */
      onTimeout(frametimeout);
      voiceState = active;
      setTimeout(statetransitiontimeout, 0.02);

      state = running;
    }
}

void
VideoTelephony::onTimeout(const Timeout& _t)
{
  /* Voice */
  if(_t == sendtimeout)
    {
      MESSAGE_SINGLE(NORMAL, logger, "APPL: Sending voicepacket!");

      applications::session::Session::iatProbesCalculation();

      packetSize = voicePacketSize;
      applications::session::Session::outgoingProbesCalculation();

      applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(voicePacketSize), pyco);
      applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());

      ++packetNumber;
      applicationPDU->setPacketNumber(packetNumber, packetFrom);
      MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

      wns::osi::PDUPtr pdu(applicationPDU);

      connection->sendData(pdu);
    }
  else if(_t == receivetimeout)
    {
      /* Comfort noise packets will be send to fill the silence */
      MESSAGE_SINGLE(NORMAL, logger, "APPL: Sending comfort noise packet!");

      applications::session::Session::iatProbesCalculation();

      packetSize = comfortNoisePacketSize;
      applications::session::Session::outgoingProbesCalculation();

      applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(comfortNoisePacketSize), pyco);
      applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());

      ++packetNumber;
      applicationPDU->setPacketNumber(packetNumber, packetFrom);
      MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

      wns::osi::PDUPtr pdu(applicationPDU);

      connection->sendData(pdu);
    }
  else if(_t == statetransitiontimeout)
    {
      /* Transition between active (speech) state and inactive (silent/pause) state. */
      double stateTransitionValue = (*stateTransitionDistribution)();

      MESSAGE_SINGLE(NORMAL, logger, "APPL: State transition. TransitionValue = " << stateTransitionValue << ".");

      if(stateTransitionValue <= 0.01 && voiceState == inactive)
	{
	  cnCounter = 0;
	  onTimeout(sendtimeout);

	  voiceState = active;
	}
      else if(stateTransitionValue > 0.01 && voiceState == inactive)
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
      else if(stateTransitionValue <= 0.01 && voiceState == active)
	{
	  cnCounter = 1;
	  onTimeout(receivetimeout);

	  voiceState = inactive;
	}
      else if(stateTransitionValue > 0.01 && voiceState == active)
	{
	  onTimeout(sendtimeout);
	}

      setTimeout(statetransitiontimeout, 0.02);
    }
  /* Video */
  else if(_t == frametimeout)
    {
      if(newGOP == false)
	{
	  if(bFrameCounter <= 2)
	    {
	      /* B-Frame */
	      MESSAGE_SINGLE(NORMAL, logger, "APPL: B-FRAME!");

	      // applications::session::Session::iatProbesCalculation();

	      bFrameCounter += 1;

	      packetSize = (*bFramePacketSizeDistribution)();
	      /* The distributed value is in byte, so it has to be converted to bit. */
	      packetSize *= 8.0;

	      applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
	      applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());

	      packetSize = voicePacketSize;
	      applications::session::Session::outgoingProbesCalculation();

	      ++packetNumber;
	      applicationPDU->setPacketNumber(packetNumber, packetFrom);
	      MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

	      wns::osi::PDUPtr pdu(applicationPDU);

	      connection->sendData(pdu);

	      setTimeout(frametimeout, videoPacketIat);

	    }
	  else
	    {
	      /* P-Frame */
	      bFrameCounter = 1;

	      if(gopCounter < 4)
		{
		  MESSAGE_SINGLE(NORMAL, logger, "APPL: P-FRAME!");

		  // applications::session::Session::iatProbesCalculation();

		  packetSize = (*pFramePacketSizeDistribution)();
		  /* The distributed value is in byte, so it has to be converted to bit. */
		  packetSize *= 8.0;

		  applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
		  applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());

		  applications::session::Session::outgoingProbesCalculation();

		  ++packetNumber;
		  applicationPDU->setPacketNumber(packetNumber, packetFrom);
		  MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

		  wns::osi::PDUPtr pdu(applicationPDU);

		  connection->sendData(pdu);

		  gopCounter += 1;
		  setTimeout(frametimeout, videoPacketIat);
		}
	      else
		{
		  gopCounter = 1;
		  newGOP = true;
		  onTimeout(frametimeout);
		}
	    }
	}
      else
	{
	  /* I-Frame */
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: I-FRAME!");

	  // applications::session::Session::iatProbesCalculation();

	  packetSize = (*iFramePacketSizeDistribution)();
	  /* This value has to be shifted. */
	  packetSize += shiftI;
	  /* The distributed value is in byte, so it has to be converted to bit. */
	  packetSize *= 8.0;

	  applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
	  applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());

	  applications::session::Session::outgoingProbesCalculation();

	  ++packetNumber;
	  applicationPDU->setPacketNumber(packetNumber, packetFrom);
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

	  wns::osi::PDUPtr pdu(applicationPDU);

	  connection->sendData(pdu);

	  newGOP = false;
	  setTimeout(frametimeout, videoPacketIat);
	}
    }
  else if(_t == connectiontimeout)
    {
      packetFrom = "client.WiMAXVideoTelephony";

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
      applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(comfortNoisePacketSize), pyco);
      applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());

      packetSize = comfortNoisePacketSize;
      applications::session::Session::outgoingProbesCalculation();

      packetNumber = 1;
      applicationPDU->setPacketNumber(packetNumber, packetFrom);
      MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

      wns::osi::PDUPtr pdu(applicationPDU);

      connection->sendData(pdu);

      state = idle;
    }
  else
    {
      assure(false, "APPL: Unknown timout type =" << _t);
    }
}

