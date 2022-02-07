#!/usr/bin/env python

"""
setup.py file for python mapper
"""

from distutils.core import setup

setup (name = 'libmapper',
       version = '2.1.36+g1a996b0a',
       author      = "libmapper.org",
       author_email = "dot_mapper@googlegroups.com",
       url = "http://libmapper.org",
       description = """A library for mapping controllers and synthesizers.""",
       license = "GNU LGPL version 2.1 or later",
       py_modules = ["libmapper"],
       )
