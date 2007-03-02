include( marsyasConfig.pri )

TEMPLATE = subdirs
CONFIG += ordered

marsyasLib:SUBDIRS += marsyas
marsyasConsoleApps:SUBDIRS += apps
