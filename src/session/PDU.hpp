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

#ifndef APPLICATIONS_SESSION_PDU_HPP
#define APPLICATIONS_SESSION_PDU_HPP

#include <APPLICATIONS/session/Session.hpp>
#include <WNS/SmartPtr.hpp>
#include <WNS/osi/PDU.hpp>
#include <WNS/osi/PCI.hpp>

namespace applications { namespace session {

    class PDU :
    public wns::osi::PDU
    {
    public:
      PDU(Bit packetSize, const wns::pyconfig::View& _pyco);
      virtual ~PDU();

      /* used to chek if this is the last packet */
      virtual void
      setLastPacket(bool _lastPacket);

      virtual bool
      getLastPacket();

      /* used to calculate the packet delay */
      virtual void
      setCreationTime(wns::simulator::Time _creationTime);

      virtual wns::simulator::Time
      getCreationTime();

      /* used to calculate the packet size */
      virtual void
      setFileLength(Bit _length);

      virtual Bit
      getFileLength();

      /* used to chek if this packet is a request or data packet */
      virtual void
      setRequest(bool _request);

      virtual bool
      getRequest();

      /* used to tell the server which movie the client wants */
      virtual void
      setMovieChoice(const char* _movieChoice);

      virtual const char*
      getMovieChoice();

      virtual void
      setPacketNumber(int _packetNumber, std::string _packetFrom);

      virtual int
      getPacketNumber();

      virtual std::string
      getPacketFrom();

    private:
      /**
       * @return The size of the %PDU in bit which is the sum of
       * the %PCI returned by PCI::getSize() and the size of the user
       * data (SDU) from the layer above.
       */
      virtual Bit
      doGetLengthInBits() const;

      bool lastPacket;
      bool request;
      Bit fileLength;
      const char* movieChoice;
      wns::simulator::Time creationTime;
      //     wns::logger::Logger logger;
      //      wns::pyconfig::View pyco;

      // TEST
      int packetNumber;
      std::string packetFrom;
    };

    /**
     * @brief Type definition for smart pointers to Application PDUs. A PDU this
     * smart pointer points to is automatically deleted when nobody references
     * it any more.
     */
    typedef wns::SmartPtr<PDU> PDUPtr;

  } //namespace session
} //namespace applications
#endif // APPLICATIONS_SESSION_PDU_HPP
