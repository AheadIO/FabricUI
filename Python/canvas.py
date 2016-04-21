#!/usr/bin/env python

"""Canvas launcher script."""

import optparse
import os
import sys

if sys.version_info < (2, 7):
    raise Exception('canvas.py currently requires Python 2.7')

from PySide import QtCore, QtGui
from FabricEngine.Canvas.CanvasWindow import CanvasWindow
from FabricEngine.Canvas.FabricStyle import FabricStyle

if __name__ == "__main__":
    # This only runs when launched directly from the command line.
    # A QApplication is setup and the Canvas Window is instanced and attached to
    # the QApplication and shown.
    #
    # Optional command line arguments for unguarded mode, Python code to
    # execute, and a Python script to execute are also available to be called on
    # startup.

    app = QtGui.QApplication([])
    app.setOrganizationName('Fabric Software Inc')
    app.setApplicationName('Fabric Canvas Standalone')
    app.setApplicationVersion('2.0.0')
    app.setStyle( FabricStyle() )

    fabricDir = os.environ.get('FABRIC_DIR', None)
    if fabricDir:
        logoPath = os.path.join(fabricDir, 'Resources', 'fe_logo.png')
        app.setWindowIcon(QtGui.QIcon(logoPath))

    opt_parser = optparse.OptionParser(usage='Usage: %prog [options] [graph]')
    opt_parser.add_option('-u', '--unguarded',
                          action='store_true',
                          dest='unguarded',
                          help='compile KL code in unguarded mode')
    opt_parser.add_option('-n', '--noopt',
                          action='store_true',
                          dest='noopt',
                          help='compile KL code wihout brackground optimization')
    opt_parser.add_option('-e', '--exec',
                          action='store',
                          dest='exec_',
                          help='Python code to execute on startup')
    opt_parser.add_option('-s', '--script',
                          action='store',
                          dest='script',
                          help='Python script file to execute on startup')
    (opts, args) = opt_parser.parse_args()

    unguarded = opts.unguarded is True
    noopt = opts.noopt is True

    settings = QtCore.QSettings()
    mainWin = CanvasWindow(settings, unguarded, noopt)
    mainWin.show()

    for arg in args:
        mainWin.loadGraph(arg)

    if opts.exec_:
        mainWin.scriptEditor.exec_(opts.exec_)

    if opts.script:
        with open(opts.script, "r") as f:
            mainWin.scriptEditor.exec_(f.read())

    app.exec_()
