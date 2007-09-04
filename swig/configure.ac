AC_INIT([marsyas-swig],[0.4.0],[carlosc@uvic.ca])
AM_INIT_AUTOMAKE

AM_CONFIG_HEADER([config.h])

AC_PROG_CXX

AC_DISABLE_STATIC
AC_PROG_LIBTOOL

AC_ARG_WITH([mad],
	    [AS_HELP_STRING([--with-mad],[Links agains libmad])],
	    [MAD_LIBS="-lmad"],
	    [MAD_LIBS=""])

# ALSA_LIBS="-lpthread -lm -ldl -lasound"
ALSA_LIBS=""

AC_ARG_ENABLE([bindings],
	      [AS_HELP_STRING([--enable-bindings],[Use containing folder as Marsyas source])],
	      [MRS_LIBS="$MAD_LIBS $ALSA_LIBS -L\$(top_builddir)/../marsyas -lmarsyas -Framework CorAudio -Framework CoreFoundation -Framework CoreMidi"
	       MRS_INCS="-I\$(top_srcdir)/.. -I\$(top_builddir)/.."
	      ],
	      [MRS_LIBS="$MAD_LIBS $ALSA_LIBS -lmarsyas"
	       MRS_INCS=
	      ])

AC_SUBST([MRS_LIBS])
AC_SUBST([MRS_INCS])

CPPFLAGS="$CPPFLAGS -I${includedir}"
LDFLAGS="$LDFLAGS -L${libdir}"

AC_PATH_PROG([SWIG],[swig])
AM_CONDITIONAL([SWIG],[test -x $SWIG])

AC_DEFUN([MRS_PROG_RUBY],
	 [AC_ARG_WITH([ruby], [AS_HELP_STRING([--with-ruby],[Creates Ruby Module.])])
	 function get_ruby_var
	 {
		 echo "puts Config::CONFIG[['${1}']]" | $RUBY -r rbconfig
	 }
	 if test "x$with_ruby" == "xyes" ; then 
		 AM_CONDITIONAL([RUBY],[true])
		 AC_PATH_PROGS([RUBY],[ruby],[AC_MSG_ERROR([You need Ruby to create Ruby module])])
		 AC_SUBST([SWIG_SUBDIRS],["$SWIG_SUBDIRS ruby"])
		 AC_MSG_CHECKING([for ruby site dir])
		 AC_SUBST([RUBY_SITE],['${libdir}/ruby/site_ruby'])
		 AC_MSG_RESULT([${RUBY_SITE}])
		 AC_MSG_CHECKING([for where .rb files go])
		 AC_SUBST([RUBY_SITE_LIB],["${RUBY_SITE}/`get_ruby_var ruby_version`"])
		 AC_MSG_RESULT([${RUBY_SITE_LIB}])
		 AC_MSG_CHECKING([for where .so files go])
		 AC_SUBST([RUBY_SITE_ARCH],["${RUBY_SITE_LIB}/`get_ruby_var sitearch`"])
		 AC_MSG_RESULT([${RUBY_SITE_ARCH}])
		 AC_MSG_CHECKING([for how to use ruby headers])
		 AC_SUBST([RUBY_INCLUDE],["-I`get_ruby_var archdir`"])
		 AC_MSG_RESULT([${RUBY_INCLUDE}])
	 else
		 AM_CONDITIONAL([RUBY],[false])
	 fi
	 ])

AC_DEFUN([MRS_PROG_PYTHON],
	 [AC_ARG_WITH([python],
		      [AS_HELP_STRING([--with-python],[Creates Python Module.])],
		      [AM_PATH_PYTHON
		       AC_SUBST([SWIG_SUBDIRS],["$SWIG_SUBDIRS python"])
		       AC_MSG_CHECKING([for python's include directory])
		       pyincdir=-I`$PYTHON <<END
import distutils.sysconfig 
print distutils.sysconfig.get_python_inc()
END`
		       AC_MSG_RESULT([$pyincdir])
		       AC_SUBST([PYTHON_INCLUDE],["$pyincdir"])
		       AM_CONDITIONAL([PYTHON],[true])
		       ],
		      [AM_CONDITIONAL([PYTHON],[false])])
	 ])

MRS_PROG_RUBY
MRS_PROG_PYTHON

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
