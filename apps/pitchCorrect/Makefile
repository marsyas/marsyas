#  TODO: fix this; make it part of the normal Marsyas build process.

all :
	rm -f *.o
	g++ -Wall -O3 -I./ -I../../marsyas/ -c -o analyze.o analyze.cpp
	g++ -Wall -O3 -I./ -I../../marsyas/ -c -o pitchCorrect.o pitchCorrect.cpp
	g++ -L../../lib/release -o pitchCorrect pitchCorrect.o analyze.o -lmarsyas -framework CoreAudio -framework CoreMidi -framework CoreFoundation
