#!/bin/sh
shiboken \
  PySideGlobal.h \
  --no-suppress-warnings \
  --typesystem-paths=/usr/local/pyside/share/PySide/typesystems/:./shiboken/ \
  --include-paths=/usr/local/pyside/include/PySide/ \
  shiboken/fabricui.xml

# [andrew 20151023] shiboken not detecting this as an abstract class
sed -i '697d' ./out/FabricUI/fabricui_dfg_dfguicmdhandler_wrapper.cpp
