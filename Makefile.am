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
EXTRA_DIST              = bootstrap TODO 

AUTOMAKE_OPTIONS        = foreign


SUBDIRS			= marsyas src $(DISTRIBUTED) doc marsyasVisualStudio2003 marsyasVisualStudio2005 marsyasMATLAB qt4GUIs $(MRSSWIG) 
 
# we must now explicitly define subdirs to be included in the dist
DIST_SUBDIRS		= marsyas src distributed doc marsyasVisualStudio \
			  qt4GUIs swig
			  
MAINTAINERCLEANFILES    = Makefile.in aclocal.m4 configure marsyas/config-h.in \
			  marsyas/stamp-h.in 

ACLOCAL                 = aclocal -I $(auxdir)


html: 
	cd doc && $(MAKE) $(AM_MAKEFLAGS) $@

.PHONY: html 


## dist-hook:
##	(cd $(distdir) && mkdir $(ac_aux_dir))
##	for file in $(AUX_DIST) $(AUX_DIST_EXTRA); do \
##	cp $$file $(distdir)/$$file; \
## done


