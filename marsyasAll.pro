include( marsyasConfig.pri )

message ( Configuring MarsyasAll ) 

TEMPLATE = subdirs
CONFIG += ordered

marsyasLib {
	message( Configuring Marsyas Lib build )
	SUBDIRS += marsyas
}

marsyasConsoleApps {
	message( Configuring Marsyas Apps build )
	SUBDIRS += apps
}

unix:message ( Done! Run make now to build Marsyas... )
win32:win32-msvc2005:message ( Done! Run nmake now to build Marsyas... )
