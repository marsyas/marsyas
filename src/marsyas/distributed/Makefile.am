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

MAINTAINERCLEANFILES = Makefile.in

INCLUDES        = -I$(top_builddir) -I$(top_builddir)/marsyas \
                -I$(top_srcdir) -I$(top_srcdir)/marsyas 


bin_PROGRAMS    = recvTCP recvUDP recvFFT sendTCP sendUDP sendFFT dispatcher collector

recvTCP_SOURCES     = recvTCP.cpp 
recvTCP_LDADD       = $(top_builddir)/marsyas/libmarsyas.a \
			$(Libs)

recvUDP_SOURCES     = recvUDP.cpp 
recvUDP_LDADD       = $(top_builddir)/marsyas/libmarsyas.a \
			$(Libs)

recvFFT_SOURCES     = recvFFT.cpp 
recvFFT_LDADD       = $(top_builddir)/marsyas/libmarsyas.a \
			$(Libs)

sendTCP_SOURCES     = sendTCP.cpp 
sendTCP_LDADD       = $(top_builddir)/marsyas/libmarsyas.a \
			$(Libs)

sendUDP_SOURCES     = sendUDP.cpp 
sendUDP_LDADD       = $(top_builddir)/marsyas/libmarsyas.a \
			$(Libs)

sendFFT_SOURCES     = sendFFT.cpp 
sendFFT_LDADD       = $(top_builddir)/marsyas/libmarsyas.a \
			$(Libs)

dispatcher_SOURCES  = dispatcher.cpp
dispatcher_LDADD    = $(top_builddir)/marsyas/libmarsyas.a \
			$(Libs) 

collector_SOURCES  = collector.cpp
collector_LDADD    = $(top_builddir)/marsyas/libmarsyas.a \
			$(Libs) 
