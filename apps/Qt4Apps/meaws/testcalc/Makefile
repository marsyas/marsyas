all :
	rm -f *.o
	g++ -Wall -O3 -I./ -I${HOME}/usr/include/marsyas -c -o analyze.o ../analyze.cpp
	g++ -Wall -O3 -I./ -I${HOME}/usr/include/marsyas -c -o temp.o testcalc.cpp
	g++ -L${HOME}/usr/lib -o temp temp.o analyze.o -lmarsyas -framework CoreAudio -framework CoreMidi -framework CoreFoundation
