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

#include <APPLICATIONS/session/Session.hpp>
#include <boost/tuple/tuple.hpp>

using namespace applications::session;

Session::Session(const wns::pyconfig::View& _pyco) :
  wns::events::MultipleTimeout<Timeout>(),
  wns::IOutputStreamable(),
  lastPacket(false),
  pyco(_pyco),
  logger(_pyco.get("logger")),
  state(idle),
  component(NULL),
  connection(NULL),
  listener(NULL),
  sessionIndex(0),
  incomingPacketSizeCounter(0),
  incomingPacketCounter(0),
  incomingPacketSize(0),
  outgoingPacketSizeCounter(0),
  outgoingPacketCounter(0),
  outgoingPacketSize(0),
  packetDelay(0.0),
  maxDelay(10.0),
  iatStart(0.0),
  iatEnd(0.0),
  settlingTime(0.0),
  probeEndTime(_pyco.get<wns::simulator::Time>("probeEndTime")),
  firstIatProbe(true),
  measuringDuration(0.0),
  windowedIncomingBitThroughput(0.0),
  windowedIncomingPacketThroughput(0),
  windowedOutgoingBitThroughput(0.0),
  windowedOutgoingPacketThroughput(0),
  windowSize(1.0),
  /* WARNING: sessionStartTime should not be greater than settlingTime */
  sessionStartTime(wns::simulator::getEventScheduler()->getTime()),
  bitRate(0),
  packetSize(0),
  iat(0.0),
  now(0.0),
  packetNumber(0),
  lastPacketNumber(0),
  packetLossCounter(0),
  packetsDuringSettlingTime(0),
  receivedPackets(0),
  receivedOnTimePackets(0),
  packetLossRatio(0.0),
  maxLossRatio(1.0),
  receivedPacketNumber(0),
  packetFrom("Session"),
  senderId(-1),
  applId(-1)
{
}

Session::~Session()
{
  listener = NULL;
  component = NULL;
  connection = NULL;
}

void
Session::onTimeout(const Timeout& _t)
{
  now = wns::simulator::getEventScheduler()->getTime();

  if(now > settlingTime)
    {
      windowIncomingBitThroughputProbe->put(windowedIncomingBitThroughput, 
            boost::make_tuple("Appl.SenderId", senderId,
                "Appl.CellId", getCellId(senderId)));
      windowIncomingPacketThroughputProbe->put(windowedIncomingPacketThroughput, 
            boost::make_tuple("Appl.SenderId", senderId,
                "Appl.CellId", getCellId(senderId)));

      windowOutgoingBitThroughputProbe->put(windowedOutgoingBitThroughput, 
            boost::make_tuple("Appl.SenderId", senderId,
                "Appl.CellId", getCellId(senderId)));
      windowOutgoingPacketThroughputProbe->put(windowedOutgoingPacketThroughput, 
            boost::make_tuple("Appl.SenderId", senderId,
                "Appl.CellId", getCellId(senderId)));

      windowedIncomingBitThroughput = 0.0;
      windowedIncomingPacketThroughput = 0;

      windowedOutgoingBitThroughput = 0.0;
      windowedOutgoingPacketThroughput = 0;

      setTimeout(probetimeout, windowSize);
    }
  else
    {
      MESSAGE_SINGLE(NORMAL, logger, "APPL: Not probing during settling time!");
    }
}


void
Session::registerBinding(const applications::session::BindingPtr& _binding)
{
  binding = _binding;
}

void
Session::registerListener(applications::session::Binding* _listener)
{
  listener = _listener;
}

void
Session::registerComponent(applications::node::component::Component* _component,
			   int _sessionIndex)
{
  component = _component;
  sessionIndex = _sessionIndex;

  //TEST
  intWert = component->getNode()->getNodeID();
  strings << intWert;
  packetFromID = strings.str();

  wns::probe::bus::ContextProviderCollection localContext(&component->getContextProviderCollection());

  MESSAGE_BEGIN(NORMAL, logger, m, "APPL: Setting probe context, ");
  m << "Appl.Id=" << component->getNode()->getNodeID();
  m << ", Appl.StationType=" << this->stationType;
  m << ", Appl.SessionType=" << this->sessionType;
  MESSAGE_END();

  applId = component->getNode()->getNodeID();
  localContext.addProvider(wns::probe::bus::contextprovider::Constant("Appl.Id", applId));
  localContext.addProvider(wns::probe::bus::contextprovider::Constant("Appl.StationType", this->stationType));
  localContext.addProvider(wns::probe::bus::contextprovider::Constant("Appl.SessionType", this->sessionType));

  /* The names of the probes must match those in default.py (no automatism) */
  incomingPacketSizeProbe = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(localContext, "applications.packet.incoming.size"));

  outgoingPacketSizeProbe = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(localContext, "applications.packet.outgoing.size"));

  packetDelayProbe = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(localContext, "applications.packet.incoming.delay"));

  windowIncomingBitThroughputProbe = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(localContext, "applications.window.incoming.bitThroughput"));

  windowIncomingPacketThroughputProbe = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(localContext, "applications.window.incoming.packetThroughput"));

  windowOutgoingBitThroughputProbe = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(localContext, "applications.window.outgoing.bitThroughput"));

  windowOutgoingPacketThroughputProbe = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(localContext, "applications.window.outgoing.packetThroughput"));

  sessionIncomingBitThroughputProbe = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(localContext, "applications.session.incoming.bitThroughput"));

  sessionIncomingPacketThroughputProbe = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(localContext, "applications.session.incoming.packetThroughput"));

  sessionOutgoingBitThroughputProbe = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(localContext, "applications.session.outgoing.bitThroughput"));

  sessionOutgoingPacketThroughputProbe = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(localContext, "applications.session.outgoing.packetThroughput"));

  measuringDurationProbe = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(localContext, "applications.session.duration"));

  iatProbe = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(localContext, "applications.packet.outgoing.iat"));

  packetLossProbe = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(localContext, "applications.session.incoming.packetLoss"));

  userSatisfactionProbe = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(localContext, "applications.session.userSatisfaction"));

  connectionProbe = wns::probe::bus::ContextCollectorPtr(new wns::probe::bus::ContextCollector(localContext, "applications.connectionEstablished"));
}


void
Session::onConnectionEstablished(wns::service::tl::Connection* _connection)
{
  connection = _connection;
}

void
Session::incomingProbesCalculation(const wns::osi::PDUPtr& _pdu)
{

  now = wns::simulator::getEventScheduler()->getTime();

  if(now > settlingTime)
    {

      assureType(_pdu.getPtr(), applications::session::PDU*);

      incomingPacketCounter++;

      incomingPacketSize = _pdu.getPtr()->getLengthInBits();
      incomingPacketSizeCounter += incomingPacketSize;

      windowedIncomingPacketThroughput++;
      windowedIncomingBitThroughput += incomingPacketSize;

      assure(now >= static_cast<applications::session::PDU*>(_pdu.getPtr())->getCreationTime(), "Living in the past!");

      packetDelay = now - static_cast<applications::session::PDU*>(_pdu.getPtr())->getCreationTime();

      applications::session::Session* sender = static_cast<applications::session::PDU*>(_pdu.getPtr())->getSender();
      if(sender != NULL)
      {
        sender->onPDUReceivedByPeer(_pdu);
        assure(senderId == -1 || senderId == sender->getId(), "Receiving from different peers.");
        senderId = sender->getId();
      }

      packetDelayProbe->put(packetDelay, boost::make_tuple("Appl.SenderId", senderId,
                "Appl.CellId", getCellId(senderId)));
      incomingPacketSizeProbe->put(incomingPacketSize, boost::make_tuple("Appl.SenderId", senderId,
                "Appl.CellId", getCellId(senderId)));

      MESSAGE_SINGLE(NORMAL, logger, "APPL: PDU received with a size of " << _pdu.getPtr()->getLengthInBits()
		     << " bits and a delay of " << packetDelay << ". Now recieved a total of "
		     << incomingPacketCounter << " packets and " << incomingPacketSizeCounter << " bits.");

      /* packetLoss */
      if(receivedPacketNumber > (lastPacketNumber + 1))
	{
	  long int counter = receivedPacketNumber - (lastPacketNumber + 1);

	  packetLossCounter += counter;

	  MESSAGE_SINGLE(NORMAL, logger, "APPL: " << counter << " packets lost. Now lost a total of "
			 << packetLossCounter << " packets out of " << receivedPacketNumber << ".\n");

	  lastPacketNumber = receivedPacketNumber;
	}
      else
	{
	  lastPacketNumber = receivedPacketNumber;
	}

    }
  else
    {
      packetsDuringSettlingTime++;
      MESSAGE_SINGLE(NORMAL, logger, "APPL: Not probing during settling time!" << packetsDuringSettlingTime
		     << " packets received during settling time.");
      lastPacketNumber = receivedPacketNumber;
    }
}

void
Session::onPDUReceivedByPeer(const wns::osi::PDUPtr& _pdu)
{
  now = wns::simulator::getEventScheduler()->getTime();
  wns::simulator::Time txTime;
  txTime = static_cast<applications::session::PDU*>(_pdu.getPtr())->getCreationTime();

  if(txTime <= probeEndTime && txTime > settlingTime)
  {
    wns::simulator::Time delay = now - txTime;

    receivedPackets++;
    if(delay < maxDelay)
    {
       receivedOnTimePackets++;
    }
  }   
}

int
Session::getId()
{
  return applId;
}

void
Session::outgoingProbesCalculation(const wns::osi::PDUPtr& pdu)
{
  now = wns::simulator::getEventScheduler()->getTime();

  static_cast<applications::session::PDU*>(pdu.getPtr())->setSender(this);

  if(now > settlingTime && now < probeEndTime)
    {
      outgoingPacketCounter++;

      outgoingPacketSize = pdu->getLengthInBits();
      outgoingPacketSizeCounter += outgoingPacketSize;

      windowedOutgoingPacketThroughput++;
      windowedOutgoingBitThroughput += outgoingPacketSize;

      outgoingPacketSizeProbe->put(outgoingPacketSize, 
        boost::make_tuple("Appl.SenderId", senderId,
                "Appl.CellId", getCellId(senderId)));
    }
  else
    {
      MESSAGE_SINGLE(NORMAL, logger, "APPL: Not probing during settling time!");
    }
}

void
Session::sessionProbesCalculation()
{
  measuringDuration = wns::simulator::getEventScheduler()->getTime() - settlingTime;
  MESSAGE_SINGLE(NORMAL, logger, "APPL: SessionDuration = " << measuringDuration << ".");
  measuringDurationProbe->put(measuringDuration, boost::make_tuple("Appl.SenderId", senderId,
    "Appl.CellId", getCellId(senderId)));

  sessionIncomingBitThroughputProbe->put(incomingPacketSizeCounter / measuringDuration, 
    boost::make_tuple("Appl.SenderId", senderId,
                "Appl.CellId", getCellId(senderId)));
  sessionIncomingPacketThroughputProbe->put(incomingPacketCounter / measuringDuration, 
    boost::make_tuple("Appl.SenderId", senderId,
                "Appl.CellId", getCellId(senderId)));

  sessionOutgoingBitThroughputProbe->put(outgoingPacketSizeCounter / measuringDuration, 
    boost::make_tuple("Appl.SenderId", senderId,
                "Appl.CellId", getCellId(senderId)));
  sessionOutgoingPacketThroughputProbe->put(outgoingPacketCounter / measuringDuration, 
    boost::make_tuple("Appl.SenderId", senderId,
                "Appl.CellId", getCellId(senderId)));

  long int lost = outgoingPacketCounter - receivedOnTimePackets;

  if(outgoingPacketCounter > 0 && double(lost) / double(outgoingPacketCounter) < maxLossRatio)
    userSatisfactionProbe->put(1, boost::make_tuple("Appl.SenderId", senderId, 
        "Appl.CellId", getCellId(senderId)));
  else
  {
    userSatisfactionProbe->put(0, boost::make_tuple("Appl.SenderId", senderId, 
        "Appl.CellId", getCellId(senderId)));
  }

  if(receivedPacketNumber > 0)
  {
    MESSAGE_SINGLE(NORMAL, logger, "APPL: At the end a total of " << packetLossCounter
        << " packets out of " << (receivedPacketNumber-packetsDuringSettlingTime) << " are lost.\n");

    if(outgoingPacketCounter > 0)
    {
        packetLossProbe->put(double(lost) / double(outgoingPacketCounter), 
            boost::make_tuple("Appl.SenderId", senderId,
                "Appl.CellId", getCellId(senderId)));
    }
    else
    {
        packetLossProbe->put(1.0, 
            boost::make_tuple("Appl.SenderId", senderId,
                "Appl.CellId", getCellId(senderId)));
    }
  }
}

void
Session::iatProbesCalculation()
{
  now = wns::simulator::getEventScheduler()->getTime();

  if(now > settlingTime)
    {
      if(firstIatProbe == false)
	{
	  iatEnd = wns::simulator::getEventScheduler()->getTime() - iatStart;

	  MESSAGE_SINGLE(NORMAL, logger, "APPL: iat = " << iatEnd << ".");

	  iatProbe->put(iatEnd, 
            boost::make_tuple("Appl.SenderId", senderId,
                "Appl.CellId", getCellId(senderId)));

	  iatStart = wns::simulator::getEventScheduler()->getTime();
	}
      else
	{
	  iatStart = wns::simulator::getEventScheduler()->getTime();

	  firstIatProbe = false;
	}
    }
  else
    {
      MESSAGE_SINGLE(NORMAL, logger, "APPL: Not probing during settling time!");
    }
}


void
Session::onShutdown()
{
}


std::string
Session::doToString() const
{
    std::stringstream ss;
    ss << "session = " << this->sessionType << std::endl;
    return ss.str();
}

int
Session::getCellId(int senderId)
{
    if(wns::simulator::getRegistry()->knows("DLLAssoc"))
    {
        std::map<int, int> assoc;
        assoc = wns::simulator::getRegistry()->find<std::map<int, int> >("DLLAssoc");
        if(assoc.find(senderId) != assoc.end())
        {
            return assoc[senderId];
        }
        else
        if(assoc.find(applId) != assoc.end())
        {
            return assoc[applId];
        }
    }
    return -1;
}

