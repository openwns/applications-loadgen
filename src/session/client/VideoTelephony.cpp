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

#include <APPLICATIONS/session/client/VideoTelephony.hpp>
#include <WNS/distribution/Norm.hpp>

using namespace applications::session::client;

STATIC_FACTORY_REGISTER_WITH_CREATOR(applications::session::client::VideoTelephony,
				     applications::session::Session,
				     "client.VideoTelephony", wns::PyConfigViewCreator);

VideoTelephony::VideoTelephony(const wns::pyconfig::View& _pyco) :
  Session(_pyco),
  i(0),
  j(0),
  limit(1000),
  idx (0),

  fracI(0.0),
  kI(1),
  pI(0.0),
  nI(0.0),
  dI(1.0),
  logNormValueI(0.0),
  meanI(0.0),
  varianceI(1.0),
  yI(0.0),
  mI(0.0),
  sqrValueI(0.0),

  fracB(0.0),
  kB(1),
  pB(0.0),
  nB(0.0),
  dB(1.0),
  logNormValueB(0.0),
  meanB(0.0),
  varianceB(1.0),
  yB(0.0),
  mB(0.0),
  sqrValueB(0.0),

  fracP(0.0),
  kP(1),
  pP(0.0),
  nP(0.0),
  dP(1.0),
  logNormValueP(0.0),
  meanP(0.0),
  varianceP(1.0),
  yP(0.0),
  mP(0.0),
  sqrValueP(0.0),

  mean(0.0),
  n(0),
  distribution(NULL),
  logNormDisI(NULL),
  logNormDisB(NULL),
  logNormDisP(NULL),
  distributionValue(0.0),
  normMean(0.0),
  variance(0.0),

  stateTransitionDistribution(NULL),
  cnCounter(0),
  bFrameCounter(1),
  gopCounter(1),
  newGOP(true)
{
  wns::pyconfig::View sTVConfig(_pyco, "stateTransition");
  std::string sTVName = sTVConfig.get<std::string>("__plugin__");
  stateTransitionDistribution = wns::distribution::DistributionFactory::creator(sTVName)->create(sTVConfig);

  settlingTime = _pyco.get<wns::simulator::Time>("settlingTime");

  byteFlag = _pyco.get<bool>("params.byteFlag");

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

  voicePacketIat = _pyco.get<wns::simulator::Time>("voicePacketIat");
  voicePacketSize = _pyco.get<Bit>("voicePacketSize");
  /* Packet size includes 12 bytes RTP header. */
  voicePacketSize = voicePacketSize + 96;

  comfortNoisePacketSize = _pyco.get<Bit>("comfortNoisePacketSize");
  /* Packet size includes 12 bytes RTP header. */
  comfortNoisePacketSize = comfortNoisePacketSize + 96;

  comfortNoise = _pyco.get<bool>("comfortNoise");

  MESSAGE_BEGIN(NORMAL, logger, m, "APPL: Starting new session with voiceparameters: ");
  m << "voicePacketSize = " << voicePacketSize;
  m << ", voicePacketIat = " << voicePacketIat;
  m << ", comfortNoisePacketSize = " << comfortNoisePacketSize;
  MESSAGE_END();

  videoFrameRate = _pyco.get<int>("params.frameRate");
  videoPacketIat = 1 / double(videoFrameRate);

  MESSAGE_BEGIN(NORMAL, logger, m, "APPL: Starting new session with videoparameters: ");
  m << "videoFrameRate = " << videoFrameRate;
  m << ", videoPacketIat = " << videoPacketIat;
  MESSAGE_END();

  calculateSLRangeDependency();

  state = running;

  /* only for probing */
  sessionType = videotelephony;

  sessionDelay = (*sessionDelayDistribution)();
  MESSAGE_SINGLE(NORMAL, logger, "APPL: Delay before session starts: " << sessionDelay << ".\n");

  setTimeout(connectiontimeout, sessionDelay);
  setTimeout(probetimeout, windowSize);
}


VideoTelephony::~VideoTelephony()
{
  if (stateTransitionDistribution != NULL)
    delete stateTransitionDistribution;
  stateTransitionDistribution = NULL;

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
VideoTelephony::onData(const wns::osi::PDUPtr& _pdu)
{
  assureType(_pdu.getPtr(), applications::session::PDU*);

  receivedPacketNumber = static_cast<applications::session::PDU*>(_pdu.getPtr())->getPacketNumber();

  MESSAGE_SINGLE(NORMAL, logger, "APPL: receivedPacketNumber = " << receivedPacketNumber << ".");

  applications::session::Session::incomingProbesCalculation(_pdu);

  if((receivedPacketNumber == 1) && (state != sessionended))
    {
      /* Voice */
      onTimeout(sendtimeout);
      voiceState = active;
      setTimeout(statetransitiontimeout, 0.02);

      /* Video */
      onTimeout(frametimeout);

      state = running;
    }
}

void
VideoTelephony::onTimeout(const Timeout& _t)
{
  /* Voice */
  if(_t == sendtimeout)
    {
      MESSAGE_SINGLE(NORMAL, logger, "APPL: Sending voicepacket!");

      applications::session::Session::iatProbesCalculation();

      packetSize = voicePacketSize;
      applications::session::Session::outgoingProbesCalculation();

      applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(voicePacketSize), pyco);
      applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());

      ++packetNumber;
      applicationPDU->setPacketNumber(packetNumber, packetFrom);
      MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

      wns::osi::PDUPtr pdu(applicationPDU);

      connection->sendData(pdu);
    }
  else if(_t == receivetimeout)
    {
      /* Comfort noise packets will be send to fill the silence */
      MESSAGE_SINGLE(NORMAL, logger, "APPL: Sending comfort noise packet!");

      applications::session::Session::iatProbesCalculation();

      packetSize = comfortNoisePacketSize;
      applications::session::Session::outgoingProbesCalculation();

      applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(comfortNoisePacketSize), pyco);
      applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());

      ++packetNumber;
      applicationPDU->setPacketNumber(packetNumber, packetFrom);
      MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

      wns::osi::PDUPtr pdu(applicationPDU);

      connection->sendData(pdu);
    }
  else if(_t == statetransitiontimeout)
    {
      /* Transition between active (speech) state and inactive (silent/pause) state. */
      double stateTransitionValue = (*stateTransitionDistribution)();

      MESSAGE_SINGLE(NORMAL, logger, "APPL: State transition. TransitionValue = " << stateTransitionValue << ".");

      if(stateTransitionValue <= 0.01 && voiceState == inactive)
	{
	  cnCounter = 0;
	  onTimeout(sendtimeout);

	  voiceState = active;
	}
      else if(stateTransitionValue > 0.01 && voiceState == inactive)
	{
	  if(cnCounter == 8)
	    {
	      cnCounter = 1;
	      onTimeout(receivetimeout);
	    }
	  else
	    {
	      cnCounter += 1;
	    }
	}
      else if(stateTransitionValue <= 0.01 && voiceState == active)
	{
	  cnCounter = 1;
	  onTimeout(receivetimeout);

	  voiceState = inactive;
	}
      else if(stateTransitionValue > 0.01 && voiceState == active)
	{
	  onTimeout(sendtimeout);
	}

      setTimeout(statetransitiontimeout, 0.02);
    }
  /* Video */
  else if(_t == frametimeout)
    {
      if(newGOP == false)
	{
	  if(bFrameCounter <= 2)
	    {
	      /* B-Frame */
	      MESSAGE_SINGLE(NORMAL, logger, "APPL: B-FRAME!");

	      bFrameCounter += 1;

	      setTimeout(frametimeout, videoPacketIat);

	      logNormalProjectedFarima(xB, phiBold, yB, phiB, thetaB, mB, sqrValueB,
				       varianceB, kB, logMeanB, logStdB);
	    }
	  else
	    {
	      /* P-Frame */
	      bFrameCounter = 1;

	      if(gopCounter < 4)
		{
		  MESSAGE_SINGLE(NORMAL, logger, "APPL: P-FRAME!");

		  logNormalProjectedFarima(xP, phiPold, yP, phiP, thetaP, mP, sqrValueP,
					   varianceP, kP, logMeanP, logStdP);

		  gopCounter += 1;
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
	  /* I-Frame */
	  MESSAGE_SINGLE(NORMAL, logger, "APPL: I-FRAME!");

	  VideoTelephony::logNormalProjectedFarima(xI, phiIold, yI, phiI, thetaI, mI, sqrValueI,
						   varianceI, kI, logMeanI, logStdI);

	  newGOP = false;
	  setTimeout(frametimeout, videoPacketIat);
	}
    }
  else if(_t == connectiontimeout)
    {
      packetFrom = "client.VideoTelephony";

      /* Open connection */
      binding->initBinding();
    }
  else if(_t == probetimeout)
    {
      applications::session::Session::onTimeout(_t);
    }
  else if(_t == calltimeout)
    {
      /* Start with calling the server. */
      applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(comfortNoisePacketSize), pyco);
      applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());

      packetSize = comfortNoisePacketSize;
      applications::session::Session::outgoingProbesCalculation();

      packetNumber = 1;
      applicationPDU->setPacketNumber(packetNumber, packetFrom);
      MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

      wns::osi::PDUPtr pdu(applicationPDU);

      connection->sendData(pdu);

      state = idle;
    }
  else
    {
      assure(false, "APPL: Unknown timout type =" << _t);
    }
}


void
VideoTelephony::onConnectionEstablished(wns::service::tl::Connection* _connection)
{
  /* Connection is ready, so start sending after session start delay. */
  connection = _connection;

  MESSAGE_SINGLE(NORMAL, logger, "APPL: Connection established!");

  onTimeout(calltimeout);
}


void
VideoTelephony::calculateSLRangeDependency()
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
VideoTelephony::logNormalProjectedFarima(std::deque<double> _x, std::deque<double> _phiold, double _y,
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

  distribution = new wns::distribution::Norm(mean, _variance);
  distributionValue = (*distribution)();
  _x.push_back(distributionValue);

  _y = _x[limit - 1];

  _m = _m + _y;

  _sqrValue = _sqrValue + pow(_y, 2);

  normMean = _m / static_cast<int>((float)(_k + 1));

  variance = (_sqrValue - (pow(_m, 2) / static_cast<int>((float)(_k + 1)))) / static_cast<int>((float)(_k));

  packetSize = exp(_logMean + (_logStd * ((_y - normMean) / sqrt(variance)))) + 1;

  if(byteFlag == true)
    {
      packetSize *= 8.0;
    }

  _x.pop_front();
  _k = _k + 1;

  delete distribution;
  distribution = NULL;

  applications::session::PDU* applicationPDU = new applications::session::PDU(Bit(packetSize), pyco);
  applicationPDU->setCreationTime(wns::simulator::getEventScheduler()->getTime());

  applications::session::Session::outgoingProbesCalculation();

  ++packetNumber;
  applicationPDU->setPacketNumber(packetNumber, packetFrom);
  MESSAGE_SINGLE(NORMAL, logger, "APPL: PacketNumber = " << packetNumber << ".");

  wns::osi::PDUPtr pdu(applicationPDU);

  connection->sendData(pdu);
}
