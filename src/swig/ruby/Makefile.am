EXTRA_DIST = extconf.rb.in marsyas.i marsyas_wrap.cxx setup.sh.in test.rb
DISTCLEANFILES = Makefile.ruby setup.sh extconf.rb

all install clean : Makefile.ruby
	-make -f Makefile.ruby $@

Makefile.ruby : setup.sh
	-$(SHELL) setup.sh
