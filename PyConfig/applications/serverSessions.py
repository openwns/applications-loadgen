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

# serverSessions.py

import openwns.distribution
from cauchy import *
from codec import *
from videoParameters import *
import sys

class ServerSessions(object):
    logger = None
    settlingTime = None
    def __init__(self, settlingTime = 1.0):
        self.settlingTime = settlingTime

class CBR(ServerSessions):
    __plugin__ = "server.CBR"
    packetSize = None
    bitRate = None
    def __init__(self, packetSize = 512, bitRate = 64, settlingTime = 1.0, parentLogger = None):
        super(CBR, self).__init__(settlingTime)
        self.packetSize = openwns.distribution.Fixed(packetSize)
        self.bitRate = openwns.distribution.Fixed(bitRate)
        self.logger = openwns.logger.Logger("APPL", "CBR", True, parentLogger)


class Email(ServerSessions):
    __plugin__ = "server.Email"
    numberOfEmails = None
    emailChoice = None
    # The maximum emailsize is 10 MB
    largeEmailSize = None
    smallEmailSize = None
    def __init__(self, meanOfNumberOfEmails = 30.0, sigmaOfNumberOfEmails = 17.0,
                 medianOfLargeEmailSize = 227.0, sigmaOfLargeEmailSize = 1.0,
                 valueOfLargeEmailSize = 9283331.2, medianOfSmallEmailSize = 22.7,
                 sigmaOfSmallEmailSize = 1.0, valueOfSmallEmailSize = 9286.11,
                 settlingTime = 1.0, parentLogger = None):
        super(Email, self).__init__(settlingTime)
        self.numberOfEmails = openwns.distribution.ABOVE(openwns.distribution.
                                                         LogNorm(meanOfNumberOfEmails,
                                                                 sigmaOfNumberOfEmails),
                                                         1.0)
        self.emailChoice = openwns.distribution.Uniform(0.0, 1.0)
        self.largeEmailSize = openwns.distribution.ABOVE(openwns.distribution.
                                                         BELOW(Cauchy(medianOfLargeEmailSize,
                                                                      sigmaOfLargeEmailSize,
                                                                      valueOfLargeEmailSize),
                                                               10240.0), 1.0)
        self.smallEmailSize = openwns.distribution.ABOVE(openwns.distribution.
                                                         BELOW(Cauchy(medianOfSmallEmailSize,
                                                                      sigmaOfSmallEmailSize,
                                                                      valueOfSmallEmailSize),
                                                               10240.0) , 1.0)
        self.logger = openwns.logger.Logger("APPL", "Email", True, parentLogger)


class FTP(ServerSessions):
    __plugin__ = "server.FTP"
    settlingTime = None
    def __init__(self, settlingTime = 1.0, parentLogger = None):
        super(FTP, self).__init__(settlingTime)
        self.logger = openwns.logger.Logger("APPL", "FTP", True, parentLogger)


class Video(ServerSessions):   
    __plugin__ = "server.Video"
    # Choose the process with that the video frames should be generated:
    # PDF, P-AR, P-ARMA, P-FARIMA, P-FDN
    # Also possible are the same processes without projection to a target distribution:
    # AR, FARIMA, FDN. Please DO NOT use these ones, because they also produce negative
    # packet sizes due to the lack of projection and therefore they are not applicable.
    frameGeneratingProcess = None
    # Choose Genre: Cartoons,Movies,News,Sports
    genre = None
    # Choose Codec: MPEG4,H263,H261
    codec = None
    # Choose Format: CIF,QCIF
    format = None
    # Choose Quantization Scale: 30-30-30,10-14-16,04-04-04
    # Type it without the minus '-' and two numerics (for example: 040404 instead of 4-4-4)
    quality = None
    # With the settings above the needed parameters for the synthetic Model will be set
    className = None
    params = None
    def __init__(self, frameGenProcess = 'PDF', genreChoice = 'Movies', codecChoice = 'MPEG4',
                 formatChoice = 'CIF', qualityChoice = '040404', settlingTime = 1.0, parentLogger = None):
        super(Video, self).__init__(settlingTime)
        self.frameGeneratingProcess = frameGenProcess
        self.genre = genreChoice
        self.codec = codecChoice
        self.format = formatChoice
        self.quality = qualityChoice
        self.className = self.genre + '_' + self.codec + '_' + self.format + '_' + self.quality
        self.params = StatisticalCharacteristics.Database[self.className]
        self.logger = openwns.logger.Logger("APPL", "Video", True, parentLogger)


class VoIP(ServerSessions):
    __plugin__ = "server.VoIP"
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
    def __init__(self, codecType = GSM(), comfortNoiseChoice = True,
                 settlingTime = 1.0, trafficStartDelay = 0.0, parentLogger = None):
        super(VoIP, self).__init__(settlingTime)
        self.codec = codecType
        self.stateTransition = openwns.distribution.Uniform(0.0, 1.0)
        self.comfortNoise = comfortNoiseChoice
        self.packetIat = codecType.packetIat # take from codec specification
        self.voicePacketSize = codecType.packetSize # take from codec specification
        self.comfortNoisePacketSize = codecType.cnPacketSize # take from codec specification
        self.maxDelay = 0.05
        self.maxLossRatio = 0.02
        self.trafficStartDelay = trafficStartDelay
        self.logger = openwns.logger.Logger("APPL", "VoIP", True, parentLogger)


class VideoTelephony(ServerSessions):
    __plugin__ = "server.VideoTelephony"
    # Voice parameters (s. VoIP)
    voiceCodec = None
    stateTransition = None
    comfortNoise = None
    voicePacketIat = None
    voicePacketSize = None
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
    def __init__(self, voiceCodecType = GSM(), comfortNoiseChoice = True,
                 videoCodecType = 'MPEG4', videoFormatType = 'QCIF', videoQualityChoice = '081014',
                 settlingTime = 1.0, parentLogger = None):
        super(VideoTelephony, self).__init__(settlingTime)
        self.voiceCodec = voiceCodecType
        self.stateTransition = openwns.distribution.Uniform(0.0, 1.0)
        self.comfortNoise = comfortNoiseChoice
        self.voicePacketIat = voiceCodecType.packetIat # take from codec specification
        self.voicePacketSize = voiceCodecType.packetSize # take from codec specification
        self.comfortNoisePacketSize = voiceCodecType.cnPacketSize # take from codec specification
        self.videoCodec = videoCodecType
        self.videoFormat = videoFormatType
        self.videoQuality = videoQualityChoice
        self.className = 'Videotelephony_' + self.videoCodec + '_' + self.videoFormat + '_' + self.videoQuality
        self.params = StatisticalCharacteristics.Database[self.className]
        self.logger = openwns.logger.Logger("APPL", "VideoTelephony", True, parentLogger)

class VideoTrace(ServerSessions):
    __plugin__ = "server.VideoTrace"
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
    def __init__(self, genreChoice = 'Movies', codecChoice = 'MPEG4', formatChoice = 'QCIF',
                 rateControlChoice = 'VBR', qualityChoice = '303030', settlingTime = 1.0, parentLogger = None):
        super(VideoTrace, self).__init__(settlingTime)
        self.genre = genreChoice
        self.codec = codecChoice
        self.format = formatChoice
        self.rateControl = rateControlChoice
        self.quality = qualityChoice
        self.logger = openwns.logger.Logger("APPL", "VideoTrace", True, parentLogger)

class WWW(ServerSessions):
    __plugin__ = "server.WWW"
    sizeOfMainObject = None
    sizeOfEmbeddedObject = None
    def  __init__(self, meanSizeOfMainObject = 10710.0, sigmaOfMainObjectSize = 158.22,
                  meanSizeOfEmbeddedObject = 7758.0, sigmaOfEmbeddedObjectSize = 355.2,
                  settlingTime = 1.0, parentLogger = None):
        super(WWW, self).__init__(settlingTime)
        self.sizeOfMainObject = openwns.distribution.ABOVE(openwns.distribution.
                                                           BELOW(openwns.distribution.
                                                                 LogNorm(meanSizeOfMainObject,
                                                                         sigmaOfMainObjectSize),
                                                                 2097152.0), 100.0)
        self.sizeOfEmbeddedObject = openwns.distribution.ABOVE(openwns.distribution.
                                                               BELOW(openwns.distribution.
                                                                     LogNorm(meanSizeOfEmbeddedObject,
                                                                             sigmaOfEmbeddedObjectSize),
                                                                     2097152.0), 50.0)
        self.logger = openwns.logger.Logger("APPL", "WWW", True, parentLogger)


class WiMAXVideo(ServerSessions):
    __plugin__ = "server.WiMAXVideo"
    frameRate = None
    numberOfPacketsPerFrame = None
    packetSize = None
    packetIat = None
    def __init__(self, framesPerSecond = 10.0, numberOfPackets = 8.0, shapeOfPacketSize = 1.2,
                 scaleOfPacketSize = 40.0 , shapeOfPacketIat = 1.2, scaleOfPacketIat = 2.5,
                 settlingTime = 1.0, parentLogger = None):
        super(WiMAXVideo, self).__init__(settlingTime)
        self.frameRate = framesPerSecond
        self.numberOfPacketsPerFrame = numberOfPackets
        self.packetSize = openwns.distribution.ABOVE(openwns.distribution.
                                                        BELOW(openwns.distribution.
                                                              Pareto(shapeOfPacketSize,
                                                                     scaleOfPacketSize),
                                                              250.0), 1.0)
        self.packetIat = openwns.distribution.BELOW(openwns.distribution.
                                                    Pareto(shapeOfPacketIat,
                                                           scaleOfPacketIat),
                                                    12.5)
        self.logger = openwns.logger.Logger("APPL", "WiMAXVideo", True, parentLogger)


class WiMAXVideoTelephony(ServerSessions):
    __plugin__ = "server.WiMAXVideoTelephony"
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
    def __init__(self, codecType = GSM() , comfortNoiseChoice = True,
                 framesPerSecond = 25.0, scaleOfIFrame = 5.15, shapeOfIFrame = 863.0,
                 shiftOfIFrameSize = 3949.0, meanOfBFrameSize = 147.0, sigmaOfBFrameSize = 74.0,
                 meanOfPFrameSize = 259.0, sigmaOfPFrameSize = 134.0, settlingTime = 1.0, parentLogger = None):
        super(WiMAXVideoTelephony, self).__init__(settlingTime)
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
