############ PARTIAL MAKEFILE FOR PNGWRITER ######################################

#       Website: Main:             http://pngwriter.sourceforge.net/
#       Author:                    Paul Blackburn
#       Email:                     individual61@users.sourceforge.net
#       Version:                   0.5.3   (24 / I / 2005)
#       License:                   GNU General Public License
#                                  © 2002, 2003, 2004, 2005 Paul Blackburn
#
##################################################################################

FT_ARG = -DNO_FREETYPE
CXX=g++
CXXFLAGS= -O3 -Wall -Wno-deprecated $(FT_ARG)
INC=  -I../src/ -I$(PREFIX)/include/
LIBS= -L../src -L$(PREFIX)/lib/ -lz -lpng -lpngwriter
INSTALL=install
SELF=make.include.linux
OBJECTS=pngwriter.o

all: libpngwriter.a

libpngwriter.a: $(OBJECTS)
	ar rv $@ $^
	ranlib $@

pngwriter.o: pngwriter.cc pngwriter.h
	$(CXX) $(CXXFLAGS) $(INC) -c -o pngwriter.o pngwriter.cc

clean	:    
	rm -f $(OBJECTS) libpngwriter.a


