## Makefile.am -- Process this file with automake to produce Makefile.in
## Copyright (C) 2002 George Tzanetakis
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2, or (at your option)
## any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

SUBDIRS=plugins tcl


MAINTAINERCLEANFILES = Makefile.in


EXTRA_DIST 	= tests.txt pvocJazz pvoc.bat dha2dhin2.txt netExtract.ms pvoc.ms

INCLUDES        = -I$(top_builddir) -I$(top_builddir)/marsyas \
                -I$(top_srcdir) -I$(top_srcdir)/marsyas -I$(Includes)



bin_PROGRAMS    = sfinfo sfplugin sfplay extract bextract \
		  mkcollection phasevocoder \
		  pitchextract sfplaygui tempo \
		  server msl tutorial \
		  ERBTest marsyasTests \
		  SeneffEarTest record \
		  virtualsensor mixer mugle classifyFile midiTest \
		  LPCExample peakClustering peVocoder

sfinfo_SOURCES     = sfinfo.cpp 
sfinfo_LDADD       = $(top_builddir)/marsyas/libmarsyas.a \
		     $(Libs)

sfplay_SOURCES     = sfplay.cpp 
sfplay_LDADD       = $(top_builddir)/marsyas/libmarsyas.a \
		     $(Libs)

tutorial_SOURCES     = tutorial.cpp 
tutorial_LDADD       = $(top_builddir)/marsyas/libmarsyas.a \
		     	$(Libs)

tempo_SOURCES     = tempo.cpp 
tempo_LDADD       = $(top_builddir)/marsyas/libmarsyas.a \
		     $(Libs)

sfplugin_SOURCES     = sfplugin.cpp 
sfplugin_LDADD       = $(top_builddir)/marsyas/libmarsyas.a \
		     $(Libs)

phasevocoder_SOURCES = phasevocoder.cpp 
phasevocoder_LDADD   = $(top_builddir)/marsyas/libmarsyas.a \
		     $(Libs)

pitchextract_SOURCES     = pitchextract.cpp 
pitchextract_LDADD       = $(top_builddir)/marsyas/libmarsyas.a \
			   $(Libs)

extract_SOURCES     =  extract.cpp 
extract_LDADD       =  $(top_builddir)/marsyas/libmarsyas.a \
		       $(Libs)

bextract_SOURCES     =  bextract.cpp 
bextract_LDADD       = $(top_builddir)/marsyas/libmarsyas.a \
		  	$(Libs)

sfplaygui_SOURCES     = sfplaygui.cpp 
sfplaygui_LDADD       = $(top_builddir)/marsyas/libmarsyas.a \
			$(Libs)

mkcollection_SOURCES     = mkcollection.cpp 
mkcollection_LDADD       = $(top_builddir)/marsyas/libmarsyas.a \
			$(Libs)

server_SOURCES     = server_main.cpp 
server_LDADD       = $(top_builddir)/marsyas/libmarsyas.a \
			$(Libs)

msl_SOURCES     = msl.cpp 
msl_LDADD       = $(top_builddir)/marsyas/libmarsyas.a \
			$(Libs) $(MslLibs)

                
ERBTest_SOURCES = ERBTest.cpp
ERBTest_LDADD   = $(top_builddir)/marsyas/libmarsyas.a \
			$(Libs)

marsyasTests_SOURCES     = marsyasTests.cpp 
marsyasTests_LDADD       = $(top_builddir)/marsyas/libmarsyas.a \
		     $(Libs)

SeneffEarTest_SOURCES     = SeneffEarTest.cpp 
SeneffEarTest_LDADD       = $(top_builddir)/marsyas/libmarsyas.a \
		     $(Libs)

record_SOURCES     = record.cpp 
record_LDADD       = $(top_builddir)/marsyas/libmarsyas.a \
		     $(Libs)

virtualsensor_SOURCES     = virtualsensor.cpp 
virtualsensor_LDADD       = $(top_builddir)/marsyas/libmarsyas.a \
		     $(Libs)

mixer_SOURCES     = mixer.cpp 
mixer_LDADD       = $(top_builddir)/marsyas/libmarsyas.a \
		    $(Libs)

mugle_SOURCES     = mugle.cpp 
mugle_LDADD       = $(top_builddir)/marsyas/libmarsyas.a \
		    $(Libs)

classifyFile_SOURCES     = classifyFile.cpp 
classifyFile_LDADD       = $(top_builddir)/marsyas/libmarsyas.a \
		    $(Libs)


midiTest_SOURCES     = midiTest.cpp 
midiTest_LDADD       = $(top_builddir)/marsyas/libmarsyas.a \
		    $(Libs)



LPCExample_SOURCES     = LPCExample.cpp 
LPCExample_LDADD       = $(top_builddir)/marsyas/libmarsyas.a \
		    $(Libs)


peakClustering_SOURCES = peakClustering.cpp 
peakClustering_LDADD   = $(top_builddir)/marsyas/libmarsyas.a \
		     $(Libs)


peVocoder_SOURCES = peVocoder.cpp 
peVocoder_LDADD   = $(top_builddir)/marsyas/libmarsyas.a \
		     $(Libs)
