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

#include <APPLICATIONS/session/server/wimax/Video.hpp>

using namespace applications::session::server::wimax;

STATIC_FACTORY_REGISTER_WITH_CREATOR(applications::session::server::wimax::Video,
				     applications::session::Session,
				     "server.WiMAXVideo", wns::PyConfigViewCreator);


using namespace applications::session::server::wimax;

Video::Video(const wns::pyconfig::View& _pyco) :
  Session(_pyco),
  framePacketCounter(0),
  firstPacketNumber(true)
{
  wns::pyconfig::View pSDConfig(_pyco, "packetSize");
  std::string pSDName = pSDConfig.get<std::string>("__plugin__");
  packetSizeDistribution = wns::distribution::DistributionFactory::creator(pSDName)->create(pSDConfig);

  wns::pyconfig::View iATConfig(_pyco, "packetIat");
  std::string iATName = iATConfig.get<std::string>("__plugin__");
  packetIatDistribution = wns::distribution::DistributionFactory::creator(iATName)->create(iATConfig);

  settlingTime = _pyco.get<wns::simulator::Time>("settlingTime");

  videoFrameRate = _pyco.get<double>("frameRate");
  videoFrameIat = 1 / double(videoFrameRate);
  numberOfPacketsPerFrame = _pyco.get<double>("numberOfPacketsPerFrame");

  MESSAGE_BEGIN(NORMAL, logger, m, "APPL: Starting new session with: ");
  m << "videoFrameRate = " << videoFrameRate;
  m << ", videoFrameIat = " << videoFrameIat;
  m << ", numberOfPackets = " << numberOfPacketsPerFrame;
  MESSAGE_END();

  state = idle;

  /* only for probing */
  sessionType = wimaxvideo;

  packetFrom = "server.WiMAXVideo";
}

Video::~Video()
{
  if(packetSizeDistribution != NULL)
    delete packetSizeDistribution;
  packetSizeDistribution = NULL;

  if(packetIatDistribution != NULL)
    delete packetIatDistribution;
  packetIatDistribution = NULL;
}

void
Video::onData(const wns::osi::PDUPtr& _pdu)
{
  state = running;

  assureType(_pdu.getPtr(), applications::session::PDU*);

  receivedPacketNumber = static_cast<applications::session::PDU*>(_pdu.getPtr())->getPacketNumber();

  MESSAGE_SINGLE(NORMAL, logger, "APPL: receivedPacketNumber = " << receivedPacketNumber << ".");

  applications::session::Session::incomingProbesCalculation(_pdu);

  onTimeout(frametimeout);
}

void
Video::onTimeout(const Timeout& _t)
{
  if(_t == statetimeout)
    {
      if(framePacketCounter < numberOfPacketsPerFrame)
	{
	  framePacketCounter += 1;

	  packetSize = (*packetSizeDistribution)();
	  /* The distributed value is in byte, so it has to be converted to bit. */
	  packetSize *= 8.0;

	  MESSAGE_SINGLE(NORMAL, logger, "APPL: Sending video packet!");

	  applications::session::Session::iatProbesCalculation();

	  applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
	  applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());

	  applications::session::Session::outgoingProbesCalculation();

	  if(firstPacketNumber == true)
	    {
	      packetNumber = 1;
	      applicationPDU->setPacketNumber(packetNumber, packetFrom);
	      MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

	      firstPacketNumber = false;
	    }
	  else
	    {
	      ++packetNumber;
	      applicationPDU->setPacketNumber(packetNumber, packetFrom);
	      MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");
	    }

	  wns::osi::PDUPtr pdu(applicationPDU);

	  connection->sendData(pdu);

	  iat = (*packetIatDistribution)();
	  /* The distributed value is in ms, so it has to be converted to sec. */
	  iat /= 1000.0;

	  setTimeout(statetimeout, iat);
	}
      else
	{
	  framePacketCounter = 0;
	}
    }
  else if(_t == frametimeout)
    {
      /* Starting new frame! */
      setTimeout(frametimeout, videoFrameIat);
      onTimeout(statetimeout);
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
