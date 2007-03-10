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

INCLUDES = -I$(top_srcdir)/marsyas

bin_PROGRAMS = sfplay phasevocoder sfplugin bextract mkcollection marsyasTests record classifyFile peakClustering sfinfo \
	       extract midiTest boomchick pitchextract orcarecord helloworld gettingdata

sfplay_SOURCES = $(top_srcdir)/apps/sfplay/sfplay.cpp
sfplay_LDADD = $(top_builddir)/lib/release/libmarsyas.a \
	       $(Libs)

phasevocoder_SOURCES = $(top_srcdir)/apps/phasevocoder/phasevocoder.cpp
phasevocoder_LDADD = $(top_builddir)/lib/release/libmarsyas.a \
	       $(Libs)

sfplugin_SOURCES = $(top_srcdir)/apps/sfplugin/sfplugin.cpp
sfplugin_LDADD = $(top_builddir)/lib/release/libmarsyas.a \
	       $(Libs)

bextract_SOURCES = $(top_srcdir)/apps/bextract/bextract.cpp
bextract_LDADD = $(top_builddir)/lib/release/libmarsyas.a \
	       $(Libs)

mkcollection_SOURCES = $(top_srcdir)/apps/mkcollection/mkcollection.cpp
mkcollection_LDADD = $(top_builddir)/lib/release/libmarsyas.a \
	       $(Libs)

marsyasTests_SOURCES = $(top_srcdir)/apps/marsyasTests/marsyasTests.cpp
marsyasTests_LDADD = $(top_builddir)/lib/release/libmarsyas.a \
	       $(Libs)

record_SOURCES = $(top_srcdir)/apps/record/record.cpp
record_LDADD = $(top_builddir)/lib/release/libmarsyas.a \
	       $(Libs)

sfinfo_SOURCES  = $(top_srcdir)/apps/sfinfo/sfinfo.cpp
sfinfo_LDADD = $(top_builddir)/lib/release/libmarsyas.a \
	       $(Libs)

peakClustering_SOURCES  = $(top_srcdir)/apps/peakClustering/peakClustering.cpp
peakClustering_LDADD = $(top_builddir)/lib/release/libmarsyas.a \
	       $(Libs)

classifyFile_SOURCES = $(top_srcdir)/apps/classifyFile/classifyFile.cpp
classifyFile_LDADD = $(top_builddir)/lib/release/libmarsyas.a \
	       $(Libs)

boomchick_SOURCES = $(top_srcdir)/apps/boomchick/boomchick.cpp
boomchick_LDADD = $(top_builddir)/lib/release/libmarsyas.a \
	       $(Libs)

pitchextract_SOURCES = $(top_srcdir)/apps/pitchextract/pitchextract.cpp
pitchextract_LDADD = $(top_builddir)/lib/release/libmarsyas.a \
	       $(Libs)

extract_SOURCES = $(top_srcdir)/apps/extract/extract.cpp
extract_LDADD = $(top_builddir)/lib/release/libmarsyas.a \
	       $(Libs)

midiTest_SOURCES = $(top_srcdir)/apps/midiTest/midiTest.cpp
midiTest_LDADD =  $(top_builddir)/lib/release/libmarsyas.a \
	       $(Libs)

orcarecord_SOURCES = $(top_srcdir)/apps/orcarecord/orcarecord.cpp
orcarecord_LDADD =  $(top_builddir)/lib/release/libmarsyas.a \
	       $(Libs)

helloworld_SOURCES = $(top_srcdir)/examples/helloworld.cpp
helloworld_LDADD = $(top_builddir)/lib/release/libmarsyas.a \ 
		$(Libs)

gettingdata_SOURCES = $(top_srcdir)/examples/gettingdata.cpp
gettingdata_LDADD = $(top_builddir)/lib/release/libmarsyas.a \ 
		    $(Libs)
