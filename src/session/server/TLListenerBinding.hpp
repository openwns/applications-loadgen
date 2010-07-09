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

#ifndef APPLICATIONS_SESSION_SERVER_TLLISTENERBINDING_HPP
#define APPLICATIONS_SESSION_SERVER_TLLISTENERBINDING_HPP

#include <APPLICATIONS/session/Binding.hpp>

#include <WNS/service/nl/Address.hpp>
#include <WNS/service/tl/ConnectionHandler.hpp>
#include <WNS/service/tl/Connection.hpp>
#include <WNS/service/tl/PortPool.hpp>

#include <WNS/PyConfigViewCreator.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/module/Base.hpp>


namespace applications { namespace session { namespace server {

      class TLListenerBinding:
    public applications::session::Binding,
    public wns::service::tl::ConnectionHandler
      {
      public:
	TLListenerBinding(const wns::pyconfig::View& _pyco);
	virtual ~TLListenerBinding();

	// from Binding
	virtual void
	registerSession(int _i, applications::session::Session* _session);

	virtual void
	registerComponent(applications::node::component::Component* _component, int _sessionIndex);

	virtual void
	initBinding();

	virtual void
	releaseBinding(wns::service::tl::Connection* _connection);

	// from ConnectionHandler
	virtual void
	onConnectionEstablished(wns::service::nl::Address _sourceIpAddress,
				wns::service::tl::Connection* _newConnection);

	virtual void
	onConnectionClosed(wns::service::tl::Connection* _connection);

	virtual void
	onConnectionClosedByPeer(wns::service::tl::Connection* _connection);

	virtual void
	onConnectionLost(wns::service::tl::Connection* _connection);

	virtual void
	onShutdown();

	typedef std::map<int, applications::session::Session*> mapType;
	mapType sessionMap;

      protected:
	applications::node::component::Component* component;

	wns::service::tl::Service* tlService;

	wns::pyconfig::View pyco;

	wns::service::nl::Address ownAddress;

	wns::service::tl::Port listenPort;

	wns::service::nl::FQDN domainName;

	wns::service::nl::FQDN destinationDomainName;

	wns::service::tl::Port destinationPort;

	wns::service::qos::QoSClass qosClass;

	int sessionIndex;

	/**
	 * @brief The Logger instance.
	 */
	wns::logger::Logger logger;

	virtual std::string	printAddress() const;

      };

      typedef wns::SmartPtr<TLListenerBinding> TLListenerBindingPtr;

    } // namespace server
  } // namespace session
} // namespace applications

#endif // APPLICATIONS_SESSION_SERVER_TLLISTENERBINDING_HPP
