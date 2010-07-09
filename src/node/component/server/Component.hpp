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

#ifndef APPLICATIONS_NODE_COMPONENT_SERVER_COMPONENT_HPP
#define APPLICATIONS_NODE_COMPONENT_SERVER_COMPONENT_HPP

#include <APPLICATIONS/node/component/Component.hpp>
#include <APPLICATIONS/session/Binding.hpp>

namespace applications { namespace node { namespace component { namespace server {
	class Component :
    public applications::node::component::Component
	{
	public:
	  Component(wns::node::Interface* _node, const wns::pyconfig::View& _pyco);

	  virtual ~Component();
	  virtual void onNodeCreated();

	  void
	  onShutdown();

	  void
	  registerListeners(int _i, applications::session::Binding* _binding);

	  typedef std::map<int, applications::session::Binding*> mapType;
	  mapType bindingMap;

	private:
	  wns::pyconfig::View pyco;
	  wns::logger::Logger logger;
	  applications::session::Binding* listenerBinding;

	};
      } // namespace server
    } // namespace component
  } // namespace node
} // namespace applications

#endif // APPLICATIONS_NODE_COMPONENT_SERVER_COMPONENT_HPP
