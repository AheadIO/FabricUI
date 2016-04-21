"""Alembic Viewer Window."""

import json
import os
import sys

from PySide import QtCore, QtGui, QtOpenGL

from FabricEngine import Core, FabricUI
from FabricEngine.FabricUI import DFG, KLASTManager, TimeLine, Viewports
from FabricEngine.Canvas.ScriptEditor import ScriptEditor
from FabricEngine.Canvas.UICmdHandler import UICmdHandler
from FabricEngine.Canvas.CanvasWindow import CanvasWindowEventFilter

from AlembicViewer.AlembicViewerUIWidget import AlembicViewerUIWidget


class AlembicViewerWindow(QtGui.QMainWindow):
    """AlembicViewerWindow

    This window encompasses the entire Alembic Viewer application. It integrates
    the custom widgets of the AlembicViewerUIWidget which includes the
    AlembicFileListWidget and adds it as the main visible dock widget when
    launched.

    Attributes:
        defaultFrameIn (int): Default in frame.
        defaultFrameOut (int): Default out frame.

    Arguments:
        settings (QtCore.QSettings): Settings object that is used to store and
            retrieve settings for the application.
        unguarded (bool): Whether to create the Fabric client in unguarded mode.
        initDir (str): Initial directory path to set the file list widget to.

    """

    defaultFrameIn = 1
    defaultFrameOut = 50

    def __init__(self, settings, unguarded, initDir=None):
        self.settings = settings

        super(AlembicViewerWindow, self).__init__()

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

        # Required! Cannot be set to none, or not included!
        self.dockFeatures = QtGui.QDockWidget.DockWidgetClosable

        self.__init()
        self._initWindow()
        self._initKL(unguarded)
        self._initLog()
        self._initDFG()
        self._initController()
        self._initGL()
        self._initValueEditor()
        self._initTimeLine()
        self._initDocksAndMenus(initDir)

        self.restoreGeometry(self.settings.value("mainWindow/geometry"))
        self.restoreState(self.settings.value("mainWindow/state"))
        self.onFrameChanged(self.timeLine.getTime())
        self.valueEditor.initConnections()
        self.installEventFilter(CanvasWindowEventFilter(self))

    def __init(self):
        """Initinalizes the settings and config for the application."""

        DFG.DFGWidget.setSettings(self.settings)
        self.config = DFG.DFGConfig()

    def _initWindow(self):
        """Initializes the window attributes, window widgets, actions, and the
        fps timer.

        """

        self.viewport = None
        self.dfgWidget = None
        self.currentGraph = None
        self.newGraphAction = None
        self.loadGraphAction = None
        self.saveGraphAction = None
        self.saveGraphAsAction = None
        self.quitAction = None
        self.manipAction = None
        self.setGridVisibleAction = None
        self.resetCameraAction = None
        self.clearLogAction = None
        self.blockCompilationsAction = None

        self.windowTitle = 'Fabric Engine - Alembic Viewer'
        self.lastFileName = ''

        self.setWindowTitle(self.windowTitle)

        statusBar = QtGui.QStatusBar(self)
        self.fpsLabel = QtGui.QLabel(statusBar)
        statusBar.addPermanentWidget(self.fpsLabel)
        self.setStatusBar(statusBar)
        statusBar.show()

        self.fpsTimer = QtCore.QTimer()
        self.fpsTimer.setInterval(1000)
        self.fpsTimer.timeout.connect(self.updateFPS)
        self.fpsTimer.start()

    def __reportCallback(self, source, level, line):
        """Call back method that fires when the client emits reports.

        Report sources:
            NONE = 0
            System = 1
            User = 2
            ALL = 3

        Report levels:
            Error = 0
            Warning = 1
            Info = 2
            Debug = 3

        Arguments:
            source (int): Source of the report call.
            level (int): Level of the report.
            line (int): Line number where the report was called.

        """

        if self.dfgWidget:
            self.dfgWidget.getDFGController().log(line)
        else:
            if source == Core.ReportSource.User or 'Ignoring' in line:
                sys.stdout.write(line + "\n")
            else:
                sys.stderr.write(line + "\n")

    def __statusCallback(self, target, data):
        """Status callback used for KL code to communicate status messages back
        to the client.

        Arguments:
            target (str): Target of the callback.
            data: Data to be sent to the target.

        """

        if target == "licensing":
            try:
                FabricUI.HandleLicenseData(self, self.client, data, True)
            except Exception as e:
                self.dfgWidget.getDFGController().logError(str(e))

    def _initKL(self, unguarded):
        """Initializes the Fabric client.

        The core client is what drives the application and what the user
        interface interacts with to create data types and process the data. The
        client is required in all Fabric based applications.

        Arguments:
            unguarded (bool): Sets the client to run in guarded mode or not.

        """

        clientOpts = {
            'guarded': not unguarded,
            'reportCallback': self.__reportCallback
        }

        client = Core.createClient(clientOpts)
        #options.licenseType = FabricCore::ClientLicenseType_Interactive
        client.loadExtension('Math')
        client.loadExtension('Parameters')
        client.loadExtension('Util')
        client.loadExtension('Alembic')
        client.setStatusCallback(self.__statusCallback)
        self.client = client
        self.qUndoStack = QtGui.QUndoStack()

    def _initDFG(self):
        """Initializes the Data Flow Graph.

        An evalContext is created to provide contextual information about the
        evaluation to operators and other objects and systems with Fabric
        Engine.

        A binding to a graph is initialized and is setup so the application can
        interact with it via the DFGWidget and through other scripted methods
        within the application.

        The UICmdHandler handles the interaction between the UI and the client.

        The DFGWidget is the UI that reflects the binding to the graph that is
        created and changed through the application.

        """

        self.evalContext = self.client.RT.types.EvalContext.create()
        self.evalContext = self.evalContext.getInstance('EvalContext')
        self.evalContext.host = 'AlembicViewer'
        self.evalContext.graph = ''

        astManager = KLASTManager(self.client)
        self.host = self.client.getDFGHost()
        binding = self.host.createBindingToNewGraph()
        self.lastSavedBindingVersion = binding.getVersion()
        self.lastAutosaveBindingVersion = self.lastSavedBindingVersion

        graph = binding.getExec()
        self.scriptEditor = ScriptEditor(self.client, binding, self.qUndoStack, self.logWidget)
        self.dfguiCommandHandler = UICmdHandler(self.client, self.scriptEditor)

        self.dfgWidget = DFG.DFGWidget(None, self.client, self.host,
                                       binding, '', graph, astManager,
                                       self.dfguiCommandHandler, self.config)

        self.dfgWidget.hide()

    def _initController(self):
        """Connects the DFG Controller's dirty signal to the onDirty method."""

        controller = self.dfgWidget.getDFGController()
        controller.dirty.connect(self.onDirty)

    def _initGL(self):
        """Initializes the Open GL viewport widget."""

        glFormat = QtOpenGL.QGLFormat()
        glFormat.setDoubleBuffer(True)
        glFormat.setDepth(True)
        glFormat.setAlpha(True)
        glFormat.setSampleBuffers(True)
        glFormat.setSamples(4)

        self.viewport = Viewports.GLViewportWidget(self.client, self.config.defaultWindowColor, glFormat, self, self.settings)
        self.setCentralWidget(self.viewport)

    def _initValueEditor(self):
        """Initializes the value editor."""

        self.valueEditor = FabricUI.DFG.DFGVEEditorOwner(self.dfgWidget)

    def _initLog(self):
        """Initializes the DFGLogWidget and Undo view."""

        self.logWidget = DFG.DFGLogWidget(self.config)
        self.qUndoView = QtGui.QUndoView(self.qUndoStack)
        self.qUndoView.setEmptyLabel("New Graph")

    def _initTimeLine(self):
        """Initializes the TimeLineWidget.

        The timeline widget is setup with the class variables for the default in
        and out frames.

        The frameChanged signal is connected to the onFrameChanged method along
        with the Value Editor's onFrameChanged method too.

        """

        self.timeLinePortIndex = -1
        self.timeLinePortPath = None
        self.timeLine = TimeLine.TimeLineWidget()
        self.timeLine.setTimeRange(AlembicViewerWindow.defaultFrameIn, AlembicViewerWindow.defaultFrameOut)
        self.timeLine.updateTime(1)
        self.timeLine.frameChanged.connect(self.onFrameChanged)
        self.timeLine.frameChanged.connect(self.valueEditor.onFrameChanged)

    def _initDocksAndMenus(self, initDir):
        """Initializes all of doc widgets and menus for the application.

        The dock widgets host the main widgets for the application and are able
        to be toggled on and off via the menu items that are created. Some
        widgets that are hosted in dock widgets are instanced here and some have
        already been created such as the DFGWidget and TimeLineWidget.

        Arguments:
            initDir (str): The initial directory that the application loads
            Alembic files from.

        """

        if initDir is None or os.path.exists(initDir) is False:
            initDir = os.getcwd()

            fabricDir = os.environ.get('FABRIC_DIR', None)
            if fabricDir:
                initDir = os.path.join(fabricDir, 'Resources')

        # Alembic Doc Widget
        abcUIDockWidget = QtGui.QDockWidget('Alembic Viewer UI', self)
        abcUIDockWidget.setObjectName('abcViewerUI')
        abcUIDockWidget.setFeatures(self.dockFeatures)

        self.abcViewUIWidget = AlembicViewerUIWidget(initDir, self)
        self.abcViewUIWidget.selItemChanged.connect(self.changeLoadedAbc)
        self.abcViewUIWidget.fpsChanged.connect(self.changeFPS)
        self.abcViewUIWidget.sceneScaleChanged.connect(self.changeSceneScale)
        self.abcViewUIWidget.dirChanged.connect(self.clearFilePath)

        abcUIDockWidget.setWidget(self.abcViewUIWidget)
        self.addDockWidget(QtCore.Qt.LeftDockWidgetArea, abcUIDockWidget, QtCore.Qt.Vertical)

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
        logDockWidget = QtGui.QDockWidget('Log Messages', self)
        logDockWidget.setObjectName('Log')
        logDockWidget.setFeatures(self.dockFeatures)
        logDockWidget.setWidget(self.logWidget)
        logDockWidget.hide()
        self.addDockWidget(QtCore.Qt.BottomDockWidgetArea, logDockWidget, QtCore.Qt.Vertical)

        # Timeline Dock Widget
        self.timeLineDockWidget = QtGui.QDockWidget('TimeLine', self)
        self.timeLineDockWidget.setObjectName('TimeLine')
        self.timeLineDockWidget.setFeatures(self.dockFeatures)
        self.timeLineDockWidget.setWidget(self.timeLine)
        self.addDockWidget(QtCore.Qt.BottomDockWidgetArea, self.timeLineDockWidget, QtCore.Qt.Vertical)

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

        toggleAction = abcUIDockWidget.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_1)
        windowMenu.addAction(toggleAction)

        toggleAction = self.timeLineDockWidget.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_2)
        windowMenu.addAction(toggleAction)

        toggleAction = logDockWidget.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_3)
        windowMenu.addAction(toggleAction)

    def _contentChanged(self) :
        """Method called to tell the application services that content has been
        changed.

        """

        self.valueEditor.onOutputsChanged()
        self.viewport.redraw()

    def onDirty(self):
        """Method called when the graph is dirtied."""

        self.dfgWidget.getDFGController().execute()
        self._contentChanged()

    def loadGraph(self, filePath):
        """Method to load a graph from disk.

        Files are tyipcally *.canvas files.

        Arguments:
            filePath (str): Path to the graph to load.

        """

        self.timeLine.pause()
        self.timeLinePortPath = None

        try:
            dfgController = self.dfgWidget.getDFGController()
            binding = dfgController.getBinding()
            binding.deallocValues()

            self.host.flushUndoRedo()
            self.qUndoStack.clear()
            self.qUndoView.setEmptyLabel('Load Graph')
            self.viewport.clearInlineDrawing()

            QtCore.QCoreApplication.processEvents()

            jsonVal = open(filePath, 'rb').read()
            binding = self.host.createBindingFromJSON(jsonVal)
            self.lastSavedBindingVersion = binding.getVersion()
            dfgExec = binding.getExec()
            dfgController.setBindingExec(binding, '', dfgExec)

            self.scriptEditor.updateBinding(binding)

            self.evalContext.currentFilePath = filePath
            dfgController.execute()

            tl_start = dfgExec.getMetadata('timeline_start')
            tl_end = dfgExec.getMetadata('timeline_end')
            tl_loopMode = dfgExec.getMetadata('timeline_loopMode')
            tl_simulationMode = dfgExec.getMetadata('timeline_simMode')
            tl_current = dfgExec.getMetadata('timeline_current')

            if len(tl_start) > 0 and len(tl_end) > 0:
                self.timeLine.setTimeRange(int(tl_start), int(tl_end))
            else:
                self.timeLine.setTimeRange(AlembicViewerWindow.defaultFrameIn,
                                           AlembicViewerWindow.defaultFrameOut)

            if len(tl_loopMode) > 0:
                self.timeLine.setLoopMode(int(tl_loopMode))
            else:
                self.timeLine.setLoopMode(1)

            if len(tl_simulationMode) > 0:
                self.timeLine.setSimulationMode(int(tl_simulationMode))
            else:
                self.timeLine.setSimulationMode(0)

            camera_mat44 = dfgExec.getMetadata('camera_mat44')
            camera_focalDistance = dfgExec.getMetadata('camera_focalDistance')
            if len(camera_mat44) > 0 and len(camera_focalDistance) > 0:
                try:
                    mat44 = self.client.RT.constructRTValFromJSON('Mat44', camera_mat44)
                    focalDistance = self.client.RT.constructRTValFromJSON('Float32',
                        camera_focalDistance)

                    camera = self.viewport.getCamera()
                    camera.setFromMat44('', mat44)
                    camera.setFocalDistance('', focalDistance)
                except Exception as e:
                    sys.stderr.write("Exception: " + str(e) + "\n")

            self._contentChanged()

            # then set it to the current value if we still have it.
            # this will ensure that sim mode scenes will play correctly.
            if len(tl_current) > 0:
                self.timeLine.updateTime(int(tl_current), True)
            else:
                self.timeLine.updateTime(AlembicViewerWindow.defaultFrameIn, True)

            self.viewport.update()

        except Exception as e:
            sys.stderr.write("Exception: " + str(e) + "\n")

        self.lastFileName = filePath

    def closeEvent(self, event):
        """Standard close event method called when the Window is closed.

        Settings for the application are stored and the window widget is closed.
        The super class's closeEvent is then fired and the Fabric client is
        closed.

        Arguments:
            event (QtCore.QEvent): Event that has been triggered.

        """

        self.viewport.setManipulationActive(False)
        self.settings.setValue("mainWindow/geometry", self.saveGeometry())
        self.settings.setValue("mainWindow/state", self.saveState())

        super(AlembicViewerWindow, self).closeEvent(event)

        self.client.close()

    def onFrameChanged(self, frame):
        """Method called when the user has changed frames.

        The frame change call pushes the new frame into the binding ports which
        map to the timeline.

        Arguments:
            frame (float): The new frame the user has changed to.

        """

        try:
            self.evalContext.time = frame
        except Exception as e:
            self.dfgWidget.getDFGController().logError(str(e))

        if not self.timeLinePortPath:
            return

        try:
            binding = self.dfgWidget.getDFGController().getBinding()
            dfgExec = binding.getExec()
            if dfgExec.isExecPortResolvedType(self.timeLinePortIndex,
                                              'SInt32'):
                binding.setArgValue(self.timeLinePortPath,
                                    self.client.RT.types.SInt32(frame), False)
            elif dfgExec.isExecPortResolvedType(self.timeLinePortIndex,
                                                'UInt32'):
                binding.setArgValue(self.timeLinePortPath,
                                    self.client.RT.types.UInt32(frame), False)
            elif dfgExec.isExecPortResolvedType(self.timeLinePortIndex,
                                                'Float32'):
                binding.setArgValue(self.timeLinePortPath,
                                    self.client.RT.types.Float32(frame), False)
            elif dfgExec.isExecPortResolvedType(self.timeLinePortIndex,
                                                'Float64'):
                binding.setArgValue(self.timeLinePortPath,
                                    self.client.RT.types.Float64(frame), False)
        except Exception as e:
            self.dfgWidget.getDFGController().logError(str(e))

    def updateFPS(self):
        """Method for updating the FPS label in the status bar."""

        if not self.viewport:
            return

        caption = str(round(self.viewport.fps(), 2)) + ' FPS'
        self.fpsLabel.setText(caption)

    # =============
    # Custom Slots
    # =============
    @QtCore.Slot(float)
    def changeFPS(self, fpsFloat):
        """Changes the currently loaded alembic file in the graph.

        Arguments:
            fpsFloat (int): New FPS.

        """

        args = ["\"fps\"", "\"Float32\"", "\"" + self.dfguiCommandHandler.encodeFloat(fpsFloat) + "\""]

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
