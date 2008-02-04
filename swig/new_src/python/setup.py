#!/usr/bin/env python

from distutils.core import setup,Extension

import os

def pkg_config ( pkg, cmd ):
	f = os.popen("pkg-config %s %s" % (cmd,pkg))
	s = f.read().strip().split()
	f.close()
	return s

marsyas_module = Extension(
		"_marsyas",
		sources=["marsyas_wrap.cxx"],
		include_dirs=["../../../src"],
		library_dirs=["../../../lib/release","/usr/lib"],
		libraries=["marsyas","stdc++","pthread","c","dl"],
		extra_compile_args = pkg_config("vorbisfile mad","--cflags"),
		extra_link_args = pkg_config("vorbisfile mad","--libs")
		)

setup(
		name="Marsyas",
		ext_modules = [marsyas_module],
		py_modules = ["marsyas"]
)
