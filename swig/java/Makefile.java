all : libmarsyas.so 

clean :
	rm -rf libmarsyas.so *.class edu/uvic/marsyas/*.class

libmarsyas.so : marsyas_wrap.cxx edu/uvic/marsyas/marsyas.java
	${LINK.o} -fPIC -O3 -shared -o $@ $< -I../../../src -L../../../lib/release -lmarsyas -lstdc++ `pkg-config --cflags --libs vorbisfile mad` -lpthread -lc -lm

marsyas_wrap.cxx edu/uvic/marsyas/marsyas.java : marsyas.i
	mkdir -p edu/uvic/marsyas
	swig -c++ -java -package edu.uvic.marsyas -outdir edu/uvic/marsyas $< 
	javac edu/uvic/marsyas/*.java Test.java
