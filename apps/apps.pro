include( $$quote( "$$BASEDIR/marsyasConfig.pri" ) )

message( Configuring Marsyas Apps build )
	
TEMPLATE = subdirs

marsyasConsoleApps {
	SUBDIRS += \
		ERBTest \
		SeneffEarTest \
		bextract \
		#boomchick \
		classifyFile \
		extract \
		mudbox \
		#midiTest \
		#mixer \
		mkcollection \
		#msl \
		#muggle \
		#orcarecord \
		peakSynth \
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
		WHaSp
}

marsyasQt4Apps:SUBDIRS += Qt4Apps
	
