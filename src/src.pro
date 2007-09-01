include( $$quote( "$$BASEDIR/marsyasConfig.pri" ) )

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += src

marsyasLib {
	SUBDIRS += src
}
marsyasConsoleApps {
	SUBDIRS += apps
}


