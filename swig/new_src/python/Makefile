all : _marsyas.so

_marsyas.so : setup.py marsyas_wrap.cxx marsyas.py
	python setup.py build_ext --inplace

%_wrap.c %.py : %.i
	swig -c++ -python $<

clean distclean :
	python setup.py clean -a
	rm -rf _marsyas.so
