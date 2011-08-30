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

from openwns.evaluation import *

probePrefix = 'applications.'

def installEvaluation(sim,
                      serverIdList = [],
                      clientIdList = [],
                      settlingTime = 0.0):

    sourceName = probePrefix + 'packet.incoming.size'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime))
    servers = node.appendChildren(SeparateOnlyServers())
    clients = node.appendChildren(SeparateOnlyClients())
    servers.appendChildren(Moments(name = sourceName, description = 'Incoming packet size [Bit]'))
    clients.appendChildren(Moments(name = sourceName, description = 'Incoming packet size [Bit]'))
    servers.getLeafs().appendChildren(SeparateBySessionTypes())
    clients.getLeafs().appendChildren(SeparateBySessionTypes())
    servers.getLeafs().appendChildren(Moments(name = sourceName, description = 'Incoming packet size [Bit]'))
    clients.getLeafs().appendChildren(Moments(name = sourceName, description = 'Incoming packet size [Bit]'))


    sourceName = probePrefix + 'packet.outgoing.size'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime))
    servers = node.appendChildren(SeparateOnlyServers())
    clients = node.appendChildren(SeparateOnlyClients())
    servers.appendChildren(Moments(name = sourceName, description = 'Outgoing packet size [Bit]'))
    clients.appendChildren(Moments(name = sourceName, description = 'Outgoing packet size [Bit]'))
    servers.getLeafs().appendChildren(SeparateBySessionTypes())
    clients.getLeafs().appendChildren(SeparateBySessionTypes())
    servers.getLeafs().appendChildren(Moments(name = sourceName, description = 'Outgoing packet size [Bit]'))
    clients.getLeafs().appendChildren(Moments(name = sourceName, description = 'Outgoing packet size [Bit]'))


    sourceName = probePrefix + 'packet.incoming.delay'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime))
    servers = node.appendChildren(SeparateOnlyServers())
    clients = node.appendChildren(SeparateOnlyClients())
    servers.appendChildren(Moments(name = sourceName, description = 'End to end packet delay [s]'))
    clients.appendChildren(Moments(name = sourceName, description = 'End to end packet delay [s]'))
    servers.getLeafs().appendChildren(SeparateBySessionTypes())
    clients.getLeafs().appendChildren(SeparateBySessionTypes())
    servers.getLeafs().appendChildren(PDF(name = sourceName, description = 'End to end packet delay [s]',
                                              maxXValue = 0.50, resolution = 500, minXValue = 0.0))
    clients.getLeafs().appendChildren(PDF(name = sourceName, description = 'End to end packet delay [s]',
                                              maxXValue = 0.50, resolution = 5000, minXValue = 0.0))
    #servers.getLeafs().appendChildren(TimeSeries(name = sourceName, description = 'End to end packet delay [s]'))
    #clients.getLeafs().appendChildren(TimeSeries(name = sourceName, description = 'End to end packet delay [s]'))
    

    sourceName = probePrefix + 'packet.outgoing.iat'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime))
    servers = node.appendChildren(SeparateOnlyServers())
    clients = node.appendChildren(SeparateOnlyClients())
    servers.appendChildren(Moments(name = sourceName, description = 'Packet iat [s]'))
    clients.appendChildren(Moments(name = sourceName, description = 'Packet iat [s]'))
    servers.getLeafs().appendChildren(SeparateBySessionTypes())
    clients.getLeafs().appendChildren(SeparateBySessionTypes())
    servers.getLeafs().appendChildren(Moments(name = sourceName, description = 'Packet iat [s]'))
    clients.getLeafs().appendChildren(Moments(name = sourceName, description = 'Packet iat [s]'))


    sourceName = probePrefix + 'window.incoming.bitThroughput'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime))
    servers = node.appendChildren(SeparateOnlyServers())
    clients = node.appendChildren(SeparateOnlyClients())
    servers.appendChildren(Moments(name = sourceName, description = 'Average windowed incoming bit throughput [Bit/s]'))
    clients.appendChildren(Moments(name = sourceName, description = 'Average windowed incoming bit throughput [Bit/s]'))
    servers.getLeafs().appendChildren(SeparateBySessionTypes())
    clients.getLeafs().appendChildren(SeparateBySessionTypes())
    servers.getLeafs().appendChildren(Moments(name = sourceName, description = 'Average windowed incoming bit throughput [Bit/s]'))
    clients.getLeafs().appendChildren(Moments(name = sourceName, description = 'Average windowed incoming bit throughput [Bit/s]'))


    sourceName = probePrefix + 'window.outgoing.bitThroughput'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime))
    servers = node.appendChildren(SeparateOnlyServers())
    clients = node.appendChildren(SeparateOnlyClients())
    servers.appendChildren(Moments(name = sourceName, description = 'Average windowed outgoing bit throughput [Bit/s]'))
    clients.appendChildren(Moments(name = sourceName, description = 'Average windowed outgoing bit throughput [Bit/s]'))
    servers.getLeafs().appendChildren(SeparateBySessionTypes())
    clients.getLeafs().appendChildren(SeparateBySessionTypes())
    servers.getLeafs().appendChildren(Moments(name = sourceName, description = 'Average windowed outgoing bit throughput [Bit/s]'))
    clients.getLeafs().appendChildren(Moments(name = sourceName, description = 'Average windowed outgoing bit throughput [Bit/s]'))


    sourceName = probePrefix + 'window.incoming.packetThroughput'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime))
    servers = node.appendChildren(SeparateOnlyServers())
    clients = node.appendChildren(SeparateOnlyClients())
    servers.appendChildren(Moments(name = sourceName, description = 'Average windowed incoming packet throughput [Packets/s]'))
    clients.appendChildren(Moments(name = sourceName, description = 'Average windowed incoming packet throughput [Packets/s]'))
    servers.getLeafs().appendChildren(SeparateBySessionTypes())
    clients.getLeafs().appendChildren(SeparateBySessionTypes())
    servers.getLeafs().appendChildren(Moments(name = sourceName, description = 'Average windowed incoming packet throughput [Packets/s]'))
    clients.getLeafs().appendChildren(Moments(name = sourceName, description = 'Average windowed incoming packet throughput [Packets/s]'))


    sourceName = probePrefix + 'window.outgoing.packetThroughput'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime))
    servers = node.appendChildren(SeparateOnlyServers())
    clients = node.appendChildren(SeparateOnlyClients())
    servers.appendChildren(Moments(name = sourceName, description = 'Average windowed outgoing packet throughput [Packets/s]'))
    clients.appendChildren(Moments(name = sourceName, description = 'Average windowed outgoing packet throughput [Packets/s]'))
    servers.getLeafs().appendChildren(SeparateBySessionTypes())
    clients.getLeafs().appendChildren(SeparateBySessionTypes())
    servers.getLeafs().appendChildren(Moments(name = sourceName, description = 'Average windowed outgoing packet throughput [Packets/s]'))
    clients.getLeafs().appendChildren(Moments(name = sourceName, description = 'Average windowed outgoing packet throughput [Packets/s]'))


    sourceName = probePrefix + 'session.incoming.bitThroughput'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime))
    servers = node.appendChildren(SeparateOnlyServers())
    clients = node.appendChildren(SeparateOnlyClients())
    servers.appendChildren(Moments(name = sourceName, description = 'Average incoming bit throughput per session [Bit/s]'))
    clients.appendChildren(Moments(name = sourceName, description = 'Average incoming bit throughput per session [Bit/s]'))
    servers.getLeafs().appendChildren(SeparateBySessionTypes())
    clients.getLeafs().appendChildren(SeparateBySessionTypes())
    servers.getLeafs().appendChildren(Moments(name = sourceName, description = 'Average incoming bit throughput per session [Bit/s]'))
    clients.getLeafs().appendChildren(Moments(name = sourceName, description = 'Average incoming bit throughput per session [Bit/s]'))


    sourceName = probePrefix + 'session.outgoing.bitThroughput'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime))
    servers = node.appendChildren(SeparateOnlyServers())
    clients = node.appendChildren(SeparateOnlyClients())
    servers.appendChildren(Moments(name = sourceName, description = 'Average outgoing bit throughput per session [Bit/s]'))
    clients.appendChildren(Moments(name = sourceName, description = 'Average outgoing bit throughput per session [Bit/s]'))
    servers.getLeafs().appendChildren(SeparateBySessionTypes())
    clients.getLeafs().appendChildren(SeparateBySessionTypes())
    servers.getLeafs().appendChildren(Moments(name = sourceName, description = 'Average outgoing bit throughput per session [Bit/s]'))
    clients.getLeafs().appendChildren(Moments(name = sourceName, description = 'Average outgoing bit throughput per session [Bit/s]'))


    sourceName = probePrefix + 'session.incoming.packetThroughput'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime))
    servers = node.appendChildren(SeparateOnlyServers())
    clients = node.appendChildren(SeparateOnlyClients())
    servers.appendChildren(Moments(name = sourceName, description = 'Average incoming packet throughput per session [Packets/s]'))
    clients.appendChildren(Moments(name = sourceName, description = 'Average incoming packet throughput per session [Packets/s]'))
    servers.getLeafs().appendChildren(SeparateBySessionTypes())
    clients.getLeafs().appendChildren(SeparateBySessionTypes())
    servers.getLeafs().appendChildren(Moments(name = sourceName, description = 'Average incoming packet throughput per session [Packets/s]'))
    clients.getLeafs().appendChildren(Moments(name = sourceName, description = 'Average incoming packet throughput per session [Packets/s]'))


    sourceName = probePrefix + 'session.outgoing.packetThroughput'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime))
    servers = node.appendChildren(SeparateOnlyServers())
    clients = node.appendChildren(SeparateOnlyClients())
    servers.appendChildren(Moments(name = sourceName, description = 'Average outgoing packet throughput per session [Packets/s]'))
    clients.appendChildren(Moments(name = sourceName, description = 'Average outgoing packet throughput per session [Packets/s]'))
    servers.getLeafs().appendChildren(SeparateBySessionTypes())
    clients.getLeafs().appendChildren(SeparateBySessionTypes())
    servers.getLeafs().appendChildren(Moments(name = sourceName, description = 'Average incoming packet throughput per session [Packets/s]'))
    clients.getLeafs().appendChildren(Moments(name = sourceName, description = 'Average incoming packet throughput per session [Packets/s]'))


    sourceName = probePrefix + 'session.duration'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime))
    servers = node.appendChildren(SeparateOnlyServers())
    clients = node.appendChildren(SeparateOnlyClients())
    servers.getLeafs().appendChildren(Moments(name = sourceName, description = 'Length of session [s]'))
    clients.getLeafs().appendChildren(Moments(name = sourceName, description = 'Length of session [s]'))
    servers.getLeafs().appendChildren(SeparateBySessionTypes())
    clients.getLeafs().appendChildren(SeparateBySessionTypes())
    servers.getLeafs().appendChildren(Moments(name = sourceName, description = 'Length of session [s]'))
    clients.getLeafs().appendChildren(Moments(name = sourceName, description = 'Length of session [s]'))

    sourceName = probePrefix + 'session.incoming.packetLoss'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime))
    servers = node.appendChildren(SeparateOnlyServers())
    clients = node.appendChildren(SeparateOnlyClients())
    servers.appendChildren(Moments(name = sourceName, description = 'Incoming packet loss ratio'))
    clients.appendChildren(Moments(name = sourceName, description = 'Incoming packet loss ratio'))
    servers.getLeafs().appendChildren(SeparateBySessionTypes())
    clients.getLeafs().appendChildren(SeparateBySessionTypes())
    servers.getLeafs().appendChildren(Moments(name = sourceName, description = 'Incoming packets loss ratio'))
    clients.getLeafs().appendChildren(Moments(name = sourceName, description = 'Incoming packets loss ratio'))

    sourceName = probePrefix + 'session.userSatisfaction'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    node.getLeafs().appendChildren(SettlingTimeGuard(settlingTime))
    servers = node.appendChildren(SeparateOnlyServers())
    clients = node.appendChildren(SeparateOnlyClients())
    servers.appendChildren(Moments(name = sourceName, description = 'Amount of satisfied users'))
    clients.appendChildren(Moments(name = sourceName, description = 'Amount of satisfied users'))
    servers.getLeafs().appendChildren(SeparateBySessionTypes())
    clients.getLeafs().appendChildren(SeparateBySessionTypes())
#    servers.appendChildren(Accept(by = 'Appl.SessionType', ifIn = [4]))
#    clients.appendChildren(Accept(by = 'Appl.SessionType', ifIn = [4]))
    servers.getLeafs().appendChildren(Moments(name = sourceName, description = 'Amount of satisfied users'))
    clients.getLeafs().appendChildren(Moments(name = sourceName, description = 'Amount of satisfied users'))

    # We do not need a SettlingTimeGuard here because connections should be established before the settling time
    sourceName = probePrefix + 'connectionEstablished'
    node = openwns.evaluation.createSourceNode(sim, sourceName)
    clients = node.appendChildren(SeparateOnlyClients())
    clients.getLeafs().appendChildren(Moments(name = sourceName, description = 'Number of established connections'))
    clients.getLeafs().appendChildren(SeparateBySessionTypes())
    clients.getLeafs().appendChildren(Moments(name = sourceName, description = 'Number of established connections'))
