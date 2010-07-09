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
#include <APPLICATIONS/session/server/TLListenerBinding.hpp>

#include <WNS/StaticFactory.hpp>
#include <WNS/service/qos/QoSClasses.hpp>

using namespace applications::session::server;

STATIC_FACTORY_REGISTER_WITH_CREATOR(applications::session::server::TLListenerBinding,
				     applications::session::Binding,
				     "TLListenerBinding", wns::PyConfigViewCreator);

TLListenerBinding::TLListenerBinding(const wns::pyconfig::View& _pyco):
  pyco(_pyco),
  listenPort(_pyco.get<int>("listenPort")),
  destinationPort(_pyco.get<int>("destinationPort")),
  domainName(_pyco.get<std::string>("domainName")),
  destinationDomainName(_pyco.get<std::string>("destinationDomainName")),
  qosClass(_pyco.get<int>("qosClass")),
  logger(pyco.get("logger")),
  sessionIndex(0)
{
  tlService = NULL;
  component = NULL;
}

TLListenerBinding::~TLListenerBinding()
{
  tlService = NULL;
  component = NULL;
}

void
TLListenerBinding::registerComponent(applications::node::component::Component* _component, int _sessionIndex)
{
  component = _component;
  sessionIndex = _sessionIndex;

  // Start listening!
  tlService =
    component->getService<wns::service::tl::Service*>(pyco.get<std::string>("tlService"));
  tlService->listenOnPort(listenPort, this);
}

void
TLListenerBinding::onConnectionEstablished(wns::service::nl::Address _address, wns::service::tl::Connection* _connection)
{
  ownAddress = _address;

  //create serversession
  wns::pyconfig::View sessionConfig = pyco.get("session");
  std::string plugin = sessionConfig.get<std::string>("__plugin__");

  applications::session::Session::Creator* sessionCreator =
    applications::session::Session::Factory::creator(plugin);
  applications::session::Session* session = sessionCreator->create(sessionConfig);

  // Set probe context
  session->registerComponent(component, sessionIndex);

  // Connect session with its listener
  session->registerListener(this);

  _connection->registerDataHandler(session);

  session->onConnectionEstablished(_connection);

  registerSession(sessionIndex, session);

  ++sessionIndex;
}

void
TLListenerBinding::registerSession(int _i, applications::session::Session* _session)
{
  sessionMap[_i] = _session;
}

void
TLListenerBinding::initBinding()
{
  // Not needed. Client opened connection.
}

void
TLListenerBinding::releaseBinding(wns::service::tl::Connection* _connection)
{
  tlService->closeConnection(_connection);
}

void
TLListenerBinding::onConnectionClosed(wns::service::tl::Connection*)
{
  // no connection that can be closed
  // intentionally left empty
}

void
TLListenerBinding::onConnectionClosedByPeer(wns::service::tl::Connection*)
{
  // no connection that can be closed
  // intentionally left empty
}

void
TLListenerBinding::onConnectionLost(wns::service::tl::Connection*)
{
  // no connection that can be closed
  // intentionally left empty
}

void
TLListenerBinding::onShutdown()
{
  for(mapType::const_iterator it = sessionMap.begin(); it != sessionMap.end(); ++it)
    {
      (it->second)->onShutdown();
    }

    for(mapType::const_iterator it = sessionMap.begin(); it != sessionMap.end(); ++it)
    {
      delete (it->second);
    }
}

std::string
TLListenerBinding::printAddress() const
{
  std::ostringstream tmp;
  tmp << ownAddress << ":" << listenPort;
  return tmp.str();
}


