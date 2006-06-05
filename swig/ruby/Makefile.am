rubysitelibdir = @RUBY_SITE_LIB@
rubysitearchdir = @RUBY_SITE_ARCH@

rubysitelib_DATA = marsyas.rb
rubysitearch_LTLIBRARIES = marsyas_ruby.la

marsyas_ruby_la_SOURCES = marsyas-ruby_wrap.cxx
marsyas_ruby_la_LDFLAGS = -module -avoid-version

AM_CPPFLAGS = @RUBY_INCLUDE@ @MRS_INCS@
AM_LDFLAGS = @MRS_LIBS@

EXTRA_DIST = marsyas-ruby.i marsyas.rb test.rb

if SWIG

%_wrap.cxx : %.i
	$(SWIG) -c++ -ruby -O $<

endif
