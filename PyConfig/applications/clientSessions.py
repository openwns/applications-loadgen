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

# clientSessions.py

import openwns.distribution
from cauchy import *
from codec import *
from videoParameters import *
from random import *
import sys
import os

class ClientSessions(object):
    logger = None
    sessionDelay = None
    settlingTime = None
    trafficDelay = None
    def __init__(self, settlingTime = 1.0, minStartDelay = 0.1, maxStartDelay = 1.0,
        minTrafficDelay = 0.0, maxTrafficDelay = 0.0):
        self.sessionDelay = openwns.distribution.Uniform(minStartDelay, maxStartDelay)
        self.trafficDelay = openwns.distribution.Uniform(minTrafficDelay, maxTrafficDelay)
        self.settlingTime = settlingTime

class CBR(ClientSessions):
    __plugin__ = "client.CBR"
    packetSize = None
    bitRate = None
    def __init__(self, packetSize = 512, bitRate = 64, settlingTime = 1.0, 
                 minStartDelay = 0.1, maxStartDelay = 1.0, parentLogger = None,
                 minTrafficDelay = 0.0, maxTrafficDelay = 0.0):
        super(CBR, self).__init__(settlingTime, minStartDelay, maxStartDelay, 
                minTrafficDelay, maxTrafficDelay)
        self.packetSize = openwns.distribution.Fixed(packetSize)
        self.bitRate = openwns.distribution.Fixed(bitRate)
        self.logger = openwns.logger.Logger("APPL", "CBR", True, parentLogger)


class Email(ClientSessions):
    __plugin__ = "client.Email"
    numberOfEmails = None
    emailSizeChoice = None
    largeEmailSize = None
    smallEmailSize = None
    emailWritingTime = None
    emailReadingTime = None
    settlingTime = None
    def __init__(self, meanOfNumberOfEmails = 14.0, sigmaOfNumberOfEmails = 12.0,
                 medianOfLargeEmailSize = 227.0, sigmaOfLargeEmailSize = 1.0,
                 valueOfLargeEmailSize = 928331.2, medianOfSmallEmailSize = 22.7,
                 sigmaOfSmallEmailSize = 1.0, valueOfSmallEmailSize = 9286.11,
                 shapeOfEmailWritingTime = 1.1, scaleOfEmailWritingTime = 2.0,
                 shapeOfEmailReadingTime = 1.1, scaleOfEmailReadingTime = 2.0,
                 settlingTime = 1.0, minStartDelay = 0.1, maxStartDelay = 1.0, 
                 parentLogger = None, minTrafficDelay = 0.0, maxTrafficDelay = 0.0):
        super(Email, self).__init__(settlingTime, minStartDelay, maxStartDelay, 
                minTrafficDelay, maxTrafficDelay)
        self.numberOfEmails = openwns.distribution.ABOVE(openwns.distribution.
                                                         LogNorm(meanOfNumberOfEmails,
                                                                 sigmaOfNumberOfEmails),
                                                         1.0)
        self.emailSizeChoice = openwns.distribution.Uniform(0.0, 1.0)
        self.largeEmailSize = openwns.distribution.ABOVE(openwns.distribution.
                                                         BELOW(Cauchy(medianOfLargeEmailSize,
                                                                      sigmaOfLargeEmailSize,
                                                                      valueOfLargeEmailSize),
                                                               83886080.0), 1.0)
        self.smallEmailSize = openwns.distribution.ABOVE(openwns.distribution.
                                                         BELOW(Cauchy(medianOfSmallEmailSize,
                                                                      sigmaOfSmallEmailSize,
                                                                      valueOfSmallEmailSize),
                                                               83886080.0), 1.0)
        self.emailWritingTime = openwns.distribution.ABOVE(openwns.distribution.
                                                           BELOW(openwns.distribution.
                                                                 Pareto(shapeOfEmailWritingTime,
                                                                        scaleOfEmailWritingTime),
                                                                 123.0), 2.0)
        self.emailReadingTime = openwns.distribution.ABOVE(openwns.distribution.
                                                           BELOW(openwns.distribution.
                                                                 Pareto(shapeOfEmailReadingTime,
                                                                        scaleOfEmailReadingTime),
                                                                 63.0), 2.0)
        self.logger = openwns.logger.Logger("APPL", "Email", True, parentLogger)


class FTP(ClientSessions):
    __plugin__ = "client.FTP"
    amountOfData = None
    readingTime = None
    settlingTime = None
    def __init__(self, meanOfReadingTime = 180.0, meanOfAmountOfData = 2.0,
                 sigmaOfAmountOfData = 0.722, settlingTime = 1.0, minStartDelay = 0.1,
                 maxStartDelay = 1.0, parentLogger = None,
                 minTrafficDelay = 0.0, maxTrafficDelay = 0.0):
        super(FTP, self).__init__(settlingTime, minStartDelay, maxStartDelay, 
                minTrafficDelay, maxTrafficDelay)
        self.readingTime = openwns.distribution.NegExp(meanOfReadingTime)
        self.amountOfData = openwns.distribution.ABOVE(openwns.distribution.
                                                       BELOW(openwns.distribution.
                                                             LogNorm(meanOfAmountOfData,
                                                                     sigmaOfAmountOfData),
                                                             5.0), 0.1)
        self.logger = openwns.logger.Logger("APPL", "FTP", True, parentLogger)


class Video(ClientSessions):
    __plugin__ = "client.Video"
    settlingTime = None
    def __init__(self, settlingTime = 1.0, minStartDelay = 0.1, maxStartDelay = 1.0, parentLogger = None,
                 minTrafficDelay = 0.0, maxTrafficDelay = 0.0):
        super(Video, self).__init__(settlingTime, minStartDelay, maxStartDelay, 
                minTrafficDelay, maxTrafficDelay)
        self.logger = openwns.logger.Logger("APPL", "Video", True, parentLogger)


class VoIP(ClientSessions):
    __plugin__ = "client.VoIP"
    # Choose a different Codec Type from file codec.py in PyConfig directory
    # Existing Codecs:
    # G723, G728, G729, GSM, GSM_EFR, LPC,
    # G722, G726-40, G726-32, G726-24, G726-16,
    # AMR_4_75, AMR_5_15, AMR_5_9, AMR_6_7,
    # AMR_7_4, AMR_7_95, AMR_10_2, AMR_12_2
    codec = None
    stateTransition = None
    # true  means "send Comfort Noise Packets periodically", and
    # false means "switch off sending Comfort Noise Packets"
    comfortNoise = None
    packetIat = None
    voicePacketSize = None
    comfortNoisePacketSize = None
    maxDelay = None
    maxLossRatio = None
    settlingTime = None
    def __init__(self, codecType = GSM(), comfortNoiseChoice = True, settlingTime = 1.0, minStartDelay = 0.1,
                 maxStartDelay = 1.0, parentLogger = None, minTrafficDelay = 0.0, maxTrafficDelay = 0.0):
        super(VoIP, self).__init__(settlingTime, minStartDelay, maxStartDelay, 
                minTrafficDelay, maxTrafficDelay)
        self.codec = codecType
        self.stateTransition = openwns.distribution.Uniform(0.0, 1.0)
        self.comfortNoise = comfortNoiseChoice
        self.packetIat = codecType.packetIat # take from codec specification
        self.voicePacketSize = codecType.packetSize # take from codec specification
        self.comfortNoisePacketSize = codecType.cnPacketSize # take from codec specification
        self.maxDelay = 0.05
        self.maxLossRatio = 0.02
        self.logger = openwns.logger.Logger("APPL", "VoIP", True, parentLogger)


class VideoTelephony(ClientSessions):
    __plugin__ = "client.VideoTelephony"
    # Voice parameters (s. VoIP)
    voiceCodec = None
    stateTransition = None
    comfortNoise = None
    voicePacketIat = None
    voicePacketSize = None
    # Video parameters (s. Video)
    # Choose Codec: MPEG4,H263,H261
    videoCodec = None
    # Choose Format: CIF,QCIF
    videoFormat = None
    # Choose Quantization Scale: 30-30-30,10-14-16,4-4-4
    # Please type it without the minus '-' and two numerics (for example: 040404 instead of 4-4-4)
    videoQuality = None
    # With the settings above the needed parameters for the synthetic Model will be set
    className = None
    params = None
    settlingTime = None
    def __init__(self, voiceCodecType = GSM(), comfortNoiseChoice = True, videoCodecType = 'MPEG4',
                 formatType = 'QCIF', qualityChoice = '081014', settlingTime = 1.0, minStartDelay = 0.1,
                 maxStartDelay = 1.0, parentLogger = None, minTrafficDelay = 0.0, maxTrafficDelay = 0.0):
        super(VideoTelephony, self).__init__(settlingTime, minStartDelay, maxStartDelay, 
                minTrafficDelay, maxTrafficDelay)
        self.voiceCodec = voiceCodecType
        self.stateTransition = openwns.distribution.Uniform(0.0, 1.0)
        self.comfortNoise = comfortNoiseChoice
        self.voicePacketIat = voiceCodecType.packetIat # take from codec specification
        self.voicePacketSize = voiceCodecType.packetSize # take from codec specification
        self.comfortNoisePacketSize = voiceCodecType.cnPacketSize # take from codec specification
        self.videoCodec = videoCodecType
        self.videoFormat = formatType
        self.videoQuality = qualityChoice
        self.className = 'Videotelephony_' + self.videoCodec + '_' + self.videoFormat + '_' + self.videoQuality
        self.params = StatisticalCharacteristics.Database[self.className]
        self.logger = openwns.logger.Logger("APPL", "VideoTelephony", True, parentLogger)


class VideoTrace(ClientSessions):
    __plugin__ = "client.VideoTrace"
    # Choose Genre: Cartoons,Movies,News,Sports
    genre = None
    # Choose Codec: MPEG4,H263,H261
    codec = None    
    # Choose Format: CIF,QCIF
    format = None
    # Choose RateControl: VBR (Variable Bit Rate), TBR (Target Bit Rate)
    rateControl = None
    # if RateControl == 'VBR'
    # Choose Quantization Scale: 30-30-30, 10-14-16, 04-04-04
    # else if RateControl == 'TBR'
    # Choose Bit Rate: 64kbps, 128kbps, 256kbps
    quality = None
    pyConfigDirIndex = None
    traceFileDir = None
    movieChoice = None
    settlingTime = None
    def __init__(self, genreChoice = 'Movies', codecChoice = 'MPEG4',
                 formatChoice = 'QCIF', rateControlChoice = 'VBR', qualityChoice = '30-30-30',
                 settlingTime = 1.0, minStartDelay = 0.1, maxStartDelay = 1.0, parentLogger = None,
                 minTrafficDelay = 0.0, maxTrafficDelay = 0.0):
        super(VideoTrace, self).__init__(settlingTime, minStartDelay, maxStartDelay, 
                minTrafficDelay, maxTrafficDelay)
        self.genre = genreChoice
        self.codec = codecChoice
        self.format = formatChoice
        self.rateControl = rateControlChoice
        self.quality = qualityChoice
        self.pyConfigDirIndex = self.getPyConfigDirIndex(sys.path)
        self.traceFileDir = os.path.join(sys.path[self.pyConfigDirIndex], "applications",
                                         "VideoTraceFiles", self.codec, self.format,
                                         self.rateControl, self.quality, self.genre)
        self.movieChoice = os.path.join(self.traceFileDir , "star_wars_v_vhs.dat")
        self.logger = openwns.logger.Logger("APPL", "VideoTrace", True, parentLogger)

    def getPyConfigDirIndex(self, list):
        for s in list:
            if 'PyConfig' in s:
                 if os.path.exists(s):
                     return list.index(s)
        return None
            

class WWW(ClientSessions):
    __plugin__ = "client.WWW"
    readingTime = None
    parsingTime = None
    embeddedObjectsPerPage = None
    settlingTime = None
    def __init__(self, meanOfReadingTime = 30.0, meanOfParsingTime = 0.13,
                 shapeOfEmbeddedObjects = 1.1, scaleOfEmbeddedObjects = 2.0,
                 settlingTime = 1.0, minStartDelay = 0.1, maxStartDelay = 1.0,
                 parentLogger = None, minTrafficDelay = 0.0, maxTrafficDelay = 0.0):
        super(WWW, self).__init__(settlingTime, minStartDelay, maxStartDelay, 
                minTrafficDelay, maxTrafficDelay)
        self.readingTime = openwns.distribution.NegExp(meanOfReadingTime)
        self.parsingTime = openwns.distribution.NegExp(meanOfParsingTime)
        self.embeddedObjectsPerPage = openwns.distribution.ABOVE(openwns.distribution.
                                                                 BELOW(openwns.distribution.
                                                                       Pareto(shapeOfEmbeddedObjects,
                                                                              scaleOfEmbeddedObjects),
                                                                       53.0),2.0)
        self.logger = openwns.logger.Logger("APPL", "WWW", True, parentLogger)


class WiMAXVideo(ClientSessions):
    __plugin__ = "client.WiMAXVideo"
    maxDelay = None
    settlingTime = None
    maxLossRatio = None
    def __init__(self, settlingTime = 1.0, minStartDelay = 0.1, maxStartDelay = 1.0, 
                 parentLogger = None, minTrafficDelay = 0.0, maxTrafficDelay = 0.0):
        super(WiMAXVideo, self).__init__(settlingTime, minStartDelay, maxStartDelay, 
                minTrafficDelay, maxTrafficDelay)
        self.maxDelay = 5.0
        self.maxLossRatio = 0.02
        self.logger = openwns.logger.Logger("APPL", "WiMAXVideo", True, parentLogger)


class WiMAXVideoTelephony(ClientSessions):
    __plugin__ = "client.WiMAXVideoTelephony"
    # Voice parameters (s. VoIP)
    codec = None
    stateTransition = None
    comfortNoise = None
    voicePacketIat = None
    voicePacketSize = None
    comfortNoisePacketSize = None
    # Video parameters
    frameRate = None
    iFramePacketSize = None
    shiftI = None
    bFramePacketSize = None
    pFramePacketSize = None
    settlingTime = None
    def __init__(self, codecType = GSM(), comfortNoiseChoice = True,
                 framesPerSecond = 25.0, scaleOfIFrame = 5.15, shapeOfIFrame = 863.0,
                 shiftOfIFrameSize = 3949.0, meanOfBFrameSize = 147.0, sigmaOfBFrameSize = 74.0,
                 meanOfPFrameSize = 259.0, sigmaOfPFrameSize = 134.0, settlingTime = 1.0,
                 minStartDelay = 0.1, maxStartDelay = 1.0, parentLogger = None,
                 minTrafficDelay = 0.0, maxTrafficDelay = 0.0):
        super(WiMAXVideoTelephony, self).__init__(settlingTime, minStartDelay, maxStartDelay, 
                minTrafficDelay, maxTrafficDelay)
        self.codec = codecType
        self.stateTransition = openwns.distribution.Uniform(0.0, 1.0)
        self.comfortNoise = comfortNoiseChoice
        self.voicePacketIat = codecType.packetIat # take from codec specification
        self.voicePacketSize = codecType.packetSize # take from codec specification
        self.comfortNoisePacketSize = codecType.cnPacketSize # take from codec specification
        self.frameRate = framesPerSecond
        self.iFramePacketSize = openwns.distribution.ABOVE(openwns.distribution.
                                                           BELOW(openwns.distribution.
                                                                 Weibull(scaleOfIFrame,
                                                                         shapeOfIFrame),
                                                                 1240.0), 85.0)
        self.shiftI = shiftOfIFrameSize
        self.bFramePacketSize = openwns.distribution.ABOVE(openwns.distribution.
                                                           BELOW(openwns.distribution.
                                                                 LogNorm(meanOfBFrameSize,
                                                                         sigmaOfBFrameSize),
                                                                 882.0), 35.0)
        self.pFramePacketSize = openwns.distribution.ABOVE(openwns.distribution.
                                                           BELOW(openwns.distribution.
                                                                 LogNorm(meanOfPFrameSize,
                                                                         sigmaOfPFrameSize),
                                                                 1663.0), 100.0)
        self.logger = openwns.logger.Logger("APPL", "WiMAXVideoTelephony", True, parentLogger)
