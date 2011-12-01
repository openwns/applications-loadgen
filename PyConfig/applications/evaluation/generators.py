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

# generators.py

from openwns.evaluation import * 
import default
import applications

class SeparateOnlyServers(ITreeNodeGenerator):
    """ Separate only Servers.
    """
    def __init__(self, string = 'Server'):
        self.string = string

    def __call__(self, pathname):
        for node in Enumerated(by = 'Appl.StationType', keys = [1], names = [self.string], format='%s')(pathname):
            yield node

class SeparateOnlyClients(ITreeNodeGenerator):
    """ Separate only Clients.
    """
    def __init__(self, string = 'Server'):
        self.string = string

    def __call__(self, pathname):
        for node in Enumerated(by = 'Appl.StationType', keys = [2], names = [self.string], format='%s')(pathname):
            yield node

class SeparateBySessionTypes(ITreeNodeGenerator):
    """ Separate by type of session.
    """

    names = []
    keys = []    

    def __init__(self, sesTypes = applications.sesStrTypes.keys()):
        sesStrTypesSet = set(sesTypes)
        for s in sesStrTypesSet:
            self.names.append(s)
            self.keys.append(applications.sesStrTypes[s])

    def __call__(self, pathname):
        for node in Enumerated(by = 'Appl.SessionType', keys = self.keys, names = self.names, format='%s')(pathname):
            yield node


class SeparateByVoIP_Video(ITreeNodeGenerator):
    """ Separate by type of session.
    """
    def __call__(self, pathname):
        for node in Enumerated(by = 'Appl.SessionType', keys = [4, 9], names = ['VoIP', 'WiMAXVideo'], format='%s')(pathname):
            yield node
