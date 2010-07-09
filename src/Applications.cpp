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

#include <APPLICATIONS/Applications.hpp>

#include <WNS/pyconfig/Parser.hpp>

using namespace std;
using namespace applications;

STATIC_FACTORY_REGISTER_WITH_CREATOR(Applications, wns::module::Base,
				     "applications", wns::PyConfigViewCreator);

Applications::Applications(const wns::pyconfig::View& _pyConfigView)
	: wns::module::Module<Applications>(_pyConfigView)
{
  //  version = wns::module::VersionInformation(BUILDVINFO);
}

Applications::~Applications()
{}

void
Applications::configure()
{
//	wns::pyconfig::View pyco = getPyConfigView().getView("probes");
//	wns::probe::registerProbesFromDict(pyco);//wird nicht mehr benutzt!
} // configure

void Applications::startUp()
{
} // startUp

void Applications::shutDown()
{
} // shutDown

/*
  Local Variables:
  mode: c++
  fill-column: 80
  c-basic-offset: 4
  c-comment-only-line-offset: 0
  c-tab-always-indent: t
  indent-tabs-mode: t
  tab-width: 4
  End:
*/
