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

#include <APPLICATIONS/node/component/client/Component.hpp>

using namespace applications::node::component::client;

STATIC_FACTORY_REGISTER_WITH_CREATOR(applications::node::component::client::Component,
				     wns::node::component::Interface,
				     "applications.component.Client",
				     wns::node::component::ConfigCreator);

Component::Component(wns::node::Interface* _node,
		     const wns::pyconfig::View& _pyco) :
  applications::node::component::Component(_node, _pyco),
  pyco(_pyco),
  logger(_pyco.get("logger"))
{
}

Component::~Component()
{
}

void
Component::onNodeCreated()
{
  // Create client sessions which are configured
  for(int i = 0; i < pyco.len("sessions"); ++i)
    {
      // create all sessions
      wns::pyconfig::View sessionConfig = pyco.get("sessions", i);
      std::string plugin = sessionConfig.get<std::string>("__plugin__");
      applications::session::Session::Creator* sessionCreator =
	applications::session::Session::Factory::creator(plugin);
      applications::session::Session* session = sessionCreator->create(sessionConfig);

      // create the session's binding, either TCP or UDP
      wns::pyconfig::View bindingConfig = pyco.get("bindings", i);
      plugin = bindingConfig.get<std::string>("__plugin__");
      applications::session::Binding::Creator* bindingCreator =
	applications::session::Binding::Factory::creator(plugin);
      applications::session::BindingPtr binding(bindingCreator->create(bindingConfig));

      // Connect session with its binding
      session->registerBinding(binding);

      // Set probe context
      session->registerComponent(this, i);

      binding->registerSession(i, session);

      binding->registerComponent(this, i);

      registerSessions(i, session);
    }
}


void
Component::onShutdown()
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

void
Component::registerSessions(int _i, applications::session::Session* _session)
{
  sessionMap[_i] = _session;
}
