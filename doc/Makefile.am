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


TEXI2HTML = makeinfo --html
TEXI2PDF  = texi2pdf
info_TEXINFOS   = marsyas.texi 

#SUBDIRS = marsyas images
EXTRA_DIST = marsyas.pdf intro.texi tools.texi architecture.texi programming.texi install.texi unsorted.texi interoperability.texi
EXTRA_DIST += macros.itexi README
EXTRA_DIST += images/*.eps images/*.pdf images/*.txt images/*.png
EXTRA_DIST += marsyas/*.html marsyas/images/*.png

CLEANFILES = marsyas.acs  marsyas.cvs  marsyas.mss  marsyas.prs \
             marsyas.ats  marsyas.evs  marsyas.fns  marsyas.ovs \
             marsyas.tmp  marsyas.toc \
             marsyas*.html 

QT_EXAMPLES = ../examples/Qt4-tutorial/*.h ../examples/Qt4-tutorial/*.cpp
QT_EXAMPLES_AS_PERL = ../examples/Qt4-tutorial/*.pro
EXAMPLES = ../examples/*.cpp

html: marsyas.texi
	$(TEXI2HTML) $(TEXI2HTML_FLAGS) $(srcdir)/marsyas.texi 
	mkdir -p marsyas/images/
	cp images/*.png marsyas/images/

pdf: marsyas.texi
	$(TEXI2PDF) $(TEXI2PDF_FLAGS) $(srcdir)/marsyas.texi

sources:
	source-highlight --output-dir=marsyas/ -f html $(QT_EXAMPLES) $(EXAMPLES)
	source-highlight --output-dir=marsyas/ -f texinfo $(QT_EXAMPLES) $(EXAMPLES)
	source-highlight --output-dir=marsyas/ -f html --src-lang=perl $(QT_EXAMPLES_AS_PERL)
	source-highlight --output-dir=marsyas/ -f texinfo --src-lang=perl $(QT_EXAMPLES_AS_PERL)

.PHONY: html pdf

