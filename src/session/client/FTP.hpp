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

#ifndef APPLICATIONS_SESSION_CLIENT_FTP_HPP
#define APPLICATIONS_SESSION_CLIENT_FTP_HPP

#include <APPLICATIONS/session/client/Session.hpp>

namespace applications { namespace session { namespace client {

      /* FTPStates are used to see Sessionphase */
      enum ClientFTPState
	{
	  reading = 1,
	  requestftpok = 2,
	  waitforftpok = 3,
	  waitforusernameok = 4,
	  waitforpasswordok = 5,
	  requestdata = 6
	};

      class FTP :
    public applications::session::client::Session
      {
      public:
	FTP(const wns::pyconfig::View& _pyco);
	~FTP();

	void
	onData(const wns::osi::PDUPtr& _pdu);

	void
	onTimeout(const Timeout& _t);

	void
	onConnectionEstablished(wns::service::tl::Connection* _connection);

      private:
	applications::session::client::ClientFTPState ftpState;

	/* According to "IEEE 802.16m Evaluation Methodology Document(2009), Page 110-111, section 10.2"
	   the mean size of one packet is 2MB and the maximumsize of one packet is 5MB. The values are
	   Lognormal distributed. */
	wns::distribution::Distribution* amountOfDataDistribution;

	/* According to "IEEE 802.16m Evaluation Methodology Document(2009), Page 110-111,
	   section 10.2" the mean readingtime of one received file is 180sec.. The values are
	   Exponential distributed. */
	wns::distribution::Distribution* readingTimeDistribution;

	/* Number of files client wants to download. The client first uploads one file and than requests two files.*/
	int numberOfFiles;

	/* PacketSize of the requested file. */
	Bit filePacketSize;

	bool request;
      };
    } // namespace client
  } // namespace session
} // namespace applications

#endif //APPLICATIONS_SESSION_CLIENT_FTP_HPP
