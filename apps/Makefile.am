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


SUBDIRS = sfplay sfinfo
# isn't working.  :(   -gp
#SUBDIRS += regressionChecks

EXTRA_DIST = apps.pro
EXTRA_DIST += consoleApps.pri
EXTRA_DIST += ERBTest/
EXTRA_DIST += MarEditor/
EXTRA_DIST += Qt4Apps/
EXTRA_DIST += SeneffEarTest/
EXTRA_DIST += backup/

# I wish there was a better way of doing this.  :(
EXTRA_DIST += regressionChecks/
EXTRA_DIST += bextract/
EXTRA_DIST += boomchick/
EXTRA_DIST += classifyFile/
EXTRA_DIST += extract/
EXTRA_DIST += mudbox/
EXTRA_DIST += midiTest/
EXTRA_DIST += mixer/
EXTRA_DIST += mkcollection/
EXTRA_DIST += msl/
EXTRA_DIST += muggle/
EXTRA_DIST += orcarecord/
EXTRA_DIST += peakEval/
EXTRA_DIST += peakSynth/
EXTRA_DIST += peakClustering/
EXTRA_DIST += phasevocoder/
EXTRA_DIST += pitchextract/
EXTRA_DIST += record/
EXTRA_DIST += scheduler/
EXTRA_DIST += server_main/
EXTRA_DIST += sfplaygui/
EXTRA_DIST += sfplugin/
EXTRA_DIST += speakerSeg/
EXTRA_DIST += tempo/
EXTRA_DIST += textract/
EXTRA_DIST += tutorial/
EXTRA_DIST += virtualsensor/
EXTRA_DIST += vocalEffort/
EXTRA_DIST += WHaSp/

dist-hook:
	rm -rf `find $(distdir)/regressionChecks/ -name .svn`
	rm -rf `find $(distdir)/ERBTest/ -name .svn`
	rm -rf `find $(distdir)/MarEditor/ -name .svn`
	rm -rf `find $(distdir)/Qt4Apps/ -name .svn`
	rm -rf `find $(distdir)/SeneffEarTest/ -name .svn`
	rm -rf `find $(distdir)/backup/ -name .svn`
	rm -rf `find $(distdir)/bextract/ -name .svn`
	rm -rf `find $(distdir)/boomchick/ -name .svn`
	rm -rf `find $(distdir)/classifyFile/ -name .svn`
	rm -rf `find $(distdir)/extract/ -name .svn`
	rm -rf `find $(distdir)/mudbox/ -name .svn`
	rm -rf `find $(distdir)/midiTest/ -name .svn`
	rm -rf `find $(distdir)/mixer/ -name .svn`
	rm -rf `find $(distdir)/mkcollection/ -name .svn`
	rm -rf `find $(distdir)/msl/ -name .svn`
	rm -rf `find $(distdir)/muggle/ -name .svn`
	rm -rf `find $(distdir)/orcarecord/ -name .svn`
	rm -rf `find $(distdir)/peakSynth/ -name .svn`
	rm -rf `find $(distdir)/peakClustering/ -name .svn`
	rm -rf `find $(distdir)/phasevocoder/ -name .svn`
	rm -rf `find $(distdir)/pitchextract/ -name .svn`
	rm -rf `find $(distdir)/record/ -name .svn`
	rm -rf `find $(distdir)/scheduler/ -name .svn`
	rm -rf `find $(distdir)/server_main/ -name .svn`
	rm -rf `find $(distdir)/sfinfo/ -name .svn`
	rm -rf `find $(distdir)/sfplaygui/ -name .svn`
	rm -rf `find $(distdir)/sfplugin/ -name .svn`
	rm -rf `find $(distdir)/speakerSeg/ -name .svn`
	rm -rf `find $(distdir)/tempo/ -name .svn`
	rm -rf `find $(distdir)/textract/ -name .svn`
	rm -rf `find $(distdir)/tutorial/ -name .svn`
	rm -rf `find $(distdir)/virtualsensor/ -name .svn`
	rm -rf `find $(distdir)/vocalEffort/ -name .svn`
	rm -rf `find $(distdir)/WHaSp/ -name .svn`
	rm -rf `find $(distdir)/peakEval/ -name .svn`
	rm -rf `find $(distdir)/peakSynth/ -name .svn`

