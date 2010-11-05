#!/usr/bin/env python3

from distutils.core import setup, Extension

xroot = Extension('xroot',
                  sources = ['xroot.c'],
                  define_macros = [
                    # ('DEBUG', '1'),
                    # ('XROOT_EXTENDED_IMAGE_PLACEMENT', '1')
                    # ('XROOT_EXTENDED_ALPHA', '1')
                    ],
                  # include_dirs = ['/usr/local/include'],
                  libraries = ['Imlib2', 'X11'],
                  # library_dirs = ['/usr/local/lib'],
                  )

setup(name = 'xroot',
      version = '0.0.2',
      description = 'wset is a tool which allows you to compose wallpapers for X',
      author = 'Ni@m',
      author_email = 'niam[dot]niam[at]gmail[dot]com',
      url = 'http://github.com/niamster/wset',
      ext_modules = [xroot],
      scripts = ['wset', 'wset-gtk'],
      data_files = [
        ('share/applications', ['wset.desktop'])
        ],
      )
