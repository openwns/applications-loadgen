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

#ifndef APPLICATIONS_SESSION_CLIENT_VIDEOTELEPHONY_HPP
#define APPLICATIONS_SESSION_CLIENT_VIDEOTELEPHONY_HPP

#include <APPLICATIONS/session/client/Session.hpp>

namespace applications { namespace session { namespace client {

      /* VideoTelephonyStates are used to see the voice Sessionphase */
      enum VideoTelephonyState
	{
	  active = 1,
	  inactive = 2
	};

      class VideoTelephony :
    public applications::session::client::Session
      {
      public:
	VideoTelephony(const wns::pyconfig::View& _pyco);
	~VideoTelephony();

	void
	logNormalProjectedFarima(std::deque<double> _x, std::deque<double> _phiold, double _y,
				 double _phi, double _theta, double _m, double _sqrValue,
				 double _variance, int _k, double _logMean, double _logStd);

	void
	onData(const wns::osi::PDUPtr& _pdu);

	void
	onTimeout(const Timeout& _t);

	void
	onConnectionEstablished(wns::service::tl::Connection* _connection);

	void
	calculateSLRangeDependency();

      private:
	bool byteFlag;
	int i;
	int j;
	int limit;
	int idx;

	/***** I Frame Generating *****/
	double fracI;
	double hurstI;
	int kI;
	std::deque<double> autocorrI;
	std::deque<double> phiInew;
	std::deque<double> phiIold;
	double pI;
	double nI;
	double dI;
	/* According to "IEEE 802.16m Evaluation Methodology Document(2009), Page 118,
	   section 10.5", the I-frame packetsize is Weibull distributed. */
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
	/* According to "IEEE 802.16m Evaluation Methodology Document(2009), Page 118,
	   section 10.5", the B-frame packetsize is Lognormal distributed. */
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
	/* According to "IEEE 802.16m Evaluation Methodology Document(2009), Page 118,
	   section 10.5", the P-frame packetsize is Lognormal distributed. */
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

	/* Used in logNormalProjectedFarima() */
	double mean;
	int n;
	wns::distribution::Distribution* distribution;
	double distributionValue;
	double normMean;
	double variance;

	/* Voice parameters */
	bool comfortNoise;
	Bit voicePacketSize;
	Bit comfortNoisePacketSize;

	wns::simulator::Time voicePacketIat;

	wns::simulator::Time videoPacketIat;
	int videoFrameRate;

	private:
	/* According to "IMT-Adv. ITU-R M.2135" every 20ms it is decided if a state transition takes place. */
	wns::distribution::Distribution* stateTransitionDistribution;
	int cnCounter;

	applications::session::client::VideoTelephonyState voiceState;

	int bFrameCounter;
	int gopCounter;
	bool newGOP;

      };
    } // namespace client
  } // namespace session
} // namespace applications

#endif //APPLICATIONS_SESSION_CLIENT_VIDEOTELEPHONY
