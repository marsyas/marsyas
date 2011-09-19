# if(MARSYAS_AUDIOIO)
#        add_subdirectory(RtAudio)
# endif(MARSYAS_AUDIOIO)

# if(MARSYAS_MIDIIO)
#        add_subdirectory(RtMidi)
# endif(MARSYAS_MIDIIO)

if(MARSYAS_PNG) 
	add_subdirectory(zlib-1.2.3)
	add_subdirectory(libpng-1.2.35)
endif(MARSYAS_PNG)


