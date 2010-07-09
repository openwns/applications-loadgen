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

class SeparateOnlyServers(ITreeNodeGenerator):
    """ Separate only Servers.
    """
    def __call__(self, pathname):
        for node in Enumerated(by = 'Appl.StationType', keys = [1], names = ['Server'], format='%s')(pathname):
            yield node

class SeparateOnlyClients(ITreeNodeGenerator):
    """ Separate only Clients.
    """
    def __call__(self, pathname):
        for node in Enumerated(by = 'Appl.StationType', keys = [2], names = ['Client'], format='%s')(pathname):
            yield node

class SeparateBySessionTypes(ITreeNodeGenerator):
    """ Separate by type of session.
    """
    def __call__(self, pathname):
        for node in Enumerated(by = 'Appl.SessionType', keys = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10], names = ['CBR', 'Email', 'FTP', 'VoIP', 'Video', 'VideoTelephony', 'VideoTrace', 'WWW', 'WiMAXVideo', 'WiMAXVideoTelephony'], format='%s')(pathname):
            yield node


class SeparateByVoIP_Video(ITreeNodeGenerator):
    """ Separate by type of session.
    """
    def __call__(self, pathname):
        for node in Enumerated(by = 'Appl.SessionType', keys = [4, 9], names = ['VoIP', 'WiMAXVideo'], format='%s')(pathname):
            yield node
