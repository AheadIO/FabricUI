#!/usr/bin/env python

"""Alembic Viewer launcher script."""

import os
import sys

if sys.version_info < (2, 7):
    raise Exception('alembic_viewer.py currently requires Python 2.7')

from PySide import QtGui

from FabricEngine.Canvas.FabricStyle import FabricStyle
from FabricEngine.Canvas.FabricParser import FabricParser

from AlembicViewer.AlembicViewerWindow import AlembicViewerWindow


if __name__ == "__main__":
    # This only runs when launched directly from the command line.
    # A QApplication is setup and the Alembic Viewer Window is instanced and
    # attached to the QApplication and shown.
    #
    # Optional command line arguments for the initial directory are also
    # available to be called on startup.

    app = QtGui.QApplication([])
    app.setOrganizationName('Fabric Software Inc')
    app.setApplicationName('Alembic Viewer')
    app.setApplicationVersion('1.0.0')
    app.setStyle( FabricStyle() )

    fabricDir = os.environ.get('FABRIC_DIR', None)
    if fabricDir:
        logoPath = os.path.join(fabricDir, 'Resources', 'fe_logo.png')
        app.setWindowIcon(QtGui.QIcon(logoPath))

    parser = FabricParser()

    parser.add_argument('-d', '--initDir',
                        action='store',
                        dest='initDir',
                        help='initial directory to open')

    args = parser.parse_args()

    initDir = args.initDir

    mainWin = AlembicViewerWindow(initDir=initDir)
    mainWin.show()

    alembicViewerGraphPath = os.path.join(os.getcwd(), 'AlembicViewer.canvas')
    mainWin.loadGraph(alembicViewerGraphPath)

    app.exec_()
