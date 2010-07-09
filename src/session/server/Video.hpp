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

#ifndef APPLICATIONS_SESSION_SERVER_VIDEO_HPP
#define APPLICATIONS_SESSION_SERVER_VIDEO_HPP

#include <APPLICATIONS/session/server/Session.hpp>

namespace applications { namespace session { namespace server {
      class Video :
    public applications::session::server::Session
      {
      public:
	Video(const wns::pyconfig::View& _pyco);
	virtual ~Video();

	void logNormalProjectedFarima(std::deque<double> _x, std::deque<double> _phiold, double _y,
				      double _phi, double _theta, double _m, double _sqrValue,
				      double _variance, int _k, double _logMean, double _logStd);
	void logNormalPDF(double _logMean, double _logStd);
	void logNormalProjectedAR(double _y, double _phi, double _m, double _sqrValue,
				  int _k, double _logMean, double _logStd);
	void logNormalProjectedARMA(double _y, double _phi, double _theta, double _m,
				    double _sqrValue, int _k, double _logMean, double _logStd);
	void logNormalProjectedFDN(std::deque<double> _x, std::deque<double> _phiold, double _y,
				   double _phi, double _theta, double _m, double _sqrValue,
				   double _variance, int _k, double _logMean, double _logStd);
	void videoAR(double _y, double _phi, double _logMean, double _logStd);
	void videoFARIMA(std::deque<double> _x, std::deque<double> _phiold, double _y, double _phi,
			 double _theta, double _variance, int _k, double _logMean, double _logStd);
	void videoFDN(std::deque<double> _x, std::deque<double> _phiold, double _y, double _variance,
		      int _k, double _logMean, double _logStd);

	void
	onData(const wns::osi::PDUPtr& _pdu);

	void
	onTimeout(const Timeout& _t);

	void
	calculateSLRangeDependency();

      protected:
	int i;
	int j;
	int idx;
	int limit;
	/*****Video Related*****/
	wns::simulator::Time videoPacketIat;
	int videoFrameRate;

	/***** I Frame Generating*****/
	double fracI;
	double hurstI;
	int kI;
	std::deque<double> autocorrI;
	std::deque<double> phiInew;
	std::deque<double> phiIold;
	double pI;
	double nI;
	double dI;
	/* According to "WIMAX System Evaluation Methodologie, Version 1.0, 30.01.2007",
	   the I-frame packetsize is Lognormal distributed. */
	wns::distribution::Distribution* logNormDisI;
	double logNormValueI;
	double logMeanI;
	double logStdI;
	double meanI;
	double varianceI;
	std::deque<double> xI;
	double yI;
	double phiI;
	double thetaI;
	double mI;
	double sqrValueI;

	/***** B Frame Generating*****/
	double fracB;
	double hurstB;
	int kB;
	std::deque<double> autocorrB;
	std::deque<double> phiBnew;
	std::deque<double> phiBold;
	double pB;
	double nB;
	double dB;
	/* According to "WIMAX System Evaluation Methodologie, Version 1.0, 30.01.2007",
	   the B-frame packetsize is Lognormal  distributed. */
	wns::distribution::Distribution* logNormDisB;
	double logNormValueB;
	double logMeanB;
	double logStdB;
	double meanB;
	double varianceB;
	std::deque<double> xB;
	double yB;
	double phiB;
	double thetaB;
	double mB;
	double sqrValueB;

	/***** P Frame Generating*****/
	double fracP;
	double hurstP;
	int kP;
	std::deque<double> autocorrP;
	std::deque<double> phiPnew;
	std::deque<double> phiPold;
	double pP;
	double nP;
	double dP;
	/* According to "WIMAX System Evaluation Methodologie, Version 1.0, 30.01.2007",
	   the P-frame packetsize is Lognormal  distributed. */
	wns::distribution::Distribution* logNormDisP;
	double logNormValueP;
	double logMeanP;
	double logStdP;
	double meanP;
	double varianceP;
	std::deque<double> xP;
	double yP;
	double phiP;
	double thetaP;
	double mP;
	double sqrValueP;

      private:
	/*****Video Related*****/
	bool byteFlag;
	std::string frameGeneratingProcess;

	/* Used in methods */
	double mean;
	int n;
	wns::distribution::Distribution* logNormDis;
	double logNormValue;
	wns::distribution::Distribution* normDis;
	double normValue;
	double normMean;
	double normStd;
	double projection;
	double value;
	double logNormVariance;

	int bFrameCounter;
	int gopCounter;
	bool newGOP;

	bool firstPacketNumber;

      };
    } // namespace server
  } // namespace session
} // namespace applications

#endif //APPLICATIONS_SESSION_SERVER_VIDEO_HPP
