all : marsyas.so

clean :
	rm -rf marsyas.so

%.so : %_wrap.cxx
	${LINK.o} -fPIC -O3 -shared -o $@ $< -I../../../src -L../../../lib/release -lmarsyas -lstdc++ `pkg-config --cflags --libs vorbisfile mad` -lpthread -lc -lm

%_wrap.cxx : %.i
	swig -c++ -lua $< 
