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
		#peakSynth \
		peakClustering \
		phasevocoder \
		pitchextract \
		record \
		regressionChecks \
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
	
