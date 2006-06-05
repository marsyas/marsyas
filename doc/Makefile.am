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
## TEXI2PDF  = texi2pdf
info_TEXINFOS   = marsyas.texi
SUBDIRS = marsyas 
EXTRA_DIST = marsyas.pdf marsyasArchitecture1.png marsyasArchitecture1.pdf marsyasArchitecture1.txt marsyasArchitecture1.eps

CLEANFILES = marsyas.acs  marsyas.cvs  marsyas.mss  marsyas.prs \
             marsyas.ats  marsyas.evs  marsyas.fns  marsyas.ovs \
             marsyas.tmp  marsyas.toc \
             marsyas*.html 


html: marsyas.texi
	$(TEXI2HTML) $(TEXI2HTML_FLAGS) $(srcdir)/marsyas.texi

pdf: marsyas.texi
	$(TEXI2PDF) $(TEXI2PDF_FLAGS) $(srcdir)/marsyas.texi

.PHONY: html pdf

