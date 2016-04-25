"""Alembic Viewer launcher script."""

import optparse
import os

from PySide import QtGui

from FabricEngine.Canvas.FabricStyle import FabricStyle

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

    opt_parser = optparse.OptionParser(usage='Usage: %prog [options] [graph]')
    opt_parser.add_option('-d', '--initDir',
                          action='store',
                          dest='initDir',
                          help='Initial directory to load alembic files from.')

    (opts, args) = opt_parser.parse_args()

    initDir = opts.initDir

    mainWin = AlembicViewerWindow(initDir=initDir)
    mainWin.show()

    alembicViewerGraphPath = os.path.join(os.getcwd(), 'AlembicViewer.canvas')
    mainWin.loadGraph(alembicViewerGraphPath)

    app.exec_()