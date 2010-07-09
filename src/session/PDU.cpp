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

#include <APPLICATIONS/session/PDU.hpp>

using namespace applications::session;

PDU::PDU(Bit _packetSize, const wns::pyconfig::View& _pyco):
  wns::osi::PDU(new wns::osi::PCI),
  lastPacket(false),
  creationTime(0.0),
  fileLength(0.0),
  request(false),
  movieChoice(""),
  packetNumber(0)
{
  getPCI()->setPDULength(_packetSize);
}

PDU::~PDU()
{
}

Bit
PDU::doGetLengthInBits() const
{
  return getPCI()->getPDULength();
}

void
PDU::setLastPacket(bool _lastPacket)
{
  lastPacket = _lastPacket;
}

bool
PDU::getLastPacket()
{
  return lastPacket;
}

void
PDU::setCreationTime(wns::simulator::Time _creationTime)
{
  creationTime = _creationTime;
}

wns::simulator::Time
PDU::getCreationTime()
{
  return creationTime;
}

void
PDU::setFileLength(Bit _length)
{
  fileLength = _length;
}

Bit
PDU::getFileLength()
{
  return fileLength;
}

void
PDU::setRequest(bool _request)
{
  request = _request;
}

bool
PDU::getRequest()
{
  return request;
}

void
PDU::setMovieChoice(const char* _movieChoice)
{
  movieChoice = _movieChoice;
}

const char*
PDU::getMovieChoice()
{
  return movieChoice;
}

void
PDU::setPacketNumber(int _packetNumber, std::string _packetFrom)
{
  packetNumber = _packetNumber;
  packetFrom = _packetFrom;
}

int
PDU::getPacketNumber()
{
  return packetNumber;
}

std::string
PDU::getPacketFrom()
{
  return packetFrom;
}
