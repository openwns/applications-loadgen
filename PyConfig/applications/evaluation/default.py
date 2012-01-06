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

# default.py

from applications.evaluation.generators import *
import applications

from openwns.evaluation import *

probePrefix = 'applications.'

def installEvaluation(sim,
                      loggingIds = [],
                      sessionTypes = set(applications.sesStrTypes),
                      settlingTime = 0.0,
                      numNodes = 100,
                      perSessionType = False,
                      useCellId = False):

    if(useCellId):
        nodeString = "Appl.CellId"
        senderString = "Appl.CellId"
        maxId = numNodes + len(loggingIds) + 2;
    else:
        nodeString = "wns.node.Node.id"
        senderString = "Appl.SenderId"
        maxId = max(loggingIds) + 1;

        

    sourceName = probePrefix + 'packet.incoming.size'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node = node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime))
    servers = node.appendChildren(SeparateOnlyServers("Uplink"))
    clients = node.appendChildren(SeparateOnlyClients("Donwlink"))
    clients.getLeafs().appendChildren(Accept(by=nodeString, ifIn = loggingIds))
    servers.getLeafs().appendChildren(Accept(by=senderString, ifIn = loggingIds))
    if(perSessionType):
        servers.getLeafs().appendChildren(SeparateBySessionTypes(sessionTypes))
        clients.getLeafs().appendChildren(SeparateBySessionTypes(sessionTypes))
    servers.appendChildren(Moments(name = sourceName, description = 'Incoming packet size [Bit]'))
    clients.appendChildren(Moments(name = sourceName, description = 'Incoming packet size [Bit]'))

    sourceName = probePrefix + 'packet.outgoing.size'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node = node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime))
    servers = node.appendChildren(SeparateOnlyServers("Donwlink"))
    clients = node.appendChildren(SeparateOnlyClients("Uplink"))
    clients.getLeafs().appendChildren(Accept(by=senderString, ifIn = loggingIds))
    servers.getLeafs().appendChildren(Accept(by=nodeString, ifIn = loggingIds))
    if(perSessionType):
        servers.getLeafs().appendChildren(SeparateBySessionTypes(sessionTypes))
        clients.getLeafs().appendChildren(SeparateBySessionTypes(sessionTypes))
    servers.appendChildren(Moments(name = sourceName, description = 'Outgoing packet size [Bit]'))
    clients.appendChildren(Moments(name = sourceName, description = 'Outgoing packet size [Bit]'))

    sourceName = probePrefix + 'packet.incoming.delay'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node = node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime))
    servers = node.appendChildren(SeparateOnlyServers("Uplink"))
    clients = node.appendChildren(SeparateOnlyClients("Downlink"))
    clients.getLeafs().appendChildren(Accept(by=nodeString, ifIn = loggingIds))
    servers.getLeafs().appendChildren(Accept(by=senderString, ifIn = loggingIds))
    if(perSessionType):
        servers.getLeafs().appendChildren(SeparateBySessionTypes(sessionTypes))
        clients.getLeafs().appendChildren(SeparateBySessionTypes(sessionTypes))
    servers.getLeafs().appendChildren(PDF(name = sourceName, description = 'End to end packet delay [s]',
                                              maxXValue = 0.50, resolution = 500, minXValue = 0.0))
    clients.getLeafs().appendChildren(PDF(name = sourceName, description = 'End to end packet delay [s]',
                                              maxXValue = 0.50, resolution = 5000, minXValue = 0.0))
    servers.getLeafs().appendChildren(Plot2D(xDataKey = "Appl.SenderId",
                                            minX = 0,
                                            maxX = maxId,
                                            resolution = maxId,
                                            statEvals = ['mean','deviation','max']))
    clients.getLeafs().appendChildren(Plot2D(xDataKey = "wns.node.Node.id",
                                            minX = 0,
                                            maxX = maxId,
                                            resolution = maxId,
                                            statEvals = ['mean','deviation','max']))

    sourceName = probePrefix + 'packet.outgoing.iat'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node = node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime))
    servers = node.appendChildren(SeparateOnlyServers("Downlink"))
    clients = node.appendChildren(SeparateOnlyClients("Uplink"))
    clients.getLeafs().appendChildren(Accept(by=senderString, ifIn = loggingIds))
    servers.getLeafs().appendChildren(Accept(by=nodeString, ifIn = loggingIds))
    if(perSessionType):
        servers.getLeafs().appendChildren(SeparateBySessionTypes(sessionTypes))
        clients.getLeafs().appendChildren(SeparateBySessionTypes(sessionTypes))
    servers.getLeafs().appendChildren(Moments(name = sourceName, description = 'Packet iat [s]'))
    clients.getLeafs().appendChildren(Moments(name = sourceName, description = 'Packet iat [s]'))


    sourceName = probePrefix + 'window.incoming.bitThroughput'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node = node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime))
    servers = node.appendChildren(SeparateOnlyServers("Uplink"))
    clients = node.appendChildren(SeparateOnlyClients("Downlink"))
    clients.getLeafs().appendChildren(Accept(by=nodeString, ifIn = loggingIds))
    servers.getLeafs().appendChildren(Accept(by=senderString, ifIn = loggingIds))
    if(perSessionType):
        servers.getLeafs().appendChildren(SeparateBySessionTypes(sessionTypes))
        clients.getLeafs().appendChildren(SeparateBySessionTypes(sessionTypes))
    servers.getLeafs().appendChildren(Moments(name = sourceName, description = 'Average windowed incoming bit throughput [Bit/s]'))
    clients.getLeafs().appendChildren(Moments(name = sourceName, description = 'Average windowed incoming bit throughput [Bit/s]'))

    sourceName = probePrefix + 'window.outgoing.bitThroughput'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node = node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime))
    servers = node.appendChildren(SeparateOnlyServers("Downlink"))
    clients = node.appendChildren(SeparateOnlyClients("Uplink"))
    clients.getLeafs().appendChildren(Accept(by=senderString, ifIn = loggingIds))
    servers.getLeafs().appendChildren(Accept(by=nodeString, ifIn = loggingIds))
    if(perSessionType):
        servers.getLeafs().appendChildren(SeparateBySessionTypes(sessionTypes))
        clients.getLeafs().appendChildren(SeparateBySessionTypes(sessionTypes))
    servers.getLeafs().appendChildren(Moments(name = sourceName, description = 'Average windowed outgoing bit throughput [Bit/s]'))
    clients.getLeafs().appendChildren(Moments(name = sourceName, description = 'Average windowed outgoing bit throughput [Bit/s]'))

    sourceName = probePrefix + 'window.incoming.packetThroughput'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node = node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime))
    servers = node.appendChildren(SeparateOnlyServers("Uplink"))
    clients = node.appendChildren(SeparateOnlyClients("Downlink"))
    clients.getLeafs().appendChildren(Accept(by=nodeString, ifIn = loggingIds))
    servers.getLeafs().appendChildren(Accept(by=senderString, ifIn = loggingIds))
    if(perSessionType):
        servers.getLeafs().appendChildren(SeparateBySessionTypes(sessionTypes))
        clients.getLeafs().appendChildren(SeparateBySessionTypes(sessionTypes))
    servers.getLeafs().appendChildren(Moments(name = sourceName, description = 'Average windowed incoming packet throughput [Packets/s]'))
    clients.getLeafs().appendChildren(Moments(name = sourceName, description = 'Average windowed incoming packet throughput [Packets/s]'))

    sourceName = probePrefix + 'window.outgoing.packetThroughput'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node = node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime))
    servers = node.appendChildren(SeparateOnlyServers("Downlink"))
    clients = node.appendChildren(SeparateOnlyClients("Uplink"))
    clients.getLeafs().appendChildren(Accept(by=senderString, ifIn = loggingIds))
    servers.getLeafs().appendChildren(Accept(by=nodeString, ifIn = loggingIds))
    if(perSessionType):
        servers.getLeafs().appendChildren(SeparateBySessionTypes(sessionTypes))
        clients.getLeafs().appendChildren(SeparateBySessionTypes(sessionTypes))
    servers.getLeafs().appendChildren(Moments(name = sourceName, description = 'Average windowed outgoing packet throughput [Packets/s]'))
    clients.getLeafs().appendChildren(Moments(name = sourceName, description = 'Average windowed outgoing packet throughput [Packets/s]'))

    sourceName = probePrefix + 'session.incoming.bitThroughput'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node = node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime))
    servers = node.appendChildren(SeparateOnlyServers("Uplink"))
    clients = node.appendChildren(SeparateOnlyClients("Downlink"))
    clients.getLeafs().appendChildren(Accept(by=nodeString, ifIn = loggingIds))
    servers.getLeafs().appendChildren(Accept(by=senderString, ifIn = loggingIds))
    if(perSessionType):
        servers.getLeafs().appendChildren(SeparateBySessionTypes(sessionTypes))
        clients.getLeafs().appendChildren(SeparateBySessionTypes(sessionTypes))
    servers.getLeafs().appendChildren(Moments(name = sourceName, description = 'Average incoming bit throughput per session [Bit/s]'))
    clients.getLeafs().appendChildren(Moments(name = sourceName, description = 'Average incoming bit throughput per session [Bit/s]'))

    sourceName = probePrefix + 'session.outgoing.bitThroughput'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node = node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime))
    servers = node.appendChildren(SeparateOnlyServers("Downlink"))
    clients = node.appendChildren(SeparateOnlyClients("Uplink"))
    clients.getLeafs().appendChildren(Accept(by=senderString, ifIn = loggingIds))
    servers.getLeafs().appendChildren(Accept(by=nodeString, ifIn = loggingIds))
    if(perSessionType):
        servers.getLeafs().appendChildren(SeparateBySessionTypes(sessionTypes))
        clients.getLeafs().appendChildren(SeparateBySessionTypes(sessionTypes))
    servers.getLeafs().appendChildren(Moments(name = sourceName, description = 'Average outgoing bit throughput per session [Bit/s]'))
    clients.getLeafs().appendChildren(Moments(name = sourceName, description = 'Average outgoing bit throughput per session [Bit/s]'))

    sourceName = probePrefix + 'session.incoming.packetThroughput'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node = node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime))
    servers = node.appendChildren(SeparateOnlyServers("Uplink"))
    clients = node.appendChildren(SeparateOnlyClients("Downlink"))
    clients.getLeafs().appendChildren(Accept(by=nodeString, ifIn = loggingIds))
    servers.getLeafs().appendChildren(Accept(by=senderString, ifIn = loggingIds))
    if(perSessionType):
        servers.getLeafs().appendChildren(SeparateBySessionTypes(sessionTypes))
        clients.getLeafs().appendChildren(SeparateBySessionTypes(sessionTypes))
    servers.getLeafs().appendChildren(Moments(name = sourceName, description = 'Average incoming packet throughput per session [Packets/s]'))
    clients.getLeafs().appendChildren(Moments(name = sourceName, description = 'Average incoming packet throughput per session [Packets/s]'))

    sourceName = probePrefix + 'session.outgoing.packetThroughput'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node = node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime))
    servers = node.appendChildren(SeparateOnlyServers("Downlink"))
    clients = node.appendChildren(SeparateOnlyClients("Uplink"))
    clients.getLeafs().appendChildren(Accept(by=senderString, ifIn = loggingIds))
    servers.getLeafs().appendChildren(Accept(by=nodeString, ifIn = loggingIds))
    if(perSessionType):
        servers.getLeafs().appendChildren(SeparateBySessionTypes(sessionTypes))
        clients.getLeafs().appendChildren(SeparateBySessionTypes(sessionTypes))
    servers.getLeafs().appendChildren(Moments(name = sourceName, description = 'Average outgoing packet throughput per session [Packets/s]'))
    clients.getLeafs().appendChildren(Moments(name = sourceName, description = 'Average outgoing packet throughput per session [Packets/s]'))

    sourceName = probePrefix + 'session.duration'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node = node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime))
    servers = node.appendChildren(SeparateOnlyServers("Uplink"))
    clients = node.appendChildren(SeparateOnlyClients("Downlink"))
    clients.getLeafs().appendChildren(Accept(by=nodeString, ifIn = loggingIds))
    servers.getLeafs().appendChildren(Accept(by=senderString, ifIn = loggingIds))
    if(perSessionType):
        servers.getLeafs().appendChildren(SeparateBySessionTypes(sessionTypes))
        clients.getLeafs().appendChildren(SeparateBySessionTypes(sessionTypes))
    servers.getLeafs().appendChildren(Moments(name = sourceName, description = 'Length of session [s]'))
    clients.getLeafs().appendChildren(Moments(name = sourceName, description = 'Length of session [s]'))

    sourceName = probePrefix + 'session.incoming.packetLoss'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node = node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime))
    servers = node.appendChildren(SeparateOnlyServers("Downlink"))
    clients = node.appendChildren(SeparateOnlyClients("Uplink"))
    clients.getLeafs().appendChildren(Accept(by=nodeString, ifIn = loggingIds))
    servers.getLeafs().appendChildren(Accept(by=senderString, ifIn = loggingIds))
    if(perSessionType):
        servers.getLeafs().appendChildren(SeparateBySessionTypes(sessionTypes))
        clients.getLeafs().appendChildren(SeparateBySessionTypes(sessionTypes))
    servers.getLeafs().appendChildren(PDF(name = sourceName, description = 'Incoming packet loss ratio',
                                              maxXValue = 1.0, resolution = 1000, minXValue = 0.0))
    clients.getLeafs().appendChildren(PDF(name = sourceName, description = 'Incoming packet loss ratio',
                                              maxXValue = 1.0, resolution = 1000, minXValue = 0.0))
    servers.getLeafs().appendChildren(Plot2D(xDataKey = "Appl.SenderId",
                                            minX = 0,
                                            maxX = maxId,
                                            resolution = maxId,
                                            statEvals = ['mean']))    
    clients.getLeafs().appendChildren(Plot2D(xDataKey = "wns.node.Node.id",
                                            minX = 0,
                                            maxX = maxId,
                                            resolution = maxId,
                                            statEvals = ['mean']))

    sourceName = probePrefix + 'session.userSatisfaction'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node = node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime))
    servers = node.appendChildren(SeparateOnlyServers("Downlink"))
    clients = node.appendChildren(SeparateOnlyClients("Uplink"))
    clients.getLeafs().appendChildren(Accept(by='Appl.CellId', ifIn = loggingIds))
    servers.getLeafs().appendChildren(Accept(by='Appl.CellId', ifIn = loggingIds))
    servers.getLeafs().appendChildren(Moments(name = sourceName, description = 'Amount of satisfied users'))
    clients.getLeafs().appendChildren(Moments(name = sourceName, description = 'Amount of satisfied users'))
    if(perSessionType):
        servers.getLeafs().appendChildren(SeparateBySessionTypes(sessionTypes))
        clients.getLeafs().appendChildren(SeparateBySessionTypes(sessionTypes))
    servers.getLeafs().appendChildren(Moments(name = sourceName, description = 'Amount of satisfied users'))
    clients.getLeafs().appendChildren(Moments(name = sourceName, description = 'Amount of satisfied users'))

    # We do not need a SettlingTimeGuard here because connections should be established before the settling time
    sourceName = probePrefix + 'connectionEstablished'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node = node.appendChildren(Accept(by=nodeString, ifIn = loggingIds))
    if(perSessionType):
        node.getLeafs().appendChildren(SeparateBySessionTypes(sessionTypes))
    node.getLeafs().appendChildren(Moments(name = sourceName, description = 'Number of established connections'))
