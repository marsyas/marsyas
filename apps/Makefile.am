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


SUBDIRS = sfplay

EXTRA_DIST = *.pro *.pri
EXTRA_DIST += ERBTest/
EXTRA_DIST += MarEditor/
EXTRA_DIST += Qt4Apps/
EXTRA_DIST += SeneffEarTest/
EXTRA_DIST += backup/

dist-hook:
	rm -rf `find $(distdir)/ERBTest/ -name .svn`
	rm -rf `find $(distdir)/MarEditor/ -name .svn`
	rm -rf `find $(distdir)/Qt4Apps/ -name .svn`
	rm -rf `find $(distdir)/SeneffEarTest/ -name .svn`
	rm -rf `find $(distdir)/backup/ -name .svn`

