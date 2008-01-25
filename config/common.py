import os
import CNBuildSupport
from CNBuildSupport import CNBSEnvironment
import wnsbase.RCS as RCS

commonEnv = CNBSEnvironment(PROJNAME       = 'applications',
                            AUTODEPS       = ['wns'],
                            SHORTCUTS      = True,
                            LIBRARY        = True,
                            FLATINCLUDES   = False,
                            EXAMPLESSEARCH = 'src',
                            LIBS           = ['python2.4'],
                            PROJMODULES    = ['BASE', 'SERVICE', 'SESSION', 'CLIENT', 'SERVER'],
			    REVISIONCONTROL = RCS.Bazaar('../', 'applications', 'main', '3.0'),
                            )
Return('commonEnv')
