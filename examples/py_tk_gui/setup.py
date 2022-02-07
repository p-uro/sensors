#!/usr/bin/env python

"""
setup.py file for SWIG pwm
"""

from distutils.core import setup, Extension

top = '..'
pwm_module = Extension('_pwm',
                       sources=['pwm_wrap.cxx'],
                       include_dirs=['../../examples/pwm_synth'],
                       library_dirs=['../../examples/pwm_synth/.libs'],
                       libraries=['pwm'],
                       extra_link_args='-framework CoreAudio -framework CoreFoundation'.split(),
                       )

setup (name = 'pwm',
       version = '2.1',
       author      = "IDMIL",
       description = """Simple PWM audio synth example for libmapper""",
       ext_modules = [pwm_module],
       py_modules = ["pwm"],
       )
