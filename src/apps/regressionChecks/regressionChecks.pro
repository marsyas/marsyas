include ( ../consoleApps.pri )

TEMPLATE = app
CONFIG += console
TARGET = regressionChecks

SOURCES += regressionChecks.cpp
SOURCES += common-reg.cpp coreChecks.cpp basicChecks.cpp analysisChecks.cpp 
HEADERS += common-reg.h coreChecks.h basicChecks.h analysisChecks.h

