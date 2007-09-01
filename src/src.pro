include( $$quote( "$$BASEDIR/marsyasConfig.pri" ) )

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += src

marsyasLib {
	SUBDIRS += marsyas
}
marsyasConsoleApps {
	SUBDIRS += apps
}


