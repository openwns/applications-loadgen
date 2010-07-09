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

#ifndef APPLICATIONS_NODE_COMPONENT_COMPONENT_HPP
#define APPLICATIONS_NODE_COMPONENT_COMPONENT_HPP

#include <WNS/node/component/Component.hpp>
#include <WNS/service/tl/Service.hpp>

namespace applications {
  namespace session {
    class Session;
    class Binding;
  } // namespace session
  namespace node { namespace component {
      class Component :
    public wns::node::component::Component
      {
      public:

	Component(wns::node::Interface* _node, const wns::pyconfig::View& _pyco);

	virtual ~Component();

	virtual void
	onWorldCreated();

	virtual void
	onShutdown();

	virtual wns::service::tl::Service*
	getTransportService(int);

	virtual wns::service::nl::FQDN
	getDomainName();

	const wns::pyconfig::View&
	getPyConfig();

	virtual void
	sessionEnded(int _i, applications::session::Session* _session) {};

      protected:
	virtual void
	doStartup();

	wns::logger::Logger logger;
	const wns::pyconfig::View pyco;
	wns::service::nl::FQDN domainName;

      };
    } //namespace component
  } //namespace node
} //namespace applications

#endif // APPLICATIONS_NODE_COMPONENT_COMPONENT_HPP
