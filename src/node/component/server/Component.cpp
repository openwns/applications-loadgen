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

#include <APPLICATIONS/node/component/server/Component.hpp>

using namespace applications::node::component::server;

STATIC_FACTORY_REGISTER_WITH_CREATOR(applications::node::component::server::Component,
				     wns::node::component::Interface,
				     "applications.component.Server",
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
  for(int i = 0; i < pyco.len("listenerBindings"); ++i)
    {
      // create the listener's binding, either TCP or UDP
      wns::pyconfig::View listenerBindingConfig = pyco.get("listenerBindings", i);
      std::string plugin = listenerBindingConfig.get<std::string>("__plugin__");
      applications::session::Binding::Creator* listenerBindingCreator =
	applications::session::Binding::Factory::creator(plugin);
      listenerBinding = listenerBindingCreator->create(listenerBindingConfig);

      listenerBinding->registerComponent(this, i);

      registerListeners(i, listenerBinding);
    }
}


void
Component::registerListeners(int _i, applications::session::Binding* _binding)
{
  bindingMap[_i] = _binding;
}

void
Component::onShutdown()
{
  for(mapType::const_iterator it = bindingMap.begin(); it != bindingMap.end(); ++it)
    {
      (it->second)->onShutdown();
    }

  for(mapType::const_iterator it = bindingMap.begin(); it != bindingMap.end(); ++it)
    {
      delete (it->second);
    }
}
