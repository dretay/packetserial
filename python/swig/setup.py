#!/usr/bin/env python

"""
setup.py file for SWIG example
"""

from distutils.core import setup, Extension


my_modules = Extension('_PacketDAO',
                           sources=['PacketDAO_wrap.c', 'PacketDAO.c'],
                           include_dirs=['../../release/include','../../release/src'],
                           )

setup (name = 'PacketDAO',
       version = '0.1',
       author      = "NA",
       description = """Simple swig dao""",
       ext_modules = [my_modules],
       py_modules = ["PacketDAO"],
       )
