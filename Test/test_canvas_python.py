import platform
import pytest
import sys

# [andrew 20160330] FE-6364
pytestmark = pytest.mark.skipif(sys.version_info < (2, 7) or platform.system() == 'Darwin',
        reason = "no support for Python 2.6 or missing display")

@pytest.fixture(scope="module")
def canvas_win():
    from FabricEngine.FabricUI import Application
    from FabricEngine.Canvas.CanvasWindow import CanvasWindow
    from PySide import QtCore
    from PySide.QtTest import QTest

    global app
    app = Application.FabricApplication()
    app.setOrganizationName('Fabric Software Inc')
    app.setApplicationName('Fabric Canvas Standalone')

    settings = QtCore.QSettings()
    unguarded = False
    noopt = True
    main_win = CanvasWindow(settings, unguarded, noopt)
    main_win.show()

    # https://doc.qt.io/qt-4.8/qttest-module.html
    QTest.qWaitForWindowShown(main_win)

    return main_win

def test_fe5730(canvas_win):
    dfg_controller = canvas_win.dfgWidget.getDFGController()
    binding = dfg_controller.getBinding()
    root_exec = binding.getExec()
    i_name = root_exec.addExecPort('i', canvas_win.client.DFG.PortTypes.In)
    binding.setArgValue(i_name, canvas_win.client.RT.types.UInt32(5))
    canvas_win.onNewGraph(skip_save=True)

def test_fe6338(canvas_win):
    dfg_controller = canvas_win.dfgWidget.getDFGController()
    binding = dfg_controller.getBinding()
    canvas_win.dfguiCommandHandler.dfgDoSetArgValue(binding, 'not_exist',
            canvas_win.client.RT.types.UInt32(5))
    canvas_win.onNewGraph(skip_save=True)
