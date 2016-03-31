import os, sys
current_dir = os.path.dirname(os.path.realpath(__file__))
shiboken_dir = os.path.join(current_dir, '..', 'shiboken')
sys.path.append(shiboken_dir)

import unittest

fabric_dir = os.environ['FABRIC_DIR']

class CanvasTest(unittest.TestCase):
    def test_main(self):
        # [andrew 20160330] FE-6364
        if sys.version_info < (2, 7):
            return

        from canvas import FabricStyle
        from CanvasWindow import CanvasWindow
        from PySide import QtCore, QtGui
        from PySide.QtTest import QTest

        app = QtGui.QApplication([])
        app.setOrganizationName('Fabric Software Inc')
        app.setApplicationName('Fabric Canvas Standalone')
        app.setStyle( FabricStyle() )

        settings = QtCore.QSettings()
        unguarded = False
        main_win = CanvasWindow(fabric_dir, settings, unguarded)
        main_win.show()
        QTest.qWaitForWindowShown(main_win)

        main_win.loadGraph(os.path.join(fabric_dir, 'Samples', 'Canvas', 'InlineDrawing', 'inlinedrawing_teapot.canvas'))

        dfg_controller = main_win.dfgWidget.getDFGController()
        self.assertEqual(dfg_controller.getExecPath(), '')
        main_win.onNewGraph(skip_save=True)

        # QTest.mousePress(main_win.viewport, QtCore.Qt.LeftButton, pos=QtCore.QPoint(0, 0))
        ## https://bugreports.qt.io/browse/QTBUG-5232
        # QTest.mouseMove(main_win.viewport, pos=QtCore.QPoint(500, 230))
        # QTest.mouseRelease(main_win.viewport, QtCore.Qt.LeftButton, pos=QtCore.QPoint(500, 230))

        # FE-5730
        binding = dfg_controller.getBinding()
        root_exec = binding.getExec()
        i_name = root_exec.addExecPort('i', main_win.client.DFG.PortTypes.In)
        binding.setArgValue(i_name, main_win.client.RT.types.UInt32(5))
        main_win.onNewGraph(skip_save=True)

