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

# component.py

import openwns.node
import openwns.logger
import openwns.qos
from openwns.pyconfig import attrsetter
from openwns.pyconfig import Sealed

class Client(openwns.node.Component):
    nameInComponentFactory = "applications.component.Client"
    logger = None
    sessions = None
    bindings = None

    def __init__(self, _node, _name, parentLogger = None):
        super(Client, self).__init__(_node, _name)
        self.sessions = []
        self.bindings = []
        self.logger = openwns.logger.Logger("applications", True, parentLogger)

    def addTraffic(self, _bindings, _sessions):
        self.bindings.append(_bindings)
        self.sessions.append(_sessions)


class Server(openwns.node.Component):
    nameInComponentFactory = "applications.component.Server"
    logger = None
    listenerBindings = None

    def __init__(self, _node, _name, parentLogger = None):
        super(Server, self).__init__(_node, _name)
        self.listenerBindings = []
        self.logger = openwns.logger.Logger("applications", True, parentLogger)

    def addListenerBinding(self, _listenerBindings):
        self.listenerBindings.append(_listenerBindings)


class TLBinding(Sealed):
    __plugin__ = "TLBinding"
    # the UDP service
    tlService = "tcp.connectionService"
    # UDP specific parameters
    domainName = None
    destinationDomainName = None
    destinationPort = None
    qosClass = None
    listenPort = None
    logger = None

    def __init__(self, _domainName, _destinationDomainName, _destionationPort, _qosClass, _listenPort, parentLogger = None):
        self.domainName = _domainName;
        self.destinationDomainName = _destinationDomainName
        self.destinationPort = _destionationPort
        self.qosClass = _qosClass.number
        self.listenPort = _listenPort
        self.logger = openwns.logger.Logger("APPL", "TLBinding", True, parentLogger)


class TLListenerBinding(Sealed):
    __plugin__ = "TLListenerBinding"
    # the UDP service
    tlService = "tcp.connectionService"
    # UDP specific parameters
    probeWindow = 1.0 # [s], 1.0 is quite slow, 0.01s=10ms is often reasonable, but may generate a lot of samples
    domainName = None
    destinationDomainName = None
    destinationPort = None
    qosClass = None
    listenPort = None
    session = None
    logger = None

    def __init__(self, _domainName, _destinationDomainName, _destionationPort, _qosClass, _listenPort, _session, parentLogger = None):
        self.domainName = _domainName;
        self.destinationDomainName = _destinationDomainName
        self.destinationPort = _destionationPort  
        self.qosClass = _qosClass.number
        self.listenPort = _listenPort
        self.session = _session
        self.logger = openwns.logger.Logger("APPL", "TLListenerBinding", True, parentLogger)

