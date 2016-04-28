#!/usr/bin/env python

"""Canvas launcher script."""

import os
import sys

if sys.version_info < (2, 7):
    raise Exception('canvas.py currently requires Python 2.7')

from PySide import QtCore, QtGui
from FabricEngine.Canvas.FabricApplication import FabricApplication
from FabricEngine.Canvas.CanvasWindow import CanvasWindow
from FabricEngine.Canvas.FabricParser import FabricParser
from FabricEngine.Canvas.FabricParser import CheckExtension

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

    fabricDir = os.environ.get('FABRIC_DIR', None)
    if fabricDir:
        logoPath = os.path.join(fabricDir, 'Resources', 'fe_logo.png')
        app.setWindowIcon(QtGui.QIcon(logoPath))

    parser = FabricParser()

    parser.add_argument('graph',
                        nargs='?',
                        action=CheckExtension(['canvas']),
                        help='canvas graph (.canvas) to load on startup')

    parser.add_argument('-u', '--unguarded',
                        action='store_true',
                        help='compile KL code in unguarded mode')

    parser.add_argument('-n', '--noopt',
                        action='store_true',
                        help='compile KL code wihout brackground optimization')

    parser.add_argument('-e', '--exec',
                          action='store',
                          dest='exec_',
                          help='python code to execute on startup')

    parser.add_argument('-s', '--script',
                          action='store',
                          dest='script',
                          help='python script file to execute on startup')


    args = parser.parse_args()
 
    unguarded = args.unguarded is True
    noopt = args.noopt is True

    settings = QtCore.QSettings()
    mainWin = CanvasWindow(settings, unguarded, noopt)
    mainWin.show()

    if args.graph:
        mainWin.loadGraph(args.graph)

    if args.exec_:
        mainWin.scriptEditor.exec_(args.exec_)

    if args.script:
        mainWin.scriptEditor.loadScript(args.script)

    app.exec_()
