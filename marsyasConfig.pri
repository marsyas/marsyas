######################################################################
# qmake internal options
######################################################################

CONFIG	+= warn_on
CONFIG	+= thread	 # multithreaded
CONFIG -= qt         # by default do not include any Qt stuff

######################################################################
# Build marsyas lib
######################################################################

CONFIG += marsyasLib

#DEFINES+= MARSYAS_VERSION=0.2.10 #is this the way to set a MACRO?!???????

######################################################################
# Build marsyas apps
######################################################################

CONFIG += marsyasConsoleApps
CONFIG += marsyasQt4Apps

######################################################################
# release/debug mode
######################################################################

unix:CONFIG	+= release
win32:win32-msvc2005:CONFIG += debug_and_release

######################################################################
# enable/disable ASSERTIONS/WARNINGS/DIAGNOSTICS/LOGS, etc
######################################################################

#CONFIG += marsyasASSERTS		#turn on assertions
#CONFIG += marsyasPROFILING		#turn on profiling

#CONFIG += marsyasLOGWARNINGS	#warning messages in log
#CONFIG += marsyasLOGDEBUGS		#debug messages in log
#CONFIG += marsyasLOGDIAGNOSTICS	#diagnostic messages in log
#CONFIG += marsyasLOG2FILE			#log to file
#CONFIG += marsyasLOG2STDOUT		#log to stdout
#CONFIG += marsyasLOG2GUI			#log to GUI

######################################################################
# Audio and MIDI I/O support in marsyas lib
######################################################################

CONFIG += marsyasAUDIOIO
CONFIG += marsyasMIDIIO

# ------------------------------------
# choose which Audio APIs to use:
# ------------------------------------
win32:CONFIG += marsyasAUDIOIO_DS
#win32:CONFIG += marsyasAUDIOIO_ASIO
unix:!macx:CONFIG += marsyasAUDIOIO_ALSA
#unix:!macx:CONFIG += marsyasAUDIOIO_JACK
#unix:!macx:CONFIG += marsyasAUDIOIO_OSS

### no choice about audio API for MacOSX.

######################################################################
# Qt suport for Marsyas library 
######################################################################

#CONFIG += marsyasQT

######################################################################
# Adds support for MATLAB engine classes, for debug/release builds
######################################################################

CONFIG	+= marsyasMATLABdebug
# CONFIG	+= marsyasMATLABrelease

######################################################################
# Adds support for MP3 MAD
######################################################################

CONFIG += marsyasMAD

######################################################################
# Adds support for qglviewer (QT-based OpenGL viewer)
######################################################################

#CONFIG += marsyasQGLVIEWER

######################################################################
# Adds support for oggvorbis
######################################################################

#CONFIG	+= marsyasOGG



#*********************************************************************
# let qmake set stuff based on above user definitions
# DO NOT CHANGE THE LINES BELLOW!
#*********************************************************************
message ( ***************************************************** )
message ( Configuring Marsyas build with the following options: )

INCLUDEPATH += $$quote( "$$BASEDIR/marsyas" ) 

CONFIG(debug, debug|release) {
	message ( -> DEBUG building )
	DEFINES += MARSYAS_DEBUG
	
	#in debug, activate assertions
	CONFIG += marsyasASSERTS		#turn on assertions
	
	#in debug, activate some logging by default 
	CONFIG += marsyasLOGWARNINGS	#warning messages in log
	CONFIG += marsyasLOGDEBUGS		#debug messages in log
	CONFIG += marsyasLOG2STDOUT			#log to stdout
}

CONFIG(release, debug|release) {
	message (  -> RELEASE building )
	DEFINES += MARSYAS_RELEASE
}

marsyasQT {
	message ( -> Qt support in marsyas.lib )
	CONFIG += qt
	DEFINES += MARSYAS_QT
	INCLUDEPATH += $$quote( "$$BASEDIR/marsyas/Qt" ) 
}

marsyasMATLABdebug {
	CONFIG(debug, debug|release) {
		message ( -> MATLAB support  for debug builds)
		DEFINES += MARSYAS_MATLAB
		INCLUDEPATH += $$quote( "$$BASEDIR/marsyas/MATLAB" ) 
	}
}

marsyasMATLABrelease {
	CONFIG(release, debug|release) {
		message ( -> MATLAB support  for release builds)
		DEFINES += MARSYAS_MATLAB
		INCLUDEPATH += $$quote( "$$BASEDIR/marsyas/MATLAB" ) 
	}
}


marsyasAUDIOIO:message( -> AUDIO I/O support (RtAudio) )
marsyasAUDIOIO_ALSA:message ( --> ALSA  )
marsyasAUDIOIO_JACK:message ( --> JACK )
marsyasAUDIOIO_OSS:message ( --> OSS )
marsyasAUDIOIO_COREAUDIO:message( --> CORE AUDIO )
marsyasAUDIOIO_DS:message( --> DIRECT SHOW )
marsyasAUDIOIO_ASIO:message( --> ASIO )

marsyasMIDIIO:message ( -> MIDI I/O support (RtMIDI) )
marsyasMAD:message( -> MP3 MAD support )
marsyasOGG:message( -> Ogg Vorbis support )

marsyasConsoleApps:message ( -> build Console Apps )
marsyasQt4Apps:message(  -> build Qt4 Apps )
marsyasQGLVIEWER:message (-> QGVVIEWER support) 

unix:{
!macx {    # qmake detects osx as "unix" in 4.2.2.  :/
	DEFINES += MARSYAS_LINUX
	LIBS += -lm
			
	marsyasAUDIOIO {
		DEFINES += MARSYAS_AUDIOIO
		marsyasAUDIOIO_ALSA {
			DEFINES += \
				MARSYAS_ALSA \
				__LINUX_ALSA__
			LIBS += -lasound -lpthread
		}
		marsyasAUDIOIO_JACK {
			DEFINES += \
				MARSYAS_JACK \
				__LINUX_JACK__
			LIBS += -ljack -lpthread
		}
		marsyasAUDIOIO_OSS {
			DEFINES += \
				MARSYAS_OSS \
				__LINUX_OSS__
			LIBS += -lpthread
		}
	}
	
	marsyasMIDIIO {
		DEFINES += \
			MARSYAS_MIDIIO \
			MARSYAS_ALSASEQ \
			__LINUX_ALSASEQ__
		LIBS += -lasound -lpthread
	}
	
	marsyasMATLABdebug {
		CONFIG(debug, debug|release) {
			#INCLUDEPATH += ???
			#LIBS += ???
			DEFINES 	+= MARSYAS_MATLAB
		}
	}
	marsyasMATLABrelease {
		CONFIG(release, debug|release) {
			#INCLUDEPATH += ???
			#LIBS += ???
			DEFINES 	+= MARSYAS_MATLAB
		}
	}
	
	marsyasMAD {
		LIBS += -lmad
		DEFINES += MARSYAS_MAD
	}
}

   # For all unix, including macx
   
   
}

macx {
	DEFINES += MARSYAS_MACOSX
	LIBS += -framework CoreFoundation
	
	marsyasAUDIOIO {
		DEFINES += \
			MARSYAS_AUDIOIO \
			MARSYAS_COREAUDIO \
			__MACOSX_CORE__
		LIBS += -framework CoreAudio -lpthread
	}
	
	marsyasMIDIIO {
		DEFINES += \
			MARSYAS_MIDIIO \
			MARSYAS_COREMIDI \
			__MACOSX_CORE__
		LIBS += -framework CoreMidi -framework CoreAudio
	}
		
	marsyasMATLABdebug {
		CONFIG(debug, debug|release) {
			INCLUDEPATH += $$(MATLAB)/extern/include
			LIBPATH += $$(MATLAB)/bin/maci \
				   $$(MATLAB)/sys/os/maci 
			LIBS += -leng -lmx -lut -lmat -licudata -licuuc -licui18n -licuio -lz -lhdf5
			DEFINES 	+= MARSYAS_MATLAB
		}
	}
	marsyasMATLABrelease {
		CONFIG(release, debug|release) {
			INCLUDEPATH += $$(MATLAB)/extern/include
			LIBPATH += $$(MATLAB)/bin/maci \
				   $$(MATLAB)/sys/os/maci 
			LIBS += -leng -lmx -lut -lmat -licudata -licuuc -licui18n -licuio -lz -lhdf5
			DEFINES 	+= MARSYAS_MATLAB
		}
	}

	marsyasMAD {
		DEFINES += MARSYAS_MAD 
		LIBS += -lmad 
	}
}

win32 {
	win32-msvc2005:QMAKE_CXXFLAGS_DEBUG += /ZI /Od
	win32-msvc2005:QMAKE_LFLAGS_DEBUG += /INCREMENTAL

	DEFINES += 	MARSYAS_WIN32 \
				WIN32 \
				_WINDOWS 	
	LIBS 	+= 	-luser32

	CONFIG(release, debug|release){
		DEFINES += NDEBUG
	}
	CONFIG(debug, debug|release){
		DEFINES += _DEBUG
	}
	
	marsyasAUDIOIO {
		DEFINES += MARSYAS_AUDIOIO
		marsyasAUDIOIO_DS {
			DEFINES += \
				MARSYAS_DS \
				__WINDOWS_DS__
			LIBS += dsound.lib 
			LIBPATH += $$quote( "$$(DXSDK_DIR)Lib/x86" ) #DXSDK_DIR must be defined as an environment variable in windows!
			INCLUDEPATH += $$quote( "$$(DXSDK_DIR)Include" ) #include path for dsound.h
		}
		marsyasAUDIOIO_ASIO {
			DEFINES += \
				MARSYAS_ASIO \
				__WINDOWS_ASIO_
			DEFINES -= UNICODE
			SOURCES += 
				$$quote( "$$BASEDIR/otherlibs/asio/asio.cpp" ) \
				$$quote( "$$BASEDIR/otherlibs/asio/asiodrivers.cpp" ) \
				$$quote( "$$BASEDIR/otherlibs/asio/asiolist.cpp" ) \
				$$quote( "$$BASEDIR/otherlibs/asio/iasiothiscallresolver.cpp" )
			HEADERS += \ 
				$$quote( "$$BASEDIR/otherlibs/asio/asio.h" ) \
				$$quote( "$$BASEDIR/otherlibs/asio/asiodrivers.h" ) \
				$$quote( "$$BASEDIR/otherlibs/asio/asiodrvr.h" ) \
				$$quote( "$$BASEDIR/otherlibs/asio/asiolist.h" ) \
				$$quote( "$$BASEDIR/otherlibs/asio/asiosys.h" ) \
				$$quote( "$$BASEDIR/otherlibs/asio/ginclude.h" ) \
				$$quote( "$$BASEDIR/otherlibs/asio/iasiodrv.h" ) \
				$$quote( "$$BASEDIR/otherlibs/asio/iasiothiscallresolver.h" )
		}
	}
	
	marsyasMIDIIO {
		DEFINES += \
			MARSYAS_MIDIIO \
			MARSYAS_WINMM \
			__WINDOWS_MM__
		LIBS += -lwinmm		
	}
	
	marsyasMATLABdebug{
		CONFIG(debug, debug|release) {
			DEFINES += MARSYAS_MATLAB
			INCLUDEPATH += $$quote( "$$(MATLAB)/extern/include" )
			LIBS += libeng.lib libmx.lib libut.lib 
			LIBPATH += $$quote( \"$$(MATLAB)/extern/lib/win32/microsoft\" )
		}
	}
	marsyasMATLABrelease{
		CONFIG(release, debug|release) {
			DEFINES += MARSYAS_MATLAB
			INCLUDEPATH += $$quote( "$$(MATLAB)/extern/include" )
			LIBS += libeng.lib libmx.lib libut.lib 
			LIBPATH += $$quote( \"$$(MATLAB)/extern/lib/win32/microsoft\" )
		}
	}
	
	marsyasMAD {
		INCLUDEPATH += $$quote( "$$(LIBMAD)" )
		DEFINES += MARSYAS_MAD
		CONFIG(release, debug|release){
			message(Building with libMAD MP3 support (release).)
			LIBS += libmad.lib 
			LIBPATH += $$quote( \"$$(LIBMAD)/msvc++/Release\" )
		}
		CONFIG(debug, debug|release){
			message(Building with libMAD MP3 support (debug).)
			LIBS += libmad.lib 
			LIBPATH += $$quote( \"$$(LIBMAD)/msvc++/Debug\" )
		}
	}
}

marsyasASSERTS {
	message ( -> Assertions turned on )
	DEFINES += MARSYAS_ASSERTS
}
marsyasPROFILING {
	message ( -> Profiling turned on )
	DEFINES += MARSYAS_PROFILING
}

marsyasLOGWARNINGS {
	message ( -> Warning messages in log )
	DEFINES += MARSYAS_LOG_WARNINGS
}
marsyasLOGDEBUGS {
	message ( -> Debug messages in log )
	DEFINES += MARSYAS_LOG_DEBUGS
}
marsyasLOGDIAGNOSTICS {
	message ( -> Diagnostic messages in log )
	DEFINES += MARSYAS_LOG_DIAGNOSTICS
}
marsyasLOG2FILE {
	message ( --> Log to file )
	DEFINES += MARSYAS_LOG2FILE
}
marsyasLOG2STDOUT {
	message ( --> Log to stdout )
	DEFINES += MARSYAS_LOG2STDOUT
}
marsyasLOG2GUI {
	message ( --> Log to GUI )
	DEFINES += MARSYAS_LOG2GUI
}



