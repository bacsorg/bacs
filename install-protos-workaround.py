#!/usr/bin/python3

import os
from os.path import join, relpath
import shutil
import sys


def main():
  source = os.getenv('MESON_SOURCE_ROOT')
  build = os.getenv('MESON_BUILD_ROOT')
  prefix = os.getenv('MESON_INSTALL_PREFIX')
  destdir_prefix = os.getenv('MESON_INSTALL_DESTDIR_PREFIX')
  for lib in sys.argv[1:]:
    libroot = join(build, lib + '@sta')
    for dirpath, dirnames, filenames in os.walk(libroot):
      for f in filenames:
        if f.endswith('.pb.h'):
          directory = relpath(dirpath, start=libroot)
          dst_dir = join(destdir_prefix, 'include', directory)
          os.makedirs(dst_dir, exist_ok=True)
          src_file = join(dirpath, f)
          dst_file = join(dst_dir, f)
          print('Installing', src_file, 'to', dst_file)
          shutil.copyfile(src_file, dst_file)
  return 0


if __name__ == '__main__':
  sys.exit(main())
