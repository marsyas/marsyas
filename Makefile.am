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

SUBDIRS			= marsyas lib apps bin doc
SUBDIRS			+= examples regressionTests
 
# we must now explicitly define subdirs to be included in the dist
DIST_SUBDIRS		= marsyas lib apps bin doc
DIST_SUBDIRS		+= examples regressionTests


#  There's no point writing Makefiles for temporary holding areas.
EXTRA_DIST += ToDeprecate/
EXTRA_DIST += otherlibs/
EXTRA_DIST += scripts/
EXTRA_DIST += swig/
EXTRA_DIST += tools/

dist-hook:
	rm -rf `find $(distdir)/ToDeprecate/ -name .svn`
	rm -rf `find $(distdir)/otherlibs/ -name .svn`
	rm -rf `find $(distdir)/scripts/ -name .svn`
	rm -rf `find $(distdir)/swig/ -name .svn`
	rm -rf `find $(distdir)/tools/ -name .svn`


MAINTAINERCLEANFILES    = Makefile.in aclocal.m4 configure marsyas/config-h.in \
			  marsyas/stamp-h.in 

ACLOCAL                 = aclocal -I $(auxdir)




## dist-hook:
##	(cd $(distdir) && mkdir $(ac_aux_dir))
##	for file in $(AUX_DIST) $(AUX_DIST_EXTRA); do \
##	cp $$file $(distdir)/$$file; \
## done


