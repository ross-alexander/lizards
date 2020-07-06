#!/usr/bin/env python

"""
setup.py file for SWIG example
"""

from distutils.core import setup, Extension


point_module = Extension('_point_t',
                           sources=['point_wrap.cxx', 'point.cc'],
			   include_dirs=['/usr/include/libxml++-2.6','/usr/lib/libxml++-2.6/include','/usr/include/libxml2', '/usr/include/glibmm-2.4', '/usr/lib/glibmm-2.4/include', '/usr/include/sigc++-2.0','/usr/lib/sigc++-2.0/include','/usr/include/glib-2.0', '/usr/lib/glib-2.0/include']
                           )

setup (name = 'example',
       version = '0.1',
       author      = "SWIG Docs",
       description = """Simple swig example from docs""",
       ext_modules = [point_module],
       py_modules = ["point_t"],
       )


