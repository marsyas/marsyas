all :
	rm -f *.o
	g++ -Wall -O3 -I./ -I${HOME}/usr/include/marsyas -c -o analyze.o ../analyze.cpp
	g++ -Wall -O3 -I./ -I${HOME}/usr/include/marsyas -c -o pitchCorrect.o pitchCorrect.cpp
	g++ -L${HOME}/usr/lib -o pitchCorrect pitchCorrect.o analyze.o -lmarsyas -framework CoreAudio -framework CoreMidi -framework CoreFoundation
