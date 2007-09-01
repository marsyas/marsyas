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


# temporary ugly hack to deal with multiple manuals.  I really hope we
# move to waf soon.  :/
#     (not that automake can't do this; I just don't want to spend
#     enough time to figure out how to do it, since we'll probably
#     change to waf in 2008)
TEXI2HTML = makeinfo --html  # -o out-www/manual/
TEXI2PDF  = texi2pdf  #  -o out-www/
info_TEXINFOS   = marsyas-user.texi marsyas-devel.texi

EXTRA_DIST = marsyas.pdf intro.texi tools.texi architecture.texi programming.texi install.texi unsorted.texi interoperability.texi

EXTRA_DIST += macros.itexi README
EXTRA_DIST += images/*.eps images/*.pdf images/*.txt images/*.png
EXTRA_DIST += source-doc/*.texinfo source-doc/*.html

CLEANFILES = marsyas.acs  marsyas.cvs  marsyas.mss  marsyas.prs \
             marsyas.ats  marsyas.evs  marsyas.fns  marsyas.ovs \
             marsyas.tmp  marsyas.toc  marsyas.info marsyas.cps 

#clean-local:
#	-rm -rf out-www

QT_EXAMPLES = ../examples/Qt4-tutorial/*.h ../examples/Qt4-tutorial/*.cpp
QT_EXAMPLES_AS_PERL = ../examples/Qt4-tutorial/*.pro

EXAMPLES = ../examples/*.cpp

#DIST_COMMON = sources pdf html

sources:
	mkdir -p source-doc/
	source-highlight --output-dir=source-doc/ -f html $(QT_EXAMPLES) $(EXAMPLES)
	source-highlight --output-dir=source-doc/ -f texinfo $(QT_EXAMPLES) $(EXAMPLES)
	source-highlight --output-dir=source-doc/ -f html --src-lang=perl $(QT_EXAMPLES_AS_PERL)
	source-highlight --output-dir=source-doc/ -f texinfo --src-lang=perl $(QT_EXAMPLES_AS_PERL)

doxy:
	mkdir -p out-www/doxy/html/
# all on one line to stay in that directory
	cd ../marsyas/ ; doxygen
# now I'm automagically back in  doc/
	mkdir -p out-www/sourceDoc/
	mv out-www/doxy/html/* out-www/sourceDoc/
	rmdir out-www/doxy/html
	rmdir out-www/doxy/

# ugly ugly ugly.  :(
html: marsyas-user.texi marsyas-devel.texi
	mkdir -p out-www/manual/
	mkdir -p out-www/manual/examples/
	cp source-doc/*.html out-www/manual/examples/
	$(TEXI2HTML) $(TEXI2HTML_FLAGS) $(srcdir)/marsyas-user.texi -o out-www/manual/marsyas-user/
	$(TEXI2HTML) $(TEXI2HTML_FLAGS) $(srcdir)/marsyas-devel.texi -o out-www/manual/marsyas-devel/
	mkdir -p out-www/manual/images/
	cp images/*.png out-www/manual/images/
	cd out-www/manual/marsyas-user/ ; ln -s ../images/ images
	cd out-www/manual/marsyas-devel/ ; ln -s ../images/ images
	cp index.html.in out-www/index.html

pdf: marsyas-user.texi marsyas-devel.texi
	mkdir -p out-www/
	$(TEXI2PDF) $(TEXI2PDF_FLAGS) $(srcdir)/marsyas-user.texi -o out-www/manual/
	$(TEXI2PDF) $(TEXI2PDF_FLAGS) $(srcdir)/marsyas-devel.texi -o out-www/manual/

.PHONY: html pdf

