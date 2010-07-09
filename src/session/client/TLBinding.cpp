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

#include <APPLICATIONS/session/Session.hpp>
#include <APPLICATIONS/session/client/TLBinding.hpp>

#include <WNS/StaticFactory.hpp>
#include <WNS/service/qos/QoSClasses.hpp>

using namespace applications::session::client;

STATIC_FACTORY_REGISTER_WITH_CREATOR(applications::session::client::TLBinding,
				     applications::session::Binding,
				     "TLBinding", wns::PyConfigViewCreator);

TLBinding::TLBinding(const wns::pyconfig::View& _pyco):
  pyco(_pyco),
  listenPort(_pyco.get<int>("listenPort")),
  domainName(_pyco.get<std::string>("domainName")),
  destinationDomainName(_pyco.get<std::string>("destinationDomainName")),
  destinationPort(_pyco.get<int>("destinationPort")),
  qosClass(_pyco.get<int>("qosClass")),
  logger(pyco.get("logger"))
{
  tlService = NULL;
  session = NULL;
}

TLBinding::~TLBinding()
{
  tlService = NULL;
  session = NULL;
}


void
TLBinding::registerComponent(applications::node::component::Component* _component, int _sessionIndex)
{
  tlService =
    _component->getService<wns::service::tl::Service*>(pyco.get<std::string>("tlService"));
  tlService->listenOnPort(listenPort, this);
}

void
TLBinding::onConnectionEstablished(wns::service::nl::Address, wns::service::tl::Connection* _connection)
{
  session->onConnectionEstablished(_connection);
  _connection->registerDataHandler(session);
}


void
TLBinding::registerSession(int _i, applications::session::Session* _session)
{
  session = _session;
}

void
TLBinding::initBinding()
{
  tlService->openConnection(destinationPort, domainName, destinationDomainName, qosClass, this);
}

void
TLBinding::releaseBinding(wns::service::tl::Connection* _connection)
{
  tlService->closeConnection(_connection);
}

void
TLBinding::startListening(wns::service::tl::Service* _tlService)
{
  // Not needed for TLBinding.
}

void
TLBinding::onConnectionClosed(wns::service::tl::Connection*)
{
  // intentionally left empty
}

void
TLBinding::onConnectionClosedByPeer(wns::service::tl::Connection*)
{
  // intentionally left empty
}

void
TLBinding::onConnectionLost(wns::service::tl::Connection*)
{
  // intentionally left empty
}

std::string
TLBinding::printAddress() const
{
  std::ostringstream tmp;
  tmp << destinationDomainName << ":" << destinationPort;
  return tmp.str();
}
