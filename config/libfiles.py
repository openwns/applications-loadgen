libname = 'applications'
srcFiles = [

    # base
    'src/Applications.cpp',
    'src/node/component/Component.cpp',
    'src/node/component/client/Component.cpp',
    'src/node/component/server/Component.cpp',

    #distribution
    'src/distribution/Cauchy.cpp',

    # session
    'src/session/Session.cpp',
    'src/session/PDU.cpp',
    'src/session/client/Session.cpp',
    'src/session/server/Session.cpp',
    'src/session/client/CBR.cpp',
    'src/session/client/Email.cpp',
    'src/session/client/FTP.cpp',
    'src/session/client/Video.cpp',
    'src/session/client/wimax/Video.cpp',
    'src/session/client/VideoTelephony.cpp',
    'src/session/client/wimax/VideoTelephony.cpp',
    'src/session/client/VideoTrace.cpp',
    'src/session/client/VoIP.cpp',
    'src/session/client/WWW.cpp',
    'src/session/server/CBR.cpp',
    'src/session/server/Email.cpp',
    'src/session/server/FTP.cpp',
    'src/session/server/Video.cpp',
    'src/session/server/wimax/Video.cpp',
    'src/session/server/VideoTelephony.cpp',
    'src/session/server/wimax/VideoTelephony.cpp',
    'src/session/server/VideoTrace.cpp',
    'src/session/server/VoIP.cpp',
    'src/session/server/WWW.cpp',
    'src/session/client/TLBinding.cpp',
    'src/session/server/TLListenerBinding.cpp'
    ]

hppFiles = [
    
    # base
    'src/Applications.hpp',
    'src/node/component/Component.hpp',
    'src/node/component/client/Component.hpp',
    'src/node/component/server/Component.hpp',

    #distribution
    'src/distribution/Cauchy.hpp',

    # session
    'src/session/Binding.hpp',
    'src/session/Session.hpp',
    'src/session/PDU.hpp',
    'src/session/client/Session.hpp',
    'src/session/server/Session.hpp',
    'src/session/client/CBR.hpp',
    'src/session/client/Email.hpp',
    'src/session/client/FTP.hpp',
    'src/session/client/Video.hpp',
    'src/session/client/wimax/Video.hpp',
    'src/session/client/VideoTelephony.hpp',
    'src/session/client/wimax/VideoTelephony.hpp',
    'src/session/client/VideoTrace.hpp',
    'src/session/client/VoIP.hpp',
    'src/session/client/WWW.hpp',
    'src/session/server/CBR.hpp',
    'src/session/server/Email.hpp',
    'src/session/server/FTP.hpp',
    'src/session/server/Video.hpp',
    'src/session/server/wimax/Video.hpp',
    'src/session/server/VideoTelephony.hpp',
    'src/session/server/wimax/VideoTelephony.hpp',
    'src/session/server/VideoTrace.hpp',
    'src/session/server/VoIP.hpp',
    'src/session/server/WWW.hpp',
    'src/session/client/TLBinding.hpp',
    'src/session/server/TLListenerBinding.hpp'
    ]

pyconfigs = [  
    
    'applications/applications.py',
    'applications/clientSessions.py',
    'applications/component.py',
    'applications/__init__.py',
    'applications/serverSessions.py',
    'applications/evaluation/default.py',
    'applications/evaluation/centerCell.py',
    'applications/evaluation/__init__.py',
    'applications/evaluation/generators.py',
    'applications/codec.py',
    'applications/videoParameters.py',
    'applications/cauchy.py',

    #VideoTraceFiles
    'applications/VideoTraceFiles/MPEG4/QCIF/VBR/30-30-30/Movies/charlies_angles_dvd.dat',
    'applications/VideoTraceFiles/MPEG4/QCIF/VBR/30-30-30/Movies/die_another_day_dvd.dat',
    'applications/VideoTraceFiles/MPEG4/QCIF/VBR/30-30-30/Movies/die_hard_dvd.dat',
    'applications/VideoTraceFiles/MPEG4/QCIF/VBR/30-30-30/Movies/dogma_dvd.dat',
    'applications/VideoTraceFiles/MPEG4/QCIF/VBR/30-30-30/Movies/flatliners_dvd.dat',
    'applications/VideoTraceFiles/MPEG4/QCIF/VBR/30-30-30/Movies/friends_vol4_dvd.dat',
    'applications/VideoTraceFiles/MPEG4/QCIF/VBR/30-30-30/Movies/star_wars_iv_vhs.dat',
    'applications/VideoTraceFiles/MPEG4/QCIF/VBR/30-30-30/Movies/star_wars_v_vhs.dat',
    'applications/VideoTraceFiles/MPEG4/QCIF/VBR/30-30-30/Movies/terminator_dvd.dat',
    'applications/VideoTraceFiles/MPEG4/QCIF/VBR/30-30-30/Movies/terminator_vhs.dat',
    'applications/VideoTraceFiles/MPEG4/QCIF/VBR/30-30-30/Movies/the_firm_dvd.dat',
    'applications/VideoTraceFiles/MPEG4/QCIF/VBR/30-30-30/Movies/the_firm_vhs.dat'
    ]

dependencies = []
Return('libname srcFiles hppFiles pyconfigs dependencies')
