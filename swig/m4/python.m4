
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
