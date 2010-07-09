/*******************************************************************************
 * This file is part of openWNS (open Wireless Network Simulator)
 * _____________________________________________________________________________
 *
 * Copyright (C) 2004-2007
 * Chair of Communication Networks (ComNets)
 * Kopernikusstr. 16, D-52074 Aachen, Germany
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

#ifndef APPLICATIONS_DISTRIBUTION_CAUCHY_HPP
#define APPLICATIONS_DISTRIBUTION_CAUCHY_HPP

#include <WNS/distribution/Distribution.hpp>
#include <WNS/distribution/Uniform.hpp>

namespace wns { namespace distribution {

    /* Cauchy disributed random numbers */
    class Cauchy :
    public Distribution
    {
    public:
      explicit
      Cauchy(double median, double sigma, double value,
		 wns::rng::RNGen* rng = wns::simulator::getRNG());

      explicit
      Cauchy(const pyconfig::View& config);

      explicit
      Cauchy(wns::rng::RNGen* rng, const pyconfig::View& config);

      virtual
      ~Cauchy();

      virtual double
      operator()();

      virtual std::string
      paramString() const;

    private:
      double median_;
      double sigma_;
      /* Value of A for the highest %-tile: y = A / (pi * (((x - median)^2) + 1)) */
      double value_;

      wns::distribution::Distribution* uniDis;
    };
  } // distribution
} // applications

#endif // APPLICATIONS_DISTRIBUTION_CAUCHY_HPP
