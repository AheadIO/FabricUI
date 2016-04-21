"""Alembic Viewer Window."""

import json
import os

from PySide import QtCore, QtGui

from FabricEngine import Core, FabricUI
from FabricEngine.FabricUI import DFG, KLASTManager, TimeLine, Viewports
from FabricEngine.Canvas.ScriptEditor import ScriptEditor
from FabricEngine.Canvas.UICmdHandler import UICmdHandler
from FabricEngine.Canvas.CanvasWindow import CanvasWindow

from AlembicViewer.AlembicViewerUIWidget import AlembicViewerUIWidget


class AlembicViewerWindow(CanvasWindow):
    """AlembicViewerWindow

    This window encompasses the entire Alembic Viewer application. It integrates
    the custom widgets of the AlembicViewerUIWidget which includes the
    AlembicFileListWidget and adds it as the main visible dock widget when
    launched.

    Arguments:
        initDir (str): Initial directory path to set the file list widget to.

    """

    def __init__(self, initDir=None):

        self.initDir = initDir

        # Required! Cannot be set to none, or not included!
        self.dockFeatures = QtGui.QDockWidget.DockWidgetClosable

        settings = QtCore.QSettings()
        unguarded = False
        noopt = False
        super(AlembicViewerWindow, self).__init__(settings, unguarded, noopt)

        widgetStyleSheet = """

            QDockWidget::title {
                border: 0px;
                border-bottom-left-radius: 5px;
                border-bottom-right-radius: 5px;
                margin: 3px;
                padding: 5px 8px;

                color: #FFFFFF;
                background-color: #4f5a36;
            }

            QDockWidget {
                titlebar-close-icon: url(close.png);
            }

            QDockWidget::close-button {
                min-width: 11px;
                min-height: 11px;
                border: none;
                icon-size: 11px;
                background: transparent;

                position: relative;
                top: 6px;
                left: -8px;
            }

            QSlider#sceneScaleSlider::groove:horizontal {
                background-color: #222;
                height: 30px;
            }

            QSlider#sceneScaleSlider::handle:horizontal {
                border: 1px #438f99;
                border-style: outset;
                margin: -2px 0;
                width: 3px;
                height: 30px;

                background-color: #438f99;
            }

            QSlider#sceneScaleSlider::sub-page:horizontal {
                background: #4B4B4B;
            }

            QLineEdit {
                border: 0px;
                border-radius: 3px;
                padding: 3px;

                color: #e1ffff;
                background-color: #2b2b2b;
            }

            QLineEdit:hover {
                background-color: #2F2F2F;
            }

            QLabel {
                padding: 3px;
                border: 1px solid #222;
                border-radius: 4px;
                color: #FFFFFF;
                background-color: #222;
            }

            QPushButton {
                border: 1px #222;
                border-radius: 3px;
                border-style: outset;
                padding: 5px;
                width: 65px;

                background-color: #222;
                color: #e1ffff;
            }

            QPushButton:hover {
                background-color: #2b2b2b;
            }

            QPushButton::pressed {
                border-style: inset;
                padding-top: 7px;
                padding-left: 7px;

                background-color: #2b2b2b;
            }

            QListWidget {
                background-color: #333;
            }

            QListWidget::item {
                color: #e1ffff;
            }

            QListWidget::item:selected {
                color: white;
                background-color: #1D80A0;
            }

            QListWidget::item:hover {
                background-color: #2AB7E5;
            }

            """

        self.setStyleSheet(widgetStyleSheet)

        self.windowTitle = 'Fabric Engine - Alembic Viewer'
        self.setWindowTitle(self.windowTitle)

    def onFileNameChanged(self, fileName):
        """Callback for when the file name has changed.

        This method updates the window title to reflect the new file path.

        """

        pass

    def _initDocks(self):
        """Initializes all of doc widgets and menus for the application.

        The dock widgets host the main widgets for the application and are able
        to be toggled on and off via the menu items that are created. Some
        widgets that are hosted in dock widgets are instanced here and some have
        already been created such as the DFGWidget and TimeLineWidget.

        """

        if self.initDir is None or os.path.exists(self.initDir) is False:
            self.initDir = os.getcwd()

            fabricDir = os.environ.get('FABRIC_DIR', None)
            if fabricDir:
                self.initDir = os.path.join(fabricDir, 'Resources')

        # Alembic Doc Widget
        self.abcUIDockWidget = QtGui.QDockWidget('Alembic Viewer UI', self)
        self.abcUIDockWidget.setObjectName('abcViewerUI')
        self.abcUIDockWidget.setFeatures(self.dockFeatures)

        self.abcViewUIWidget = AlembicViewerUIWidget(self.initDir, self)
        self.abcViewUIWidget.selItemChanged.connect(self.changeLoadedAbc)
        self.abcViewUIWidget.fpsChanged.connect(self.changeFPS)
        self.abcViewUIWidget.sceneScaleChanged.connect(self.changeSceneScale)
        self.abcViewUIWidget.dirChanged.connect(self.clearFilePath)

        self.abcUIDockWidget.setWidget(self.abcViewUIWidget)
        self.addDockWidget(QtCore.Qt.LeftDockWidgetArea, self.abcUIDockWidget, QtCore.Qt.Vertical)

        # NOTE: Leave for Debugging

        # DFG Widget
        # dfgDockWidget = QtGui.QDockWidget('Canvas Graph', self)
        # dfgDockWidget.setObjectName('Canvas Graph')
        # dfgDockWidget.setFeatures(self.dockFeatures)
        # dfgDockWidget.setWidget(self.dfgWidget)
        # self.addDockWidget(QtCore.Qt.BottomDockWidgetArea, dfgDockWidget, QtCore.Qt.Vertical)

        # valueEditorDockWidget = QtGui.QDockWidget('Value Editor', self)
        # valueEditorDockWidget.setObjectName('Values')
        # valueEditorDockWidget.setFeatures(self.dockFeatures)
        # valueEditorDockWidget.setWidget(self.valueEditor.getWidget())
        # self.addDockWidget(QtCore.Qt.RightDockWidgetArea, valueEditorDockWidget)

        # scriptEditorDock = QtGui.QDockWidget('Script Editor', self)
        # scriptEditorDock.setObjectName('Script Editor')
        # scriptEditorDock.setFeatures(self.dockFeatures)
        # scriptEditorDock.setWidget(self.scriptEditor)
        # self.addDockWidget(QtCore.Qt.BottomDockWidgetArea, scriptEditorDock, QtCore.Qt.Vertical)

        # NOTE: End of Debugging

        # Log Dock Widget
        self.logDockWidget = QtGui.QDockWidget('Log Messages', self)
        self.logDockWidget.setObjectName('Log')
        self.logDockWidget.setFeatures(self.dockFeatures)
        self.logDockWidget.setWidget(self.logWidget)
        self.logDockWidget.hide()
        self.addDockWidget(QtCore.Qt.BottomDockWidgetArea, self.logDockWidget, QtCore.Qt.Vertical)

        # Timeline Dock Widget
        self.timeLineDockWidget = QtGui.QDockWidget('TimeLine', self)
        self.timeLineDockWidget.setObjectName('TimeLine')
        self.timeLineDockWidget.setFeatures(self.dockFeatures)
        self.timeLineDockWidget.setWidget(self.timeLine)
        self.addDockWidget(QtCore.Qt.BottomDockWidgetArea, self.timeLineDockWidget, QtCore.Qt.Vertical)

    def _initMenus(self):
        """Initializes all menus for the application."""

        # ==============
        # Add File Menu
        # ==============
        fileMenu = self.menuBar().addMenu('&File')

        # Add File Menu Actions
        self.quitAction = QtGui.QAction('Quit', fileMenu)
        self.quitAction.setShortcut(QtGui.QKeySequence.Quit)
        fileMenu.addAction(self.quitAction)

        self.quitAction.triggered.connect(self.close)

        # ==============
        # Add Edit Menu
        # ==============
        editMenu = self.menuBar().addMenu('&Edit')

        # Add Edit Menu Actions
        undoAction = QtGui.QAction('Undo', self)

        def onUndo():
            self.scriptEditor.eval('undo()')

        undoAction.triggered.connect(onUndo)
        undoAction.setEnabled(self.qUndoStack.canUndo())
        self.qUndoStack.canUndoChanged.connect(undoAction.setEnabled)
        undoAction.setShortcut(QtGui.QKeySequence.Undo)
        editMenu.addAction(undoAction)

        redoAction = QtGui.QAction('Redo', self)

        def onRedo():
            self.scriptEditor.eval('redo()')

        redoAction.triggered.connect(onRedo)
        redoAction.setEnabled(self.qUndoStack.canRedo())
        self.qUndoStack.canRedoChanged.connect(redoAction.setEnabled)
        redoAction.setShortcut(QtGui.QKeySequence.Redo)
        editMenu.addAction(redoAction)

        # ==============
        # Add View Menu
        # ==============
        viewMenu = self.menuBar().addMenu('&View')

        # Add View Menu Actions
        self.setGridVisibleAction = QtGui.QAction('&Display Grid', None)
        self.setGridVisibleAction.setShortcut(
            QtCore.Qt.CTRL + QtCore.Qt.Key_G)
        self.setGridVisibleAction.setCheckable(True)
        self.setGridVisibleAction.setChecked(
            self.viewport.isGridVisible())
        self.setGridVisibleAction.toggled.connect(
            self.viewport.setGridVisible)

        self.resetCameraAction = QtGui.QAction('&Reset Camera',
                                               self.viewport)
        self.resetCameraAction.setShortcut(QtGui.QKeySequence(QtCore.Qt.Key_R))
        self.resetCameraAction.setShortcutContext(
            QtCore.Qt.WidgetWithChildrenShortcut)
        self.resetCameraAction.triggered.connect(
            self.viewport.resetCamera)
        self.viewport.addAction(self.resetCameraAction)

        viewMenu.addAction(self.setGridVisibleAction)
        viewMenu.addSeparator()
        viewMenu.addAction(self.resetCameraAction)

        # ================
        # Add Window Menu
        # ================
        windowMenu = self.menuBar().addMenu('&Window')

        toggleAction = self.abcUIDockWidget.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_1)
        windowMenu.addAction(toggleAction)

        toggleAction = self.timeLineDockWidget.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_2)
        windowMenu.addAction(toggleAction)

        toggleAction = self.logDockWidget.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_3)
        windowMenu.addAction(toggleAction)

    # =============
    # Custom Slots
    # =============
    @QtCore.Slot(float)
    def changeFPS(self, fpsFloat):
        """Changes the currently loaded alembic file in the graph.

        Arguments:
            fpsFloat (int): New FPS.

        """

        args = [
            UICmdHandler.encodeString('fps'),
            UICmdHandler.encodeString('Float32'),
            UICmdHandler.encodeString(json.dumps(fpsFloat))
            ]

        self.dfguiCommandHandler.evalCmdWithArgs('setArgValue', args)

    @QtCore.Slot(str)
    def changeLoadedAbc(self, abcFilePath):
        """Changes the currently loaded alembic file in the graph.

        Arguments:
            abcFilePath (str): File path to the Alembic file that should be
            loaded.

        """

        args = [
            UICmdHandler.encodeString('filePath'),
            UICmdHandler.encodeString('String'),
            UICmdHandler.encodeString(json.dumps(abcFilePath))
            ]

        self.dfguiCommandHandler.evalCmdWithArgs('setArgValue', args)

    @QtCore.Slot(float)
    def changeSceneScale(self, sceneScale):
        """Changes the currently loaded alembic file in the graph.

        Arguments:
            sceneScale (float): New scene scale.

        """

        args = [
            UICmdHandler.encodeString('sceneScale'),
            UICmdHandler.encodeString('Float32'),
            UICmdHandler.encodeString(json.dumps(sceneScale))
            ]

        self.dfguiCommandHandler.evalCmdWithArgs('setArgValue', args)

    @QtCore.Slot()
    def clearFilePath(self):
        """Clears the file path parameter as the directory and selection as
        changed.

        """

        args = [
            UICmdHandler.encodeString('filePath'),
            UICmdHandler.encodeString('String'),
            UICmdHandler.encodeString(json.dumps(''))
            ]

        self.dfguiCommandHandler.evalCmdWithArgs('setArgValue', args)
