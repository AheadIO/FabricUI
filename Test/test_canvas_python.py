import platform
import sys
import unittest

class CanvasTest(unittest.TestCase):
    def test_all(self):
        # [andrew 20160330] FE-6364
        if sys.version_info < (2, 7) or platform.system() == 'Darwin':
            return

        from FabricEngine.Canvas.FabricStyle import FabricStyle
        from FabricEngine.Canvas.CanvasWindow import CanvasWindow
        from PySide import QtCore, QtGui
        from PySide.QtTest import QTest

        app = QtGui.QApplication([])
        app.setOrganizationName('Fabric Software Inc')
        app.setApplicationName('Fabric Canvas Standalone')
        app.setStyle( FabricStyle() )

        settings = QtCore.QSettings()
        unguarded = False
        noopt = True
        main_win = CanvasWindow(settings, unguarded, noopt)
        main_win.show()

        # https://doc.qt.io/qt-4.8/qttest-module.html
        QTest.qWaitForWindowShown(main_win)

        # FE-5730
        dfg_controller = main_win.dfgWidget.getDFGController()
        binding = dfg_controller.getBinding()
        root_exec = binding.getExec()
        i_name = root_exec.addExecPort('i', main_win.client.DFG.PortTypes.In)
        binding.setArgValue(i_name, main_win.client.RT.types.UInt32(5))
        main_win.onNewGraph(skip_save=True)

        # FE-6338
        dfg_controller = main_win.dfgWidget.getDFGController()
        binding = dfg_controller.getBinding()
        main_win.dfguiCommandHandler.dfgDoSetArgValue(binding, 'not_exist',
                main_win.client.RT.types.UInt32(5))
        main_win.onNewGraph(skip_save=True)
