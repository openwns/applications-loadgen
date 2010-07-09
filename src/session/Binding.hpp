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

#ifndef APPLICATIONS_SESSION_BINDING_HPP
#define APPLICATIONS_SESSION_BINDING_HPP

#include <APPLICATIONS/node/component/Component.hpp>

#include <WNS/service/tl/Service.hpp>
#include <WNS/pyconfig/View.hpp>
#include <WNS/osi/PDU.hpp>
#include <WNS/StaticFactory.hpp>
#include <WNS/PyConfigViewCreator.hpp>
#include <WNS/node/component/Component.hpp>
#include <WNS/SmartPtr.hpp>
#include <WNS/RefCountable.hpp>

namespace applications { namespace session {
    class Session;

    class Binding:
    public wns::RefCountable
    {
    public:

      typedef wns::PyConfigViewCreator<Binding, Binding> Creator;
      typedef wns::StaticFactory<Creator> Factory;

      virtual
      ~Binding() {};

      virtual void
      registerComponent(applications::node::component::Component* _component, int _sessionIndex) = 0;

      virtual void
      initBinding() = 0;

      virtual void
      releaseBinding(wns::service::tl::Connection* _connection) = 0;

      virtual void
      registerSession(int _i, applications::session::Session* _session) = 0;

      virtual void
      onShutdown() {};

      friend std::ostream&
      operator <<(std::ostream& os, const Binding& b)
      {
	os << b.printAddress();
	return os;
      }

    protected:
      virtual std::string
      printAddress() const = 0;

    };

    typedef wns::SmartPtr<Binding> BindingPtr;

  } // namespace session
} // namespace appllications

#endif // APPLICATIONS_SESSION_BINDING_HPP
