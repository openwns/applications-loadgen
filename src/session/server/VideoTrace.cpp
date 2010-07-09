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

#include <APPLICATIONS/session/server/VideoTrace.hpp>

using namespace applications::session::server;

STATIC_FACTORY_REGISTER_WITH_CREATOR(applications::session::server::VideoTrace,
				     applications::session::Session,
				     "server.VideoTrace", wns::PyConfigViewCreator);

VideoTrace::VideoTrace(const wns::pyconfig::View& _pyco) :
  Session(_pyco),
  videoFrameRate(25),
  videoFile(NULL),
  movieChoice(""),
  firstPacket(true),
  firstPacketNumber(true)
{
  iat = 1 / double(videoFrameRate);

  settlingTime = _pyco.get<wns::simulator::Time>("settlingTime");

  /* only for probing */
  sessionType = videotrace;

  packetFrom = "server.VideoTrace";
}


VideoTrace::~VideoTrace()
{
}


void
VideoTrace::onData(const wns::osi::PDUPtr& _pdu)
{
  state = running;

  assureType(_pdu.getPtr(), applications::session::PDU*);

  receivedPacketNumber = static_cast<applications::session::PDU*>(_pdu.getPtr())->getPacketNumber();

  MESSAGE_SINGLE(NORMAL, logger, "APPL: receivedPacketNumber = " << receivedPacketNumber << ".");

  applications::session::Session::incomingProbesCalculation(_pdu);

  if(receivedPacketNumber == 1)
    {
      firstPacketDelay = packetDelay;

      movieChoice = static_cast<applications::session::PDU*>(_pdu.getPtr())->getMovieChoice();
    }
  onTimeout(statetimeout);
}


void
VideoTrace::onTimeout(const Timeout& _t)
{
  if(_t == statetimeout)
    {
      /* Sending requested videofile. */

      if(firstPacket == true)
	{
	  firstPacket = false;

	  MESSAGE_SINGLE(NORMAL, logger, "APPL: MovieChoice = " << movieChoice << ".");

	  videoFile = new VideoFile(movieChoice);
	}

      lastPacket = videoFile->VideoFile::lastPacket();

      if(lastPacket == false)
	{
	  applications::session::Session::iatProbesCalculation();

	  packetSize = videoFile->VideoFile::getNextPacket();

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

	  setTimeout(statetimeout, iat);
	}
      else
	{
	  /* Last packet sent, now wait till session ends. */
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: Last packet of tracefile sent. Wait till session ends. ");
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



VideoFile::VideoFile(const char* _movie) :
  lastIndex(false)
{
  file.open(_movie, std::ios::out);

  assure(file != NULL, "APPL: Video file was not opened. Please check if it is available!");
}


VideoFile::~VideoFile()
{
}


int
VideoFile::getNextPacket()
{
  int packetSize = 1;
  char line[100];

  if(!file.eof())
    {
      file.getline(line, 100);
      if (isdigit(line[0]))
	{
	  sscanf(line, "%d %*f", &packetSize);
	}
      else
	{
	  lastIndex = true;
	  file.close();
	}
    }
  else
    {
      lastIndex = true;
      file.close();
    }
  return packetSize;
}


bool
VideoFile::lastPacket()
{
  return lastIndex;
}


int
VideoFile::getNumberOfFrames()
{
  int count = 0;
  int p;
  char line[100];

  while(!file.eof())
    {
      file.getline(line, 100);
      if(isdigit(line[0]))
	{
	  sscanf(line, "%d %*f", &p);
	  count = count + 1;
	}
    }
  return count;
}
