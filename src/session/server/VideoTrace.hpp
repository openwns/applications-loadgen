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

#ifndef APPLICATIONS_SESSION_SERVER_VIDEOTRACE_HPP
#define APPLICATIONS_SESSION_SERVER_VIDEOTRACE_HPP

#include <APPLICATIONS/session/server/Session.hpp>
#include <iostream>
#include <fstream>

namespace applications { namespace session { namespace server {
      class VideoFile;

      class VideoTrace :
    public applications::session::server::Session
      {
      public:
	VideoTrace(const wns::pyconfig::View& _pyco);
	~VideoTrace();

	void
	onData(const wns::osi::PDUPtr& _pdu);

	void
	onTimeout(const Timeout& _t);

      private:
	int videoFrameRate;/* frames/sec. */
	const char* movieChoice;
	VideoFile* videoFile;

	bool firstPacket;
	bool firstPacketNumber;

      };

      class VideoFile :
    public wns::Object
      {
      public:
	VideoFile(const char* movie);
	~VideoFile();

	int  getNextPacket();
	bool lastPacket();
	int getNumberOfFrames();

      private:
	bool lastIndex;
	std::ifstream file;

      };

    } // namespace server
  } // namespace session
} // namespace applications

#endif //APPLICATIONS_SESSION_SERVER_VIDEOTRACE_HPP

