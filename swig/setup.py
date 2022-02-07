#!/usr/bin/env python

"""
setup.py file for SWIG mapper
"""

from distutils.core import setup, Extension

LO_LIBS = "-L/usr/local/Cellar/liblo/0.31/lib -llo"
LO_LDDIRS = [x[2:] for x in LO_LIBS.split() if x[0:2]=='-L']
LO_LDLIBS = [x[2:] for x in LO_LIBS.split() if x[0:2]=='-l']

LO_CFLAGS = "-I/usr/local/Cellar/liblo/0.31/include"
LO_INCDIRS = [x[2:] for x in LO_CFLAGS.split() if x[0:2]=='-I']

EXTRA_LIBS = []

mapper_module = Extension('_libmapper',
                          sources=['mapper_wrap.c'],
                          include_dirs=['../src',
                                        '../src',
                                        '../include']+LO_INCDIRS,
                          library_dirs=['../src/.libs']+LO_LDDIRS,
                          libraries=['mapper'] + LO_LDLIBS + EXTRA_LIBS,
                          )

setup (name = 'libmapper',
       version = '2.1',
       author      = "libmapper.org",
       author_email = "dot_mapper@googlegroups.com",
       url = "http://libmapper.org",
       description = """A library for mapping controllers and synthesizers.""",
       license = "GNU LGPL version 2.1 or later",
       ext_modules = [mapper_module],
       py_modules = ["libmapper"],
       )
