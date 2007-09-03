include( $$quote( "$$BASEDIR/marsyasConfig.pri" ) )

TEMPLATE = subdirs
CONFIG += ordered

marsyasLib {
	SUBDIRS += marsyas
	SUBDIRS += marsyasqt_wrapper
}
marsyasConsoleApps {
	SUBDIRS += apps
}


