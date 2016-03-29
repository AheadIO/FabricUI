import os, sys
current_dir = os.path.dirname(os.path.realpath(__file__))
shiboken_dir = os.path.join(current_dir, '..', 'shiboken')
sys.path.append(shiboken_dir)

import canvas, time, unittest
from PySide import QtCore, QtGui
from PySide.QtTest import QTest

fabric_dir = os.environ['FABRIC_DIR']

class CanvasTest(unittest.TestCase):
    def test_main(self):
        app = QtGui.QApplication([])
        app.setOrganizationName('Fabric Software Inc')
        app.setApplicationName('Fabric Canvas Standalone')
        app.setStyle( canvas.FabricStyle() )

        settings = QtCore.QSettings()
        unguarded = False
        main_win = canvas.MainWindow(settings, unguarded)
        main_win.show()
        QTest.qWaitForWindowShown(main_win)

        main_win.loadGraph(os.path.join(fabric_dir, 'Samples', 'Canvas', 'InlineDrawing', 'inlinedrawing_teapot.canvas'))

        dfg_controller = main_win.dfgWidget.getDFGController()
        self.assertEqual(dfg_controller.getExecPath(), '')

        # QTest.mousePress(main_win.viewport, QtCore.Qt.LeftButton, pos=QtCore.QPoint(0, 0))
        ## https://bugreports.qt.io/browse/QTBUG-5232
        # QTest.mouseMove(main_win.viewport, pos=QtCore.QPoint(500, 230))
        # QTest.mouseRelease(main_win.viewport, QtCore.Qt.LeftButton, pos=QtCore.QPoint(500, 230))

