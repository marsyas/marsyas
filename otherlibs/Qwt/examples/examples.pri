# -*- mode: sh -*- ################################################
# Qwt Widget Library
# Copyright (C) 1997   Josef Wilgen
# Copyright (C) 2002   Uwe Rathmann
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the Qwt License, Version 1.0
###################################################################

include( ../../qwtconfig.pri )

TEMPLATE     = app

MOC_DIR      = moc
OBJECTS_DIR  = obj
INCLUDEPATH += ../../src
DEPENDPATH  += ../../src

unix:LIBS        += -L../../lib -lqwt

win32:QwtDll {
	DEFINES    += QT_DLL QWT_DLL
}

release:win32-msvc:LIBS  += ../../lib/release/qwt.lib
release:win32-msvc.net:LIBS  += ../../lib/release/qwt.lib
release:win32-msvc2005:LIBS += ../../lib/release/qwt.lib
release:win32-g++:LIBS   += -L../../lib/release -lqwt

debug:win32-msvc:LIBS  += ../../lib/debug/qwt.lib
debug:win32-msvc.net:LIBS  += ../../lib/debug/qwt.lib
debug:win32-msvc2005:LIBS += ../../lib/debug/qwt.lib
debug:win32-g++:LIBS   += -L../../lib/debug -lqwt
