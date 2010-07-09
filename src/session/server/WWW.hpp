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

#ifndef APPLICATIONS_SESSION_SERVER_WWW_HPP
#define APPLICATIONS_SESSION_SERVER_WWW_HPP

#include <APPLICATIONS/session/server/Session.hpp>

namespace applications { namespace session { namespace server {
      class WWW :
    public applications::session::server::Session
      {
      public:
	WWW(const wns::pyconfig::View& _pyco);
	~WWW();

	void
	onData(const wns::osi::PDUPtr& _pdu);

	void
	onTimeout(const Timeout& _t);

      private:
	/* Acording to "IEEE 802.16m Evaluation Methodology Document(2009), Page 108-109,
	   section 10.1" the mean size of the main object is 10710 bytes. The values
	   are Lognormal distributed. */
	wns::distribution::Distribution* sizeOfMainObjectDistribution;
	/* Acording to "IEEE 802.16m Evaluation Methodology Document(2009), Page 108-109,
	   section 10.1" the mean size of an embedded object is 7758 bytes. The values
	   are Lognormal distributed. */
	wns::distribution::Distribution* sizeOfEmbeddedObjectDistribution;

	bool mainObject;
      };
    } // namespace server
  } // namespace session
} // namespace applications

#endif //APPLICATIONS_SESSION_SERVER_WWW_HPP
