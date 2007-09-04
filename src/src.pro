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
marsyasQt4Apps {
	SUBDIRS += Qt4Apps
}


