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

#include <APPLICATIONS/session/server/Video.hpp>
#include <math.h>
#include <WNS/distribution/Norm.hpp>
#include <WNS/distribution/LogNorm.hpp>

using namespace applications::session::server;

STATIC_FACTORY_REGISTER_WITH_CREATOR(applications::session::server::Video,
				     applications::session::Session,
				     "server.Video", wns::PyConfigViewCreator);

Video::Video(const wns::pyconfig::View& _pyco) :
  Session(_pyco),
  j(0),
  idx(0),
  limit(1000),

  kI(1),
  pI(0.0),
  nI(0.0),
  dI(1.0),
  meanI(0.0),
  varianceI(1.0),
  logNormValueI(0.0),
  yI(0.0),
  mI(0.0),
  sqrValueI(0.0),
  autocorrI(NULL),
  phiInew(NULL),
  phiIold(NULL),

  kB(1),
  pB(0.0),
  nB(0.0),
  dB(1.0),
  meanB(0.0),
  varianceB(1.0),
  logNormValueB(0.0),
  yB(0.0),
  mB(0.0),
  sqrValueB(0.0),
  autocorrB(NULL),
  phiBnew(NULL),
  phiBold(NULL),

  kP(1),
  pP(0.0),
  nP(0.0),
  dP(1.0),
  meanP(0.0),
  varianceP(1.0),
  logNormValueP(0.0),
  yP(0.0),
  mP(0.0),
  sqrValueP(0.0),
  autocorrP(NULL),
  phiPnew(NULL),
  phiPold(NULL),

  mean(0.0),
  n(0),
  logNormDis(NULL),
  logNormDisI(NULL),
  logNormDisB(NULL),
  logNormDisP(NULL),
  logNormValue(0.0),
  normMean(0.0),
  normStd(0.0),
  projection(0.0),
  logNormVariance(0.0),
  value(0.0),

  bFrameCounter(1),
  gopCounter(1),
  newGOP(true)
{
  frameGeneratingProcess = _pyco.get<std::string>("frameGeneratingProcess");

  settlingTime = _pyco.get<wns::simulator::Time>("settlingTime");

  byteFlag = _pyco.get<bool>("params.byteFlag");
  videoFrameRate = _pyco.get<int>("params.frameRate");
  videoPacketIat = 1 / double(videoFrameRate);

  MESSAGE_BEGIN(NORMAL, logger, m, "APPL: Starting new session with ");
  m << "videoFrameRate = " << videoFrameRate;
  m << ", videoPacketIat = " << videoPacketIat;
  MESSAGE_END();

  logMeanI = _pyco.get<double>("params.meanI");
  logStdI = _pyco.get<double>("params.stdI");
  hurstI = _pyco.get<double>("params.hurstI");
  phiI = _pyco.get<double>("params.phiI");
  thetaI = _pyco.get<double>("params.thetaI");

  logMeanP = _pyco.get<double>("params.meanP");
  logStdP = _pyco.get<double>("params.stdP");
  hurstP = _pyco.get<double>("params.hurstP");
  phiP = _pyco.get<double>("params.phiP");
  thetaP = _pyco.get<double>("params.thetaP");

  logMeanB = _pyco.get<double>("params.meanB");
  logStdB = _pyco.get<double>("params.stdB");
  hurstB = _pyco.get<double>("params.hurstB");
  phiB = _pyco.get<double>("params.phiB");
  thetaB = _pyco.get<double>("params.thetaB");

  state = idle;

  /* only for probing */
  sessionType = video;

  packetFrom = "server.Video";
}

Video::~Video()
{
  autocorrI.clear();
  phiInew.clear();
  phiIold.clear();
  xI.clear();

  autocorrB.clear();
  phiBnew.clear();
  phiBold.clear();
  xB.clear();

  autocorrP.clear();
  phiPnew.clear();
  phiPold.clear();
  xP.clear();
}

void
Video::onData(const wns::osi::PDUPtr& _pdu)
{
  /* Request received. */
  state = running;

  assureType(_pdu.getPtr(), applications::session::PDU*);

  receivedPacketNumber = static_cast<applications::session::PDU*>(_pdu.getPtr())->getPacketNumber();

  MESSAGE_SINGLE(NORMAL, logger, "APPL: receivedPacketNumber = " << receivedPacketNumber << ".");

  applications::session::Session::incomingProbesCalculation(_pdu);

  calculateSLRangeDependency();

  onTimeout(frametimeout);
}

void
Video::onTimeout(const Timeout& _t)
{
  if(_t == frametimeout)
    {
      if(newGOP == false)
	{
	  if(bFrameCounter <= 2)
	    {
	      /* B-Frame */
	      MESSAGE_SINGLE(NORMAL, logger, "APPL: B-FRAME!");

	      applications::session::Session::iatProbesCalculation();

	      bFrameCounter += 1;

	      if(frameGeneratingProcess == "PDF")
		{
		  logNormalPDF(logMeanB, logStdB);
		}
	      else if(frameGeneratingProcess == "P-AR")
		{
		  logNormalProjectedAR(yB, phiB, mB, sqrValueB, kB, logMeanB, logStdB);
		}
	      else if(frameGeneratingProcess == "P-ARMA")
		{
		  logNormalProjectedARMA(yB, phiB, thetaB, mB, sqrValueB, kB, logMeanB, logStdB);
		}
	      else if(frameGeneratingProcess == "P-FARIMA")
		{
		  logNormalProjectedFarima(xB, phiBold, yB, phiB, thetaB, mB, sqrValueB,
					   varianceB, kB, logMeanB, logStdB);
		}
	      else if(frameGeneratingProcess == "P-FDN")
		{
		  logNormalProjectedFDN(xB, phiBold, yB, phiB, thetaB, mB, sqrValueB,
					varianceB, kB, logMeanB, logStdB);
		}
	      else if(frameGeneratingProcess == "AR")
		{
		  videoAR(yB, phiB, logMeanB, logStdB);
		}
	      else if(frameGeneratingProcess == "FARIMA")
		{
		  videoFARIMA(xB, phiBold, yB, phiB, thetaB, varianceB, kB, logMeanB, logStdB);
		}
	      else if(frameGeneratingProcess == "FDN")
		{
		  videoFDN(xB, phiBold, yB, varianceB, kB, logMeanB, logStdB);
		}
	      else
		{
		  MESSAGE_SINGLE(NORMAL, logger, "APPL: No video B-frame generator!");
		}

	      setTimeout(frametimeout, videoPacketIat);
	    }
	  else
	    {
	      // P-Frame
	      bFrameCounter = 1;

	      if(gopCounter < 4)
		{
		  MESSAGE_SINGLE(NORMAL, logger, "APPL: P-FRAME!");

		  applications::session::Session::iatProbesCalculation();

		  gopCounter += 1;

		  if(frameGeneratingProcess == "PDF")
		    {
		      logNormalPDF(logMeanP, logStdP);
		    }
		  else if(frameGeneratingProcess == "P-AR")
		    {
		      logNormalProjectedAR(yP, phiP, mP, sqrValueP, kP, logMeanP, logStdP);
		    }
		  else if(frameGeneratingProcess == "P-ARMA")
		    {
		      logNormalProjectedARMA(yP, phiP, thetaP, mP, sqrValueP, kP, logMeanP, logStdP);
		    }
		  else if(frameGeneratingProcess == "P-FARIMA")
		    {
		      logNormalProjectedFarima(xP, phiPold, yP, phiP, thetaP, mP, sqrValueP,
					       varianceP, kP, logMeanP, logStdP);
		    }
		  else if(frameGeneratingProcess == "P-FDN")
		    {
		      logNormalProjectedFDN(xP, phiPold, yP, phiP, thetaP, mP, sqrValueP,
					    varianceP, kP, logMeanP, logStdP);
		    }
		  else if(frameGeneratingProcess == "AR")
		    {
		      videoAR(yP, phiP, logMeanP, logStdP);
		    }
		  else if(frameGeneratingProcess == "FARIMA")
		    {
		      videoFARIMA(xP, phiPold, yP, phiP, thetaP, varianceP, kP, logMeanP, logStdP);
		    }
		  else if(frameGeneratingProcess == "FDN")
		    {
		      videoFDN(xP, phiPold, yP, varianceP, kP, logMeanP, logStdP);
		    }
		  else
		    {
		      MESSAGE_SINGLE(NORMAL, logger, "APPL: No video P-frame generator!");
		    }

		  setTimeout(frametimeout, videoPacketIat);
		}
	      else
		{
		  gopCounter = 1;
		  newGOP = true;
		  onTimeout(frametimeout);
		}
	    }
	}
      else
	{
	  // I-Frame
	  MESSAGE_SINGLE(NORMAL, logger, "I-FRAME!");

	  applications::session::Session::iatProbesCalculation();

	  if(frameGeneratingProcess == "PDF")
	    {
	      logNormalPDF(logMeanI, logStdI);
	    }
	  else if(frameGeneratingProcess == "P-AR")
	    {
	      logNormalProjectedAR(yI, phiI, mI, sqrValueI, kI, logMeanI, logStdI);
	    }
	  else if(frameGeneratingProcess == "P-ARMA")
	    {
	      logNormalProjectedARMA(yI, phiI, thetaI, mI, sqrValueI, kI, logMeanI, logStdI);
	    }
	  else if(frameGeneratingProcess == "P-FARIMA")
	    {
	      logNormalProjectedFarima(xI, phiIold, yI, phiI, thetaI, mI, sqrValueI,
				       varianceI, kI, logMeanI, logStdI);
	    }
	  else if(frameGeneratingProcess == "P-FDN")
	    {
	      logNormalProjectedFDN(xI, phiIold, yI, phiI, thetaI, mI, sqrValueI,
				    varianceI, kI, logMeanI, logStdI);
	    }
	  else if(frameGeneratingProcess == "AR")
	    {
	      videoAR(yI, phiI, logMeanI, logStdI);
	    }
	  else if(frameGeneratingProcess == "FARIMA")
	    {
	      videoFARIMA(xI, phiPold, yI, phiI, thetaI, varianceI, kI, logMeanI, logStdI);
	    }
	  else if(frameGeneratingProcess == "FDN")
	    {
	      videoFDN(xI, phiIold, yI, varianceI, kI, logMeanI, logStdI);
	    }
	  else
	    {
	      MESSAGE_SINGLE(NORMAL, logger, "APPL: No video I-frame generator!");
	    }

	  newGOP = false;

	  setTimeout(frametimeout, videoPacketIat);
	}
    }
  else if(_t == probetimeout)
    {
      applications::session::Session::onTimeout(_t);
    }
  else
    {
      assure(false, "APPL: Unknown timout type =" << _t);
    }
}


void
Video::calculateSLRangeDependency()
{
  /* Callculation of the short- and longrangedependencies.
     For detailed discription see diplomathesis of Cem Mengi 10.07.2006:
     "Conception and implementation of traffic Models for VoIP and Videotelephony". */
  MESSAGE_SINGLE(NORMAL, logger, "APPL: Callculation of the short- and longrange dependencies!");

  fracI = hurstI - 0.5;
  fracP = hurstP - 0.5;
  fracB = hurstB - 0.5;

  autocorrI.push_back(1.0);
  autocorrP.push_back(1.0);
  autocorrB.push_back(1.0);

  /* This distribution is just used one time and deleted after that.
     Next time a new distribution is used.*/
  logNormDisI = new wns::distribution::Norm(meanI, varianceI);
  logNormValueI = (*logNormDisI)();
  xI.push_back(logNormValueI);

  logNormDisP = new wns::distribution::Norm(meanP, varianceP);
  logNormValueP = (*logNormDisP)();
  xP.push_back(logNormValueP);

  logNormDisB = new wns::distribution::Norm(meanB, varianceB);
  logNormValueB = (*logNormDisB)();
  xB.push_back(logNormValueB);

  delete logNormDisI;
  logNormDisI = NULL;

  delete logNormDisP;
  logNormDisP = NULL;

  delete logNormDisB;
  logNormDisB = NULL;

  for(i = 1; i < limit; i++)
    {
      autocorrI.push_back(autocorrI[i - 1] * ((static_cast<float>(i - 1) + fracI)/ (static_cast<float>(i) - fracI)));
      autocorrP.push_back(autocorrP[i - 1] * ((static_cast<float>(i - 1) + fracP)/ (static_cast<float>(i) - fracP)));
      autocorrB.push_back(autocorrB[i - 1] * ((static_cast<float>(i - 1) + fracB)/ (static_cast<float>(i) - fracB)));

      pI = 0;
      pP = 0;
      pB = 0;

      for(j = 1; j <= (i-1); j++)
	{
	  pI = pI + phiIold[j - 1] * autocorrI[i - j];
	  pP = pP + phiPold[j - 1] * autocorrP[i - j];
	  pB = pB + phiBold[j - 1] * autocorrB[i - j];
	}

      dI = dI - ((nI * nI) / dI);
      nI = autocorrI[i] - pI;
      dP = dP - ((nP * nP) / dP);
      nP = autocorrP[i] - pP;
      dB = dB - ((nB * nB) / dB);
      nB = autocorrB[i] - pB;

      phiInew.push_back(nI / dI);
      phiPnew.push_back(nP / dP);
      phiBnew.push_back(nB / dB);

      idx = i;

      meanI = phiInew[0] * xI[0];
      meanP = phiPnew[0] * xP[0];
      meanB = phiBnew[0] * xB[0];

      for(idx = i; idx > 1; idx--)
	{
	  phiInew.push_front(phiIold[idx - 2] - (phiInew[i - idx] * phiIold[i - idx]));
	  phiPnew.push_front(phiPold[idx - 2] - (phiPnew[i - idx] * phiPold[i - idx]));
	  phiBnew.push_front(phiBold[idx - 2] - (phiBnew[i - idx] * phiBold[i - idx]));

	  meanI = meanI + (phiInew[0] * xI[i - idx + 1]);
	  meanP = meanP + (phiPnew[0] * xP[i - idx + 1]);
	  meanB = meanB + (phiBnew[0] * xB[i - idx + 1]);

	}
      varianceI = (1 - pow(phiInew[i - 1], 2)) * varianceI;
      varianceP = (1 - pow(phiPnew[i - 1], 2)) * varianceP;
      varianceB = (1 - pow(phiBnew[i - 1], 2)) * varianceB;

      /* This distribution is just used one time and deleted after that.
	 Next time a new distribution is used.*/
      logNormDisI = new wns::distribution::Norm(meanI, varianceI);
      logNormValueI = (*logNormDisI)();
      xI.push_back(logNormValueI);

      logNormDisP = new wns::distribution::Norm(meanP, varianceP);
      logNormValueP = (*logNormDisP)();
      xP.push_back(logNormValueP);

      logNormDisB = new wns::distribution::Norm(meanB, varianceB);
      logNormValueB = (*logNormDisB)();
      xB.push_back(logNormValueB);

      phiIold.clear();
      phiPold.clear();
      phiBold.clear();

      phiIold = phiInew;
      phiPold = phiPnew;
      phiBold = phiBnew;

      phiInew.clear();
      phiPnew.clear();
      phiBnew.clear();

      delete logNormDisI;
      logNormDisI = NULL;

      delete logNormDisP;
      logNormDisP = NULL;

      delete logNormDisB;
      logNormDisB = NULL;
    }

  xI.pop_front();
  xP.pop_front();
  xB.pop_front();

  autocorrI.clear();
  autocorrP.clear();
  autocorrB.clear();
}

void
Video::logNormalProjectedFarima(std::deque<double> _x, std::deque<double> _phiold, double _y,
				double _phi, double _theta, double _m, double _sqrValue,
				double _variance, int _k, double _logMean, double _logStd)
{
  mean = 0;
  n = 0;

  while(n < limit - 1)
    {
      mean = mean + _x[n] * _phiold[limit - n - 2];

      n = n + 1;
    }

  _variance = (1 - pow(_phiold[limit - 2], 2)) * _variance;

  normDis = new wns::distribution::Norm(mean, _variance);
  normValue = (*normDis)();
  _x.push_back(normValue);

  _y = _phi * _y + _x[limit - 1] + _theta * _x[limit - 2];

  _m = _m + _y;

  _sqrValue = _sqrValue + pow(_y, 2);

  normMean = _m / static_cast<int>((float)(_k + 1));

  normStd = ( _sqrValue - (pow(_m,2) / static_cast<int>((float)(_k + 1)))) / static_cast<int>((float)(_k));

  projection = exp(_logMean + (_logStd * ((_y - normMean) / sqrt(normStd))));

  if(byteFlag == true)
    {
      packetSize = (projection * 8.0) + 4.0;
    }
  else
    {
      packetSize = projection + 4.0;
    }

  _x.pop_front();
  _k = _k + 1;

  delete normDis;
  normDis = NULL;

  applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
  applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());

  if(firstPacketNumber == true)
    {
      packetNumber = 1;
      applicationPDU->setPacketNumber(packetNumber, packetFrom);
      MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

      firstPacketNumber = false;
    }
  else
    {
      ++packetNumber;
      applicationPDU->setPacketNumber(packetNumber, packetFrom);
      MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");
    }

  wns::osi::PDUPtr pdu(applicationPDU);
  applications::session::Session::outgoingProbesCalculation(pdu);

  connection->sendData(pdu);
}


void
Video::logNormalPDF(double _logMean, double _logStd)
{
  logNormVariance = pow(_logStd, 2);

  logNormDis = new wns::distribution::LogNorm(_logMean, logNormVariance);
  logNormValue = (*logNormDis)();

  value = exp(logNormValue);

  if(byteFlag == true)
    {
      packetSize = (value * 8.0) + 4.0;
    }
  else
    {
      packetSize = value + 4.0;
    }

  applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
  applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());

  if(firstPacketNumber == true)
    {
      packetNumber = 1;
      applicationPDU->setPacketNumber(packetNumber, packetFrom);
      MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

      firstPacketNumber = false;
    }
  else
    {
      ++packetNumber;
      applicationPDU->setPacketNumber(packetNumber, packetFrom);
      MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");
    }

  wns::osi::PDUPtr pdu(applicationPDU);
  applications::session::Session::outgoingProbesCalculation(pdu);

  connection->sendData(pdu);

  delete logNormDis;
  logNormDis = NULL;
}


void
Video::logNormalProjectedAR(double _y, double _phi, double _m, double _sqrValue,
			    int _k, double _logMean, double _logStd)
{
  normDis = new wns::distribution::Norm(0.0, 1.0);
  normValue = (*normDis)();

  _y = _phi * _y + normValue;

  _m = _m + _y;

  _sqrValue = _sqrValue + pow(_y, 2);

  normMean = _m / static_cast<int>((float)(_k + 1));

  normStd = ( _sqrValue - (pow(_m,2) / static_cast<int>((float)(_k + 1)))) / static_cast<int>((float)(_k));

  projection = exp(_logMean + (_logStd * ((_y - normMean) / sqrt(normStd))));

  if(byteFlag == true)
    {
      packetSize = (projection * 8.0) + 4.0;
    }
  else
    {
      packetSize = projection + 4.0;
    }

  applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
  applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());

  if(firstPacketNumber == true)
    {
      packetNumber = 1;
      applicationPDU->setPacketNumber(packetNumber, packetFrom);
      MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

      firstPacketNumber = false;
    }
  else
    {
      ++packetNumber;
      applicationPDU->setPacketNumber(packetNumber, packetFrom);
      MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");
    }

  wns::osi::PDUPtr pdu(applicationPDU);
  applications::session::Session::outgoingProbesCalculation(pdu);

  connection->sendData(pdu);

  delete normDis;
  normDis = NULL;

  _k = _k + 1;
}


void
Video::logNormalProjectedARMA(double _y, double _phi, double _theta, double _m,
			      double _sqrValue, int _k, double _logMean, double _logStd)
{
  normDis = new wns::distribution::Norm(0.0, 1.0);
  normValue = (*normDis)();

  _y = _phi * _y + _theta * normValue;

  _m = _m + _y;

  _sqrValue = _sqrValue + pow(_y, 2);

  normMean = _m / static_cast<int>((float)(_k + 1));

  normStd = (_sqrValue - (pow(_m, 2) / static_cast<int>((float)(_k + 1)))) / static_cast<int>((float)(_k));

  projection = exp(_logMean + (_logStd * ((_y - normMean) / sqrt(normStd))));

  if(byteFlag == true)
    {
      packetSize = (projection * 8.0) + 4.0;
    }
  else
    {
      packetSize = projection + 4.0;
    }

  applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
  applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());

  if(firstPacketNumber == true)
    {
      packetNumber = 1;
      applicationPDU->setPacketNumber(packetNumber, packetFrom);
      MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

      firstPacketNumber = false;
    }
  else
    {
      ++packetNumber;
      applicationPDU->setPacketNumber(packetNumber, packetFrom);
      MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");
    }

  wns::osi::PDUPtr pdu(applicationPDU);
  applications::session::Session::outgoingProbesCalculation(pdu);

  connection->sendData(pdu);

  delete normDis;
  normDis = NULL;
  _k = _k + 1;
}


void
Video::logNormalProjectedFDN(std::deque<double> _x, std::deque<double> _phiold, double _y,
			     double _phi, double _theta, double _m, double _sqrValue,
			     double _variance, int _k, double _logMean, double _logStd)
{
  mean = 0;
  n = 0;

  while(n < limit - 1)
    {
      mean = mean + _x[n] * _phiold[limit - n - 2];

      n = n + 1;
    }

  _variance = (1 - pow(_phiold[limit - 2], 2)) * _variance;

  normDis = new wns::distribution::Norm(mean, _variance);
  normValue = (*normDis)();

  _x.push_back(normValue);

  _y = _x[limit - 1];

  _m = _m + _y;

  _sqrValue = _sqrValue + pow(_y, 2);

  normMean = _m / static_cast<int>((float)(_k + 1));

  normStd = ( _sqrValue - (pow(_m, 2) / static_cast<int>((float)(_k + 1)))) / static_cast<int>((float)(_k));

  projection = exp(_logMean + (_logStd * ((_y - normMean) / sqrt(normStd))));

  if(byteFlag == true)
    {
      packetSize = (projection * 8.0) + 4.0;
    }
  else
    {
      packetSize = projection + 4.0;
    }

  _x.pop_front();
  _k = _k + 1;
  delete normDis;
  normDis = NULL;

  applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
  applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());

  if(firstPacketNumber == true)
    {
      packetNumber = 1;
      applicationPDU->setPacketNumber(packetNumber, packetFrom);
      MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

      firstPacketNumber = false;
    }
  else
    {
      ++packetNumber;
      applicationPDU->setPacketNumber(packetNumber, packetFrom);
      MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");
    }

  wns::osi::PDUPtr pdu(applicationPDU);
  applications::session::Session::outgoingProbesCalculation(pdu);

  connection->sendData(pdu);
}


void
Video::videoAR(double _y, double _phi, double _logMean, double _logStd)
{
  normDis = new wns::distribution::Norm(0.0, 1.0);
  normValue = (*normDis)();

  _y = _phi * _y + normValue;

  // Mean of lognormal distribution is exp(mu+sigma^2/2) and variance is (exp(sigma^2)-1)*exp(2*mu+sigma^2)
  projection = exp(_logMean + _logStd * _logStd / 2) +
    sqrt((exp(_logStd * _logStd) - 1) *
	 exp(2 * _logMean + _logStd * _logStd)) * _y;

  if(byteFlag == true)
    {
      packetSize = (projection * 8.0) + 4.0;
    }
  else
    {
      packetSize = projection + 4.0;
    }

  applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
  applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());

  if(firstPacketNumber == true)
    {
      packetNumber = 1;
      applicationPDU->setPacketNumber(packetNumber, packetFrom);
      MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

      firstPacketNumber = false;
    }
  else
    {
      ++packetNumber;
      applicationPDU->setPacketNumber(packetNumber, packetFrom);
      MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");
    }

  wns::osi::PDUPtr pdu(applicationPDU);
  applications::session::Session::outgoingProbesCalculation(pdu);

  connection->sendData(pdu);

  delete normDis;
  normDis = NULL;
}


void
Video::videoFARIMA(std::deque<double> _x, std::deque<double> _phiold, double _y, double _phi,
		   double _theta, double _variance, int _k, double _logMean, double _logStd)
{
  mean = 0;
  n = 0;

  while(n < limit - 1)
    {
      mean = mean + _x[n] * _phiold[limit - n - 2];

      n = n + 1;
    }
  _variance = (1 - pow(_phiold[limit - 2], 2)) * _variance;

  normDis = new wns::distribution::Norm(mean, _variance);
  normValue = (*normDis)();
  _x.push_back(normValue);

  _y = _phi * _y + _x[limit - 1] + _theta * _x[limit - 2];

  projection = exp(_logMean + _logStd * _logStd / 2) +
    sqrt((exp(_logStd * _logStd) - 1) *
	 exp(2 * _logMean + _logStd * _logStd)) * _y;

  if(byteFlag == true)
    {
      packetSize = (projection * 8.0) + 4.0;
    }
  else
    {
      packetSize = projection + 4.0;
    }

  applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
  applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());

  if(firstPacketNumber == true)
    {
      packetNumber = 1;
      applicationPDU->setPacketNumber(packetNumber, packetFrom);
      MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

      firstPacketNumber = false;
    }
  else
    {
      ++packetNumber;
      applicationPDU->setPacketNumber(packetNumber, packetFrom);
      MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");
    }

  wns::osi::PDUPtr pdu(applicationPDU);
  applications::session::Session::outgoingProbesCalculation(pdu);

  connection->sendData(pdu);

  _x.pop_front();
  _k = _k + 1;
  delete normDis;
  normDis = NULL;
}


void
Video::videoFDN(std::deque<double> _x, std::deque<double> _phiold, double _y,
		double _variance, int _k, double _logMean, double _logStd)
{
  mean = 0;
  n = 0;

  while( n < limit - 1)
    {
      mean = mean + _x[n] * _phiold[limit - n - 2];

      n = n + 1;
    }
  _variance = (1 - pow(_phiold[limit - 2], 2)) * _variance;

  normDis = new wns::distribution::Norm(mean, _variance);
  normValue = (*normDis)();
  _x.push_back(normValue);

  _y = _x[limit - 1];

  projection = exp(_logMean + _logStd * _logStd / 2) +
    sqrt((exp(_logStd * _logStd) - 1) *
	 exp(2 * _logMean + _logStd * _logStd)) * _y;

  if(byteFlag == true)
    {
      packetSize = (projection * 8.0) + 4.0;
    }
  else
    {
      packetSize = projection + 4.0;
    }

  applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
  applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());

  if(firstPacketNumber == true)
    {
      packetNumber = 1;
      applicationPDU->setPacketNumber(packetNumber, packetFrom);
      MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

      firstPacketNumber = false;
    }
  else
    {
      ++packetNumber;
      applicationPDU->setPacketNumber(packetNumber, packetFrom);
      MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");
    }

  wns::osi::PDUPtr pdu(applicationPDU);
  applications::session::Session::outgoingProbesCalculation(pdu);

  connection->sendData(pdu);

  _x.pop_front();
  _k = _k + 1;
  delete normDis;
  normDis = NULL;
}

