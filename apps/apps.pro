include( $$quote( "$$BASEDIR/marsyasConfig.pri" ) )

TEMPLATE = subdirs

marsyasConsoleApps {
	SUBDIRS += \
		ERBTest \
		SeneffEarTest \
		bextract \
		#boomchick \
		classifyFile \
		extract \
		marsyasTests \
		#midiTest \
		#mixer \
		mkcollection \
		#msl \
		#muggle \
		#orcarecord \
		#peVocoder \
		peakClustering \
		phasevocoder \
		pitchextract \
		record \
		scheduler \
		#server_main \
		sfinfo \
		sfplay \
		#sfplaygui \
		sfplugin \
		#tempo \
		textract \
		tutorial \
		#virtualsensor \
		vocalEffort \
}

marsyasQt4Apps:SUBDIRS += Qt4Apps
	