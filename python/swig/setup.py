#!/usr/bin/env python

"""
setup.py file for SWIG example
"""

from distutils.core import setup, Extension
from os import listdir
from os.path import isfile, join
release_path='../../release/src/'
release_files = [(lambda f: release_path+f)(f) for f in listdir(release_path) if isfile(join(release_path, f))]

my_modules = Extension('_PacketDAO',
                           sources=['PacketDAO_wrap.c', 'PacketDAO.c']+release_files,
                           include_dirs=['../../release/include'],
                           define_macros=[('PB_FIELD_16BIT',None)],
                           )

setup (name = 'PacketDAO',
       version = '0.1',
       author      = "NA",
       description = """Simple swig dao""",
       ext_modules = [my_modules],
       py_modules = ["PacketDAO"],
       )
