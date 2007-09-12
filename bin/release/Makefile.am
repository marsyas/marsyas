## Makefile.am -- Process this file with automake to produce Makefile.in
## Copyright (C) 2002-2007 George Tzanetakis 
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


MAINTAINERCLEANFILES    = Makefile.in
  
INCLUDES += -I$(top_srcdir)/src/marsyas -I$(top_srcdir)/otherlibs/RtAudio -I$(top_srcdir)/otherlibs/RtMidi -I$(top_srcdir)/otherlibs/libsvm -I$(top_srcdir)/src/marsyas/MATLAB

bin_PROGRAMS = sfplay phasevocoder sfplugin bextract mkcollection \
	mudbox record classifyFile peakSynth peakClustering sfinfo \
	extract midiTest boomchick pitchextract orcarecord \
	virtualsensor kea scheduler msl regressionChecks

sfplay_SOURCES = $(top_srcdir)/src/apps/sfplay/sfplay.cpp
sfplay_LDADD = $(top_builddir)/lib/release/libmarsyas.a \
	       $(Libs)

phasevocoder_SOURCES = $(top_srcdir)/src/apps/phasevocoder/phasevocoder.cpp
phasevocoder_LDADD = $(top_builddir)/lib/release/libmarsyas.a \
	       $(Libs)

sfplugin_SOURCES = $(top_srcdir)/src/apps/sfplugin/sfplugin.cpp
sfplugin_LDADD = $(top_builddir)/lib/release/libmarsyas.a \
	       $(Libs)

bextract_SOURCES = $(top_srcdir)/src/apps/bextract/bextract.cpp
bextract_LDADD = $(top_builddir)/lib/release/libmarsyas.a \
	       $(Libs)

mkcollection_SOURCES = $(top_srcdir)/src/apps/mkcollection/mkcollection.cpp
mkcollection_LDADD = $(top_builddir)/lib/release/libmarsyas.a \
	       $(Libs)

mudbox_SOURCES = $(top_srcdir)/src/apps/mudbox/mudbox.cpp
mudbox_LDADD = $(top_builddir)/lib/release/libmarsyas.a \
	       $(Libs)

regressionChecks_SOURCES = \
	$(top_srcdir)/src/apps/regressionChecks/regressionChecks.cpp \
	$(top_srcdir)/src/apps/regressionChecks/common-reg.cpp \
	$(top_srcdir)/src/apps/regressionChecks/coreChecks.cpp \
	$(top_srcdir)/src/apps/regressionChecks/basicChecks.cpp \
	$(top_srcdir)/src/apps/regressionChecks/analysisChecks.cpp 

regressionChecks_LDADD = $(top_builddir)/lib/release/libmarsyas.a \
	       $(Libs)

record_SOURCES = $(top_srcdir)/src/apps/record/record.cpp
record_LDADD = $(top_builddir)/lib/release/libmarsyas.a \
	       $(Libs)

sfinfo_SOURCES  = $(top_srcdir)/src/apps/sfinfo/sfinfo.cpp
sfinfo_LDADD = $(top_builddir)/lib/release/libmarsyas.a \
	       $(Libs)

peakSynth_SOURCES  = $(top_srcdir)/src/apps/peakSynth/peakSynth.cpp
peakSynth_LDADD = $(top_builddir)/lib/release/libmarsyas.a \
	       $(Libs)

peakClustering_SOURCES  = $(top_srcdir)/src/apps/peakClustering/peakClustering.cpp
peakClustering_LDADD = $(top_builddir)/lib/release/libmarsyas.a \
	       $(Libs)

classifyFile_SOURCES = $(top_srcdir)/src/apps/classifyFile/classifyFile.cpp
classifyFile_LDADD = $(top_builddir)/lib/release/libmarsyas.a \
	       $(Libs)

boomchick_SOURCES = $(top_srcdir)/src/apps/boomchick/boomchick.cpp
boomchick_LDADD = $(top_builddir)/lib/release/libmarsyas.a \
	       $(Libs)

pitchextract_SOURCES = $(top_srcdir)/src/apps/pitchextract/pitchextract.cpp
pitchextract_LDADD = $(top_builddir)/lib/release/libmarsyas.a \
	       $(Libs)

extract_SOURCES = $(top_srcdir)/src/apps/extract/extract.cpp
extract_LDADD = $(top_builddir)/lib/release/libmarsyas.a \
	       $(Libs)

midiTest_SOURCES = $(top_srcdir)/src/apps/midiTest/midiTest.cpp
midiTest_LDADD =  $(top_builddir)/lib/release/libmarsyas.a \
	       $(Libs)

orcarecord_SOURCES = $(top_srcdir)/src/apps/orcarecord/orcarecord.cpp
orcarecord_LDADD =  $(top_builddir)/lib/release/libmarsyas.a \
	       $(Libs)

virtualsensor_SOURCES = $(top_srcdir)/src/apps/virtualsensor/virtualsensor.cpp
virtualsensor_LDADD = $(top_builddir)/lib/release/libmarsyas.a \
		    $(Libs)

#helloworld_SOURCES = $(top_srcdir)/examples/helloworld.cpp
#helloworld_LDADD = $(top_builddir)/lib/release/libmarsyas.a \
#		$(Libs)

scheduler_SOURCES = $(top_srcdir)/src/apps/scheduler/scheduler.cpp
scheduler_LDADD = $(top_builddir)/lib/release/libmarsyas.a \
		$(Libs)

msl_SOURCES = $(top_srcdir)/src/apps/msl/msl.cpp
msl_LDADD = $(top_builddir)/lib/release/libmarsyas.a \
		$(Libs) $(MslLibs)

kea_SOURCES = $(top_srcdir)/src/apps/kea/kea.cpp
kea_LDADD = $(top_builddir)/lib/release/libmarsyas.a \
		$(Libs) 

#gettingdata_SOURCES = $(top_srcdir)/examples/gettingdata.cpp
#gettingdata_LDADD = $(top_builddir)/lib/release/libmarsyas.a \
#		    $(Libs)

