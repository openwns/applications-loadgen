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

# __init__.py

import openwns
import applications

openwns.simulator.OpenWNS.modules.applications = applications.Applications()

sesTypes = dict({1: 'CBR', 2: 'Email', 3: 'FTP', 
            4: 'VoIP', 5: 'Video', 6: 'VideoTelephony', 
            7: 'VideoTrace', 8: 'WWW', 9: 'WiMAXVideo', 
            10: 'WiMAXVideoTelephony'})

sesStrTypes = dict({'CBR': 1, 'Email': 2, 'FTP': 3, 
            'VoIP': 4, 'Video': 5, 'VideoTelephony': 6, 
            'VideoTrace': 7, 'WWW': 8, 'WiMAXVideo': 9, 
            'WiMAXVideoTelephony': 10})



