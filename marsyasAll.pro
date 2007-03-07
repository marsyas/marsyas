include( marsyasConfig.pri )

message ( Configuring MarsyasAll ) 

TEMPLATE = subdirs
CONFIG += ordered

marsyasLib:SUBDIRS += marsyas
marsyasConsoleApps:SUBDIRS += apps

unix:message ( Done! Run make now to build Marsyas... )
win32:win32-msvc2005:message ( Done! Run nmake now to build Marsyas... )
