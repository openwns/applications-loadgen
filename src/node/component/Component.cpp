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

#include <APPLICATIONS/node/component/Component.hpp>

using namespace applications::node::component;

Component::Component(wns::node::Interface* _node,
		     const wns::pyconfig::View& _pyco) :
  wns::node::component::Component(_node, _pyco),
  pyco(_pyco),
  logger(_pyco.get("logger"))
{
}

Component::~Component()
{
}

void
Component::doStartup()
{
}

void
Component::onWorldCreated()
{
}

void
Component::onShutdown()
{
}

wns::service::tl::Service*
Component::getTransportService(int /*transportlayer*/)
{
  //if (getConfig().isNone(TransportLayer::toString(transportlayer)))
  return NULL;
  /*else
    return getService<wns::service::tl::Service*>(getConfig().get<std::string>
    (TransportLayer::toString(transportlayer)));*/
}

wns::service::nl::FQDN
Component::getDomainName()
{
  return domainName;
}

const wns::pyconfig::View&
Component::getPyConfig()
{
  return getConfig();
}

