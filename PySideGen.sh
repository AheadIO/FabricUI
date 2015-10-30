#!/bin/sh
shiboken \
  PySideGlobal.h \
  --no-suppress-warnings \
  --typesystem-paths=/usr/local/pyside/share/PySide/typesystems/:./shiboken/ \
  --include-paths=/usr/local/pyside/include/PySide/ \
  --enable-pyside-extensions \
  shiboken/fabricui.xml

patch -p0 <./shiboken/fabricui.diff

