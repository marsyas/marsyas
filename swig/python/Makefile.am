dist_python_DATA = marsyas_python.py
python_LTLIBRARIES = _marsyas_python.la

_marsyas_python_la_SOURCES = marsyas-python_wrap.cxx
_marsyas_python_la_LDFLAGS = -module -avoid-version

AM_CPPFLAGS = @MRS_INCS@ @PYTHON_INCLUDE@
AM_LDFLAGS = @MRS_LIBS@

EXTRA_DIST = marsyas-python.i
if SWIG

%_wrap.cxx : %.i
	        $(SWIG) -c++ -python -O $<

endif

