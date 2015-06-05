#!/bin/sh
shiboken \
  PySideGlobal.h \
  --typesystem-paths=/usr/local/Cellar/pyside/1.2.2/share/PySide/typesystems/ \
  --include-paths=/usr/local/Cellar/pyside/1.2.2/include/PySide/ \
  PySideTypesystem.xml
