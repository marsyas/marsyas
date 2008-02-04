
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
