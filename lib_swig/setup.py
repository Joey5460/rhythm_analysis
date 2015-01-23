#!/usr/bin/env python
"""
setup.py file for SWIG bf_math 
"""

from distutils.core import setup, Extension


bf_math_module = Extension('_bf_math',
                           include_dirs = ['../vt_detect/bf_lib/'],
                           sources = ['bf_math_wrap.c', '../vt_detect/bf_lib/bf_math.c'],
                           )

setup (name = 'bf_math',
       version = '0.1',
       author      = "SWIG Docs",
       description = """bf_math swig from docs""",
       ext_modules = [bf_math_module],
       py_modules = ["bf_math"],
       )
