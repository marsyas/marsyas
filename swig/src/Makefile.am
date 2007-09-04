AM_LDFLAGS = @MRS_LIBS@
AM_CPPFLAGS = @MRS_INCS@ @RUBY_INCLUDE@ @PYTHON_INCLUDE@

if RUBY

rubysitelibdir = @RUBY_SITE_LIB@
rubysitearchdir = @RUBY_SITE_ARCH@

rubysitelib_DATA = marsyas.rb
rubysitearch_LTLIBRARIES = marsyas_ruby.la

marsyas_ruby_la_SOURCES = marsyas_ruby.cxx
marsyas_ruby_la_LDFLAGS = -framework CoreAudio -framework CoreMidi -framework CoreFoundation -module -avoid-version

endif

if PYTHON
dist_python_DATA = marsyas_python.py
python_LTLIBRARIES = _marsyas_python.la

_marsyas_python_la_SOURCES = marsyas_python.cxx
_marsyas_python_la_LDFLAGS = -framework CoreAudio -framework CoreMidi -framework CoreFoundation -module -avoid-version

endif

EXTRA_DIST = marsyas.i ruby.i python.i marsyas.rb test.rb

if SWIG

marsyas_ruby.cxx : marsyas.i ruby.i
	$(SWIG) -c++ -ruby -O -o $@ $<
	
marsyas_python.cxx : marsyas.i python.i
	$(SWIG) -c++ -python -O -o $@ $<

endif
