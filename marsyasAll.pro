include( marsyasConfig.pri )

TEMPLATE = subdirs
CONFIG += ordered

marsyasLib {
	SUBDIRS += src
}
marsyasConsoleApps {
	SUBDIRS += apps
}

message ( ***************************************************** )
unix:message ( Done! Run make now to build Marsyas... )
win32:win32-msvc2005:message ( Done! Run nmake now to build Marsyas... )
message ( ***************************************************** )
