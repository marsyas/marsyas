# Makefile.am -- Process this file with automake to produce Makefile.in
## Copyright (C) 2002 George Tzanetakis
#
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

auxdir                  = @ac_aux_dir@

AUX_DIST                = $(auxdir)/install-sh $(auxdir)/missing \
                        $(auxdir)/mkinstalldirs $(auxdir)/config.sub \
			$(auxdir)/config.guess $(auxdir)/ltconfig \
			$(auxdir)/ltmain.sh
EXTRA_DIST              = bootstrap TODO marsyasAll.pro marsyasConfig.pri

AUTOMAKE_OPTIONS        = foreign
EXTRA_DIST += .qmake.cache

SUBDIRS	     = src lib bin doc regressionTests misc scripts swig
# we must now explicitly define subdirs to be included in the dist
DIST_SUBDIRS = src lib bin doc regressionTests misc scripts swig

EXTRA_DIST += swig
EXTRA_DIST += otherlibs
#  There's no point writing Makefiles for temporary holding areas.
EXTRA_DIST += ToDeprecate

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 configure
MAINTAINERCLEANFILES += src/marsyas/config-h.in src/marsyas/stamp-h.in 

ACLOCAL                 = aclocal -I $(auxdir)


# disable distcleancheck
distcleancheck:
	@:



## dist-hook:
##	(cd $(distdir) && mkdir $(ac_aux_dir))
##	for file in $(AUX_DIST) $(AUX_DIST_EXTRA); do \
##	cp $$file $(distdir)/$$file; \
## done


