##############################################################################
# This file is part of openWNS (open Wireless Network Simulator)
# _____________________________________________________________________________
#
# Copyright (C) 2004-2007
# Chair of Communication Networks (ComNets)
# Kopernikusstr. 5, D-52074 Aachen, Germany
# phone: ++49-241-80-27910,
# fax: ++49-241-80-22242
# email: info@openwns.org
# www: http://www.openwns.org
# _____________________________________________________________________________
#
# openWNS is free software; you can redistribute it and/or modify it under the
# terms of the GNU Lesser General Public License version 2 as published by the
# Free Software Foundation;
#
# openWNS is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
###############################################################################

# codec.py

from openwns.pyconfig import Sealed

class Codec(Sealed):
    packetIat = None # in sec
    packetSize = None # in Bit per packet
    cnPacketSize = None # size of comfort noise (cn) packet in Bit
    def __init__(self, _packetIat, _packetSize, _cnPacketSize):
        self.packetIat    = _packetIat
        self.packetSize  = _packetSize
        self.cnPacketSize = _cnPacketSize

# Frame-Based Codecs
class G723(Codec):
    # only high-rate speech (6.3 kbit/s) is defined
    def __init__(self):
        super(G723, self).__init__(0.03, 192, 32)

class G728(Codec):
    # bit-rate = 16 kbit/s
    def __init__(self):
        super(G728, self).__init__(0.02, 320, 8)

class G729(Codec):
    # bit-rate = 8 kbit/s
    def __init__(self):
        super(G729, self).__init__(0.02, 160, 16)

class GSM(Codec):
    # full-rate speech coding. bit-rate = 13.2 kbit/s
    def __init__(self):
        super(GSM, self).__init__(0.02, 264, 8)

class GSM_EFR(Codec):
    # enhanced full-rate speech coding. bit-rate = 12.4 kbit/s
    def __init__(self):
        super(GSM_EFR, self).__init__(0.02, 248, 8)

class LPC(Codec):
    # linear predictive coding. bit-rate = 5.6 kbit/s
    def __init__(self):
        super(LPC, self).__init__(0.02, 112, 8)


# Sample-Based Codecs
class G722(Codec):
    # bit-rate = 64 kbit/s
    def __init__(self):
        super(G722, self).__init__(0.02, 1280, 8)

class G726_40(Codec):
    # bit-rate = 40 kbit/s
    def __init__(self):
        super(G726_40, self).__init__(0.02, 800, 8)

class G726_32(Codec):
    # bit-rate = 32 kbit/s
    def __init__(self):
        super(G726_32, self).__init__(0.02, 640, 8)

class G726_24(Codec):
    # bit-rate = 24 kbit/s
    def __init__(self):
        super(G726_24, self).__init__(0.02, 480, 8)

class G726_16(Codec):
    # bit-rate = 16 kbit/s
    def __init__(self):
        super(G726_16, self).__init__(0.02, 320, 8)

class G711(Codec):
    # bit-rate = 64 kbit/s
    def __init__(self):
        super(G711, self).__init__(0.01, 1280 , 8)


# AMR Codecs (select the best codec mode to meet the local radio channel and capacity requirements)
# If the radio conditions are bad, source coding is reduced and channel coding is increased.
# This improves the quality and robustness of the network connection while sacrificing some voice clarity. 
class AMR_4_75(Codec):
    # bit-rate = 4.75 kbit/s
    def __init__(self):
        super(AMR_4_75, self).__init__(0.02, 95, 8)

class AMR_5_15(Codec):
    # bit-rate = 5.15 kbit/s
    def __init__(self):
        super(AMR_5_15, self).__init__(0.02, 103, 8)

class AMR_5_9(Codec):
    # bit-rate = 5.9 kbit/s
    def __init__(self):
        super(AMR_5_9, self).__init__(0.02, 118, 8)

class AMR_6_7(Codec):
    # bit-rate = 6.7 kbit/s
    def __init__(self):
        super(AMR_6_7, self).__init__(0.02, 134, 8)

class AMR_7_4(Codec):
    # bit-rate = 7.4 kbit/s
    def __init__(self):
        super(AMR_7_4, self).__init__(0.02, 148, 8)

class AMR_7_95(Codec):
    # bit-rate = 7.95 kbit/s
    def __init__(self):
        super(AMR_7_95, self).__init__(0.02, 159, 8)

class AMR_10_2(Codec):
    # bit-rate = 10.2 kbit/s
    def __init__(self):
        super(AMR_10_2, self).__init__(0.02, 204, 8)

class AMR_12_2(Codec):
    # bit-rate = 12.2 kbit/s
    def __init__(self):
        super(AMR_12_2, self).__init__(0.02, 244, 8)
