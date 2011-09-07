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

#ifndef APPLICATIONS_SESSION_CLIENT_WWW_HPP
#define APPLICATIONS_SESSION_CLIENT_WWW_HPP

#include <APPLICATIONS/session/client/Session.hpp>

namespace applications { namespace session { namespace client {
      class WWW :
    public applications::session::client::Session
      {
      public:
	WWW(const wns::pyconfig::View& _pyco);
	~WWW();

	void
	handleWWWDataTransferStart();

	void
	onData(const wns::osi::PDUPtr& _pdu);

	void
	onTimeout(const Timeout& _t);

      private:
	/* Acording to "IEEE 802.16m Evaluation Methodology Document(2009), Page 108-109,
	   section 10.1" the mean parsing time for the main page is 0.13sec.. The values
	   are Exponential distributed. */
	wns::distribution::Distribution* parsingTimeDistribution;
	wns::simulator::Time parsingTime;

	/* Acording to "IEEE 802.16m Evaluation Methodology Document(2009), Page 108-109,
	   section 10.1" the mean reading time 30sec.. The values are Exponential distributed. */
	wns::distribution::Distribution* readingTimeDistribution;
	wns::simulator::Time readingTime;

	/* Acording to "IEEE 802.16m Evaluation Methodology Document(2009), Page 108-109,
	   section 10.1" the mean number of embedded objects per page is 5.64. The values are
	   Pareto distributed. */
	wns::distribution::Distribution* embeddedObjectsPerPageDistribution;
	int embeddedObjectsPerPage;

	bool mainObject;
      };
    } // namespace client
  } // namespace session
} // namespace applications

#endif //APPLICATIONS_SESSION_CLIENT_WWW_HPP
