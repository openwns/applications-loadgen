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

#ifndef APPLICATIONS_SESSION_SESSION_HPP
#define APPLICATIONS_SESSION_SESSION_HPP

#include <APPLICATIONS/session/Binding.hpp>
#include <deque>
#include <map>
#include <APPLICATIONS/session/PDU.hpp>
#include <WNS/simulator/Time.hpp>
#include <WNS/distribution/Distribution.hpp>
#include <WNS/logger/Logger.hpp>
#include <WNS/events/MultipleTimeout.hpp>
#include <WNS/Enum.hpp>
#include <WNS/simulator/Bit.hpp>
#include <WNS/probe/bus/ContextCollector.hpp>
#include <WNS/probe/bus/ContextProvider.hpp>
#include <WNS/probe/bus/ContextProviderCollection.hpp>
#include <WNS/service/tl/Connection.hpp>
#include <WNS/service/tl/DataHandler.hpp>
#include <WNS/IOutputStreamable.hpp>


namespace applications {
  namespace node { namespace component {
      class Component;
    } //namespace component
  } //namespace node
  namespace session {
    class Binding;

    /* StationTypes are used to sort the Probes separated by clients and servers */
    enum StationType
      {
	SERVER = 1,
	CLIENT = 2
      };

    /* SessionTypes are used to sort the Probes separated by SessionType */
    enum SessionType
      {
	cbr = 1,
	email = 2,
	ftp = 3,
	voip = 4,
	video = 5,
	videotelephony = 6,
	videotrace = 7,
	www = 8,
	wimaxvideo = 9,
	wimaxvideotelephony = 10
      };

    /* States are used to see Sessionphase */
    enum State
      {
	idle = 1,
	running = 2,
	sessionended = 3
      };

    /* Timeouts are used to set session and state timer and for windwed probes duration */
    enum Timeout
      {
	statetimeout = 1,
	probetimeout = 2,
	sendtimeout = 3,
	receivetimeout = 4,
	frametimeout = 5,
	calltimeout = 6,
	connectiontimeout = 7,
	statetransitiontimeout = 8
      };

    class Session :
      public wns::service::tl::DataHandler,
      public wns::events::MultipleTimeout<Timeout>,
      public wns::IOutputStreamable
    {
    public:
      Session(const wns::pyconfig::View& _pyco);
      virtual ~Session();

      typedef wns::PyConfigViewCreator<Session, Session> Creator;
      typedef wns::StaticFactory<Creator> Factory;

      virtual void
      onTimeout(const Timeout& _t);

      virtual void
      onData(const wns::osi::PDUPtr& _pdu) = 0;

      virtual void
      registerBinding(const applications::session::BindingPtr& _binding);

      virtual void
      registerListener(applications::session::Binding* _listener);

      virtual void
      registerComponent(applications::node::component::Component* _component,
			int _sessionIndex);

      virtual void
      onConnectionEstablished(wns::service::tl::Connection* _connection);

      virtual void
      incomingProbesCalculation(const wns::osi::PDUPtr& _pdu);

      virtual void
      outgoingProbesCalculation(const wns::osi::PDUPtr& _pdu);

      virtual void
      sessionProbesCalculation();

      virtual void
      iatProbesCalculation();

      virtual void
      onShutdown();

      virtual std::string
      doToString() const;

      virtual void
      onPDUReceivedByPeer(const wns::osi::PDUPtr& _pdu);
    
    protected:

      /* WARNING: sessionStartTime should not be greater than settlingTime */
      wns::simulator::Time sessionStartTime;
      wns::simulator::Time now;

      /* Inter arrival Time distribution */
      wns::simulator::Time iat;// [sek]

      /* Packet Size distribution */
      Bit packetSize;// [bit]
      int bitRate;// [bit/sek]

      wns::logger::Logger logger;
      const wns::pyconfig::View pyco;
      applications::session::BindingPtr binding;
      applications::session::Binding* listener;
      applications::session::State state;

      /* used to restart a session if simulationtime left */
      applications::node::component::Component* component;
      int sessionIndex;

      /* used to tell the server which connection has to be used,
	 if there are more clients then servers, e.g. there are less
	 listenerBindings and the connection will be overwriten in
	 the listenerBinding from each client. */
      wns::service::tl::Connection* connection;

      /* used to tell the client when to close the connection */
      bool lastPacket;

      /* Variables needed for Probes */
      wns::probe::bus::ContextCollectorPtr incomingPacketSizeProbe;
      wns::probe::bus::ContextCollectorPtr outgoingPacketSizeProbe;
      wns::probe::bus::ContextCollectorPtr packetDelayProbe;
      wns::probe::bus::ContextCollectorPtr windowIncomingBitThroughputProbe;
      wns::probe::bus::ContextCollectorPtr windowOutgoingBitThroughputProbe;
      wns::probe::bus::ContextCollectorPtr windowIncomingPacketThroughputProbe;
      wns::probe::bus::ContextCollectorPtr windowOutgoingPacketThroughputProbe;
      wns::probe::bus::ContextCollectorPtr sessionIncomingBitThroughputProbe;
      wns::probe::bus::ContextCollectorPtr sessionOutgoingBitThroughputProbe;
      wns::probe::bus::ContextCollectorPtr sessionIncomingPacketThroughputProbe;
      wns::probe::bus::ContextCollectorPtr sessionOutgoingPacketThroughputProbe;
      wns::probe::bus::ContextCollectorPtr measuringDurationProbe;
      wns::probe::bus::ContextCollectorPtr iatProbe;
      wns::probe::bus::ContextCollectorPtr packetLossProbe;
      wns::probe::bus::ContextCollectorPtr delayLossProbe;
      wns::probe::bus::ContextCollectorPtr userSatisfactionProbe;
      wns::probe::bus::ContextCollectorPtr connectionProbe;
      SessionType sessionType;
      StationType stationType;

      /* counter variables for each packet */
      Bit incomingPacketSize;
      Bit outgoingPacketSize;
      wns::simulator::Time packetDelay;
      wns::simulator::Time maxDelay;
      wns::simulator::Time iatStart;
      wns::simulator::Time iatEnd;
      wns::simulator::Time settlingTime;
      wns::simulator::Time probeEndTime;
      bool firstIatProbe;

      /* counter variables for windowed probes*/
      wns::simulator::Time windowSize;
      Bit windowedIncomingBitThroughput;
      Bit windowedOutgoingBitThroughput;
      int windowedIncomingPacketThroughput;
      int windowedOutgoingPacketThroughput;

      /* counter variables for whole session */
      int incomingPacketCounter;
      int outgoingPacketCounter;
      long int receivedPackets;
      long int receivedOnTimePackets;
      wns::simulator::Time measuringDuration;
      wns::simulator::Time sessionRunTime;
      Bit incomingPacketSizeCounter;
      Bit outgoingPacketSizeCounter;
      double packetLossRatio;
      double delayLossRatio;
      double maxLossRatio;

      long int packetNumber;
      long int lastPacketNumber;
      long int packetLossCounter;
      long int delayLossCounter;
      long int receivedPacketNumber;
      long int packetsDuringSettlingTime;

      //TEST
      std::string packetFrom;
      int intWert;
      std::stringstream strings;
      std::string packetFromID;

    };
  } // namespace session
} // namespace applications

#endif //APPLICATIONS_SESSION_SESSION_HPP
