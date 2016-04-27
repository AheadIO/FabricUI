"""Canvas Window."""

import os
import sys

from PySide import QtCore, QtGui, QtOpenGL
from FabricEngine import Core, FabricUI
from FabricEngine.FabricUI import DFG, KLASTManager, Viewports, TimeLine
from FabricEngine.Canvas.ScriptEditor import ScriptEditor
from FabricEngine.Canvas.UICmdHandler import UICmdHandler

class CanvasWindowEventFilter(QtCore.QObject):

    def __init__(self, window):
        super(CanvasWindowEventFilter, self).__init__()
        self.window = window

    def eventFilter(self, obj, event):
        eventType = event.type()

        if eventType == QtCore.QEvent.KeyPress:
            keyEvent = QtGui.QKeyEvent(event)

            if keyEvent.key() != QtCore.Qt.Key_Tab:
                self.window.viewport.onKeyPressed(keyEvent)
                if keyEvent.isAccepted():
                    return True

                self.window.dfgWidget.onKeyPressed(keyEvent)
                if keyEvent.isAccepted():
                    return True
        elif eventType == QtCore.QEvent.KeyRelease:
            keyEvent = QtGui.QKeyEvent(event)

            if keyEvent.key() != QtCore.Qt.Key_Tab:
                self.window.dfgWidget.onKeyReleased(keyEvent)
                if keyEvent.isAccepted():
                    return True

        return QtCore.QObject.eventFilter(obj, event)

class CanvasWindow(DFG.DFGMainWindow):
    """CanvasWindow

    This window encompasses the entire Canvas application.

    Attributes:
        defaultFrameIn (int): Default in frame.
        defaultFrameOut (int): Default out frame.
        autosaveIntervalSecs (int): Interval at which to autosave the current graph.

    Arguments:
        settings (QtCore.QSettings): Settings object that is used to store and
            retrieve settings for the application.
        unguarded (bool): Whether to create the Fabric client in unguarded mode.
        noopt (bool): Whether to create the Fabric client in noopt mode.

    """

    defaultFrameIn = 1
    defaultFrameOut = 50
    autosaveIntervalSecs = 30

    def __init__(self, settings, unguarded, noopt):
        self.settings = settings

        super(CanvasWindow, self).__init__()

        self.autosaveTimer = QtCore.QTimer()
        self.autosaveTimer.timeout.connect(self.autosave)
        self.autosaveTimer.start(CanvasWindow.autosaveIntervalSecs * 1000)
        self.dockFeatures = QtGui.QDockWidget.DockWidgetMovable | QtGui.QDockWidget.DockWidgetFloatable | QtGui.QDockWidget.DockWidgetClosable

        self.__init()
        self._initWindow()
        self._initKL(unguarded, noopt)
        self._initLog()
        self._initDFG()
        self._initTreeView()
        self._initGL()
        self._initValueEditor()
        self._initTimeLine()
        self._initDocks()
        self._initMenus()

        self.restoreGeometry(self.settings.value("mainWindow/geometry"))
        self.restoreState(self.settings.value("mainWindow/state"))
        self.onFrameChanged(self.timeLine.getTime())
        self.onGraphSet(self.dfgWidget.getUIGraph())
        self.valueEditor.initConnections()
        self.installEventFilter(CanvasWindowEventFilter(self))

    def __init(self):
        """Initinalizes the settings and config for the application.

        The autosave directory and file name are established here.

        """

        self.setAcceptDrops(True)

        DFG.DFGWidget.setSettings(self.settings)
        self.config = DFG.DFGConfig()

        # [andrew 20160414] despite similar names this is not the same as FABRIC_DIR
        userFabricDir = Core.CAPI.GetFabricDir()
        self.autosaveFilename = os.path.join(userFabricDir, 'autosave')
        if not os.path.exists(self.autosaveFilename):
            os.makedirs(self.autosaveFilename)

        autosaveBasename = 'autosave.' + str(os.getpid()) + '.canvas'
        self.autosaveFilename = os.path.join(self.autosaveFilename, autosaveBasename)
        print 'Will autosave to ' + self.autosaveFilename + ' every ' + str(
            CanvasWindow.autosaveIntervalSecs) + ' seconds'

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

        self.windowTitle = 'Fabric Engine'
        self.lastFileName = ''
        self.onFileNameChanged('')

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

    def _initKL(self, unguarded, noopt):
        """Initializes the Fabric client.

        The core client is what drives the application and what the user
        interface interacts with to create data types and process the data. The
        client is required in all Fabric based applications.

        Arguments:
            unguarded (bool): Sets the client to run in guarded mode or not.
            noopt (bool): Set the client to skip KL code optimization

        """

        clientOpts = {
          'guarded': not unguarded,
          'noOptimization': noopt,
          'reportCallback': self.__reportCallback
          }

        client = Core.createClient(clientOpts)
        #options.licenseType = FabricCore::ClientLicenseType_Interactive
        client.loadExtension('Math')
        client.loadExtension('Parameters')
        client.loadExtension('Util')
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
        self.evalContext.host = 'Canvas'
        self.evalContext.graph = ''

        astManager = KLASTManager(self.client)
        self.host = self.client.getDFGHost()
        binding = self.host.createBindingToNewGraph()
        self.lastSavedBindingVersion = binding.getVersion()
        self.lastAutosaveBindingVersion = self.lastSavedBindingVersion

        graph = binding.getExec()
        self.scriptEditor = ScriptEditor(self.client, binding, self.qUndoStack, self.logWidget, self.settings, self)
        self.dfguiCommandHandler = UICmdHandler(self.client, self.scriptEditor)

        self.dfgWidget = DFG.DFGWidget(None, self.client, self.host,
                                       binding, '', graph, astManager,
                                       self.dfguiCommandHandler, self.config)

        tabSearchWidget = self.dfgWidget.getTabSearchWidget()
        tabSearchWidget.enabled.connect(self.enableShortCuts)

        self.dfgWidget.onGraphSet.connect(self.onGraphSet)
        self.dfgWidget.additionalMenuActionsRequested.connect(self.onAdditionalMenuActionsRequested)
        self.dfgWidget.urlDropped.connect(self.onUrlDropped)

    def _initTreeView(self):
        """Initializes the preset TreeView.

        Also connects the DFG Controller's dirty signal to the onDirty method.

        """

        controller = self.dfgWidget.getDFGController()
        self.treeWidget = DFG.PresetTreeWidget(controller, self.config, True, False, False, False, False, True)
        self.dfgWidget.newPresetSaved.connect(self.treeWidget.refresh)
        controller.varsChanged.connect(self.treeWidget.refresh)
        controller.dirty.connect(self.onDirty)
        controller.topoDirty.connect(self.onTopoDirty)

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
        self.viewport.portManipulationRequested.connect(self.onPortManipulationRequested)

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
        self.timeLine.setTimeRange(CanvasWindow.defaultFrameIn, CanvasWindow.defaultFrameOut)
        self.timeLine.updateTime(1)
        self.timeLine.frameChanged.connect(self.onFrameChanged)
        self.timeLine.frameChanged.connect(self.valueEditor.onFrameChanged)

    def _initDocks(self):
        """Initializes all of dock widgets for the application.

        The dock widgets host the main widgets for the application and are able
        to be toggled on and off via the menu items that are created. Some
        widgets that are hosted in dock widgets are instanced here and some have
        already been created such as the DFGWidget and TimeLineWidget.

        """

        # Undo Dock Widget
        self.undoDockWidget = QtGui.QDockWidget("History", self)
        self.undoDockWidget.setObjectName("History")
        self.undoDockWidget.setFeatures(self.dockFeatures)
        self.undoDockWidget.setWidget(self.qUndoView)
        self.undoDockWidget.hide()     
        self.addDockWidget(QtCore.Qt.LeftDockWidgetArea, self.undoDockWidget)

        # Log Dock Widget
        self.logDockWidget = QtGui.QDockWidget("Log Messages", self)
        self.logDockWidget.setObjectName("Log")
        self.logDockWidget.setFeatures(self.dockFeatures)
        self.logDockWidget.setWidget(self.logWidget)
        self.logDockWidget.hide()
        self.addDockWidget(QtCore.Qt.TopDockWidgetArea, self.logDockWidget, QtCore.Qt.Vertical)

        # Value Editor Dock Widget
        self.valueEditorDockWidget = QtGui.QDockWidget("Value Editor", self)
        self.valueEditorDockWidget.setObjectName("Values")
        self.valueEditorDockWidget.setFeatures(self.dockFeatures)
        self.valueEditorDockWidget.setWidget(self.valueEditor.getWidget())
        self.addDockWidget(QtCore.Qt.RightDockWidgetArea, self.valueEditorDockWidget)

        # DFG Dock Widget
        self.dfgDock = QtGui.QDockWidget('Canvas Graph', self)
        self.dfgDock.setObjectName('Canvas Graph')
        self.dfgDock.setFeatures(self.dockFeatures)
        self.dfgDock.setWidget(self.dfgWidget)
        self.addDockWidget(QtCore.Qt.BottomDockWidgetArea, self.dfgDock, QtCore.Qt.Vertical)

        # Explorer Dock Widget
        self.treeDock = QtGui.QDockWidget("Explorer", self)
        self.treeDock.setObjectName("Explorer")
        self.treeDock.setFeatures(self.dockFeatures)
        self.treeDock.setWidget(self.treeWidget)
        self.addDockWidget(QtCore.Qt.LeftDockWidgetArea, self.treeDock)

        # Timeline Dock Widget
        self.timeLineDock = QtGui.QDockWidget("TimeLine", self)
        self.timeLineDock.setObjectName("TimeLine")
        self.timeLineDock.setFeatures(self.dockFeatures)
        self.timeLineDock.setWidget(self.timeLine)
        self.addDockWidget(QtCore.Qt.BottomDockWidgetArea, self.timeLineDock, QtCore.Qt.Vertical)

        # Script Editor Dock Widget
        self.scriptEditorDock = QtGui.QDockWidget('Script Editor', self)
        self.scriptEditorDock.setObjectName('Script Editor')
        self.scriptEditorDock.setFeatures(self.dockFeatures)
        self.scriptEditorDock.setWidget(self.scriptEditor)
        self.addDockWidget(QtCore.Qt.BottomDockWidgetArea, self.scriptEditorDock, QtCore.Qt.Vertical)

    def _initMenus(self):
        """Initializes all of menus for the application.

        """

        # Main Menu Bar
        self.dfgWidget.populateMenuBar(self.menuBar())
        windowMenu = self.menuBar().addMenu('&Window')

        # Toggle DFG Dock Widget Action
        toggleAction = self.dfgDock.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_1)
        windowMenu.addAction(toggleAction)

        # Toggle Explorer Dock Widget Action
        toggleAction = self.treeDock.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_2)
        windowMenu.addAction(toggleAction)

        # Toggle Value Editor Dock Widget Action
        toggleAction = self.valueEditorDockWidget.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_6)
        windowMenu.addAction(toggleAction)

        # Toggle Timeline Dock Widget Action
        toggleAction = self.timeLineDock.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_4)
        windowMenu.addAction(toggleAction)

        windowMenu.addSeparator()

        # Toggle Undo Dock Widget Action
        toggleAction = self.undoDockWidget.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_5)
        windowMenu.addAction(toggleAction)

        # Toggle Log Dock Widget Action
        toggleAction = self.logDockWidget.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_6)
        windowMenu.addAction(toggleAction)

        # Toggle Script Editor Dock Widget Action
        toggleAction = self.scriptEditorDock.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_7)
        windowMenu.addAction(toggleAction)

    def _contentChanged(self) :
        """Method called to tell the application services that content has been
        changed.

        """

        self.valueEditor.onOutputsChanged()
        self.viewport.redraw()

    def onPortManipulationRequested(self, portName):
        """Method to trigger value changes that are requested form manipulators
        in the viewport.

        Arguments:
            portName (str): Name of the port that is being driven.

        """

        try:
            controller = self.dfgWidget.getDFGController()
            binding = controller.getBinding()
            dfgExec = binding.getExec()
            portResolvedType = dfgExec.getExecPortResolvedType(str(portName))
            value = self.viewport.getManipTool().getLastManipVal()
            if portResolvedType == 'Xfo':
                pass
            elif portResolvedType == 'Mat44':
                value = value.toMat44('Mat44')
            elif portResolvedType == 'Vec3':
                value = value.tr
            elif portResolvedType == 'Quat':
                value = value.ori
            else:
                message = "Port '" + portName
                message += "'to be driven has unsupported type '"
                message += portResolvedType.data()
                message += "'."
                self.dfgWidget.getDFGController().logError(message)
                return
            controller.cmdSetArgValue(portName, value)
        except Exception as e:
            self.dfgWidget.getDFGController().logError(str(e))

    def onDirty(self):
        """Method called when the graph is dirtied."""

        self.dfgWidget.getDFGController().execute()
        self._contentChanged()

    def onTopoDirty(self):
        self.onDirty()

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
            self.qUndoView.setEmptyLabel("Load Graph")
            self.viewport.clearInlineDrawing()

            QtCore.QCoreApplication.processEvents()

            jsonVal = open(filePath, 'rb').read()
            binding = self.host.createBindingFromJSON(jsonVal)
            self.lastSavedBindingVersion = binding.getVersion()
            dfgExec = binding.getExec()
            dfgController.setBindingExec(binding, "", dfgExec)
            self.scriptEditor.updateBinding(binding)

            self.evalContext.currentFilePath = filePath

            tl_start = dfgExec.getMetadata("timeline_start")
            tl_end = dfgExec.getMetadata("timeline_end")
            tl_loopMode = dfgExec.getMetadata("timeline_loopMode")
            tl_simulationMode = dfgExec.getMetadata("timeline_simMode")
            tl_current = dfgExec.getMetadata("timeline_current")

            if len(tl_start) > 0 and len(tl_end) > 0:
                self.timeLine.setTimeRange(int(tl_start), int(tl_end))
            else:
                self.timeLine.setTimeRange(CanvasWindow.defaultFrameIn,
                                           CanvasWindow.defaultFrameOut)

            if len(tl_loopMode) > 0:
                self.timeLine.setLoopMode(int(tl_loopMode))
            else:
                self.timeLine.setLoopMode(1)

            if len(tl_simulationMode) > 0:
                self.timeLine.setSimulationMode(int(tl_simulationMode))
            else:
                self.timeLine.setSimulationMode(0)

            camera_mat44 = dfgExec.getMetadata("camera_mat44")
            camera_focalDistance = dfgExec.getMetadata("camera_focalDistance")
            if len(camera_mat44) > 0 and len(camera_focalDistance) > 0:
                try:
                    mat44 = self.client.RT.constructRTValFromJSON('Mat44', camera_mat44)
                    focalDistance = self.client.RT.constructRTValFromJSON('Float32',
                        camera_focalDistance)

                    camera = self.viewport.getCamera()
                    camera.setFromMat44('', mat44)
                    camera.setFocalDistance("", focalDistance)
                except Exception as e:
                    sys.stderr.write("Exception: " + str(e) + "\n")

            self._contentChanged()

            self.onFileNameChanged(filePath)

            # then set it to the current value if we still have it.
            # this will ensure that sim mode scenes will play correctly.
            if len(tl_current) > 0:
                self.timeLine.updateTime(int(tl_current), True)
            else:
                self.timeLine.updateTime(CanvasWindow.defaultFrameIn, True)

            self.viewport.update()

        except Exception as e:
            sys.stderr.write("Exception: " + str(e) + "\n")

        self.lastFileName = filePath

    def onSaveGraph(self):
        """Method called when the graph is saved."""

        self.saveGraph(False)

    def onSaveGraphAs(self):
        """Method called when the graph is to be saved under a different file
        name.

        """

        self.saveGraph(True)

    def closeEvent(self, event):
        """Standard close event method called when the Window is closed.

        Settings for the application are stored and the window widget is closed.
        The super class's closeEvent is then fired and the Fabric client is
        closed.

        Arguments:
            event (QtCore.QEvent): Event that has been triggered.

        """

        if not self.checkUnsavedChanges():
            event.ignore()
            return
        if not self.scriptEditor.checkUnsavedChanges():
            event.ignore()
            return
            
        self.viewport.setManipulationActive(False)
        self.settings.setValue("mainWindow/geometry", self.saveGeometry())
        self.settings.setValue("mainWindow/state", self.saveState())

        QtGui.QMainWindow.closeEvent(self, event)

        self.client.close()

        if os.path.exists(self.autosaveFilename):
            os.remove(self.autosaveFilename)

    def checkUnsavedChanges(self):
        """Checks for any unsaved changes in the graph.

        If there are changes that haven't been saved the user is prompted with a
        dialog asking them if the graph should be saved.

        """

        binding = self.dfgWidget.getDFGController().getBinding()

        if binding.getVersion() != self.lastSavedBindingVersion:
            msgBox = QtGui.QMessageBox()
            msgBox.setText("Do you want to save your changes?")
            msgBox.setInformativeText(
                "Your changes will be lost if you don't save them.")
            msgBox.setStandardButtons(QtGui.QMessageBox.Save |
                                      QtGui.QMessageBox.Discard |
                                      QtGui.QMessageBox.Cancel)
            msgBox.setDefaultButton(QtGui.QMessageBox.Save)
            result = msgBox.exec_()
            if result == QtGui.QMessageBox.Discard:
                return True
            elif result == QtGui.QMessageBox.Cancel:
                return False
            else:
                return self.saveGraph(False)
        return True

    def onNodeEditRequested(self, node):
        """Method that is called when a request to edit the specified node has
        been emitted.

        """

        self.dfgWidget.maybeEditNode(node)

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
                                              "SInt32"):
                binding.setArgValue(self.timeLinePortPath,
                                    self.client.RT.types.SInt32(frame), False)
            elif dfgExec.isExecPortResolvedType(self.timeLinePortIndex,
                                                "UInt32"):
                binding.setArgValue(self.timeLinePortPath,
                                    self.client.RT.types.UInt32(frame), False)
            elif dfgExec.isExecPortResolvedType(self.timeLinePortIndex,
                                                "Float32"):
                binding.setArgValue(self.timeLinePortPath,
                                    self.client.RT.types.Float32(frame), False)
            elif dfgExec.isExecPortResolvedType(self.timeLinePortIndex,
                                                "Float64"):
                binding.setArgValue(self.timeLinePortPath,
                                    self.client.RT.types.Float64(frame), False)
        except Exception as e:
            self.dfgWidget.getDFGController().logError(str(e))

    def updateFPS(self):
        """Method for updating the FPS label in the status bar."""

        if not self.viewport:
            return

        caption = str(round(self.viewport.fps(), 2)) + " FPS"
        self.fpsLabel.setText(caption)

    def autosave(self):
        """Saves the scene when the auto-save timer triggers."""

        # [andrew 20150909] can happen if this triggers while the licensing
        # dialogs are up
        if not self.dfgWidget or not self.dfgWidget.getDFGController():
            return

        binding = self.dfgWidget.getDFGController().getBinding()
        if binding:
            bindingVersion = binding.getVersion()
            if bindingVersion != self.lastAutosaveBindingVersion:
                tmpAutosaveFilename = self.autosaveFilename
                tmpAutosaveFilename += ".tmp"

                if self.performSave(binding, tmpAutosaveFilename):
                    # [andrew 20160414] os.rename fails on Windows if file exists
                    if os.path.exists(self.autosaveFilename):
                        os.remove(self.autosaveFilename)
                    os.rename(tmpAutosaveFilename, self.autosaveFilename)
                    self.lastAutosaveBindingVersion = bindingVersion

    def execNewGraph(self, skip_save=False):
        """Callback Executed when a key or menu command has requested a new graph.

        This simply executes the corresponding script command.

        Arguments:
            skip_save (bool): Whether to skip the check for unsaved changes.

        """

        self.scriptEditor.exec_("newGraph(skip_save=%s)" % str(skip_save))

    def onNewGraph(self, skip_save=False):
        """Callback Executed when a call to create a new graph has been made.

        This method deallocates the previous binding and creates a new one. It
        also resets the undo stack, clears inline drawing, and sets the
        timeline.

        Arguments:
            skip_save (bool): Whether to skip the check for unsaved changes.

        """

        self.timeLine.pause()

        if not skip_save:
            if not self.checkUnsavedChanges():
                return

        self.lastFileName = ""

        try:
            dfgController = self.dfgWidget.getDFGController()

            binding = dfgController.getBinding()
            binding.deallocValues()

            self.host.flushUndoRedo()
            self.qUndoStack.clear()
            self.viewport.clearInlineDrawing()
            QtCore.QCoreApplication.processEvents()

            # Note: the previous binding is no longer functional
            #             create the new one before resetting the timeline options

            binding = self.host.createBindingToNewGraph()
            self.lastSavedBindingVersion = binding.getVersion()
            dfgExec = binding.getExec()
            self.timeLinePortPath = None

            dfgController.setBindingExec(binding, '', dfgExec)
            self.scriptEditor.updateBinding(binding)

            self.timeLine.setTimeRange(CanvasWindow.defaultFrameIn,
                                       CanvasWindow.defaultFrameOut)
            self.timeLine.setLoopMode(1)
            self.timeLine.setSimulationMode(0)
            self.timeLine.updateTime(CanvasWindow.defaultFrameIn, True)

            QtCore.QCoreApplication.processEvents()
            self.qUndoView.setEmptyLabel("New Graph")

            self._contentChanged()

            self.onFileNameChanged('')

            self.viewport.update()

        except Exception as e:
            print 'Exception: ' + str(e)
            raise e

    def onLoadGraph(self):
        """Callback for when users wish to load a graph from the UI.

        A file dialog is opened and users can select the file to load. The last
        directory the user saved or opened a graph from is used.

        """

        self.timeLine.pause()

        if not self.checkUnsavedChanges():
            return

        lastPresetFolder = str(self.settings.value(
            "mainWindow/lastPresetFolder"))
        filePath, _ = QtGui.QFileDialog.getOpenFileName(
            self, "Load graph", lastPresetFolder, "*.canvas")

        filePath = str(filePath)
        if len(filePath) > 0:
            folder = QtCore.QDir(filePath)
            folder.cdUp()
            self.settings.setValue("mainWindow/lastPresetFolder",
                                   str(folder.path()))
            self.loadGraph(filePath)

    def performSave(self, binding, filePath):
        """Writes the current graph to disk.

        Arguments:
            binding: The graph binding to save to disk.
            filePath (str): The file path to which the binding is saved.

        """

        graph = binding.getExec()

        graph.setMetadata("timeline_start", str(self.timeLine.getRangeStart()),
                          False)
        graph.setMetadata("timeline_end", str(self.timeLine.getRangeEnd()),
                          False)
        graph.setMetadata("timeline_current", str(self.timeLine.getTime()),
                          False)
        graph.setMetadata("timeline_loopMode", str(self.timeLine.loopMode()),
                          False)
        graph.setMetadata("timeline_simMode",
                          str(self.timeLine.simulationMode()), False)

        try:
            camera = self.viewport.getCamera()
            mat44 = camera.getMat44('Mat44')
            focalDistance = camera.getFocalDistance('Float32')

            graph.setMetadata("camera_mat44",
                              str(mat44.getJSONStr()), False)
            graph.setMetadata("camera_focalDistance",
                              str(focalDistance.getJSONStr()),
                              False)
        except Exception as e:
            print 'Exception: ' + str(e)
            raise e

        try:
            jsonVal = binding.exportJSON()
            jsonFile = open(filePath, "wb")
            if jsonFile:
                jsonFile.write(jsonVal)
                jsonFile.close()
        except Exception as e:
            print 'Exception: ' + str(e)
            return False

        return True

    def saveGraph(self, saveAs):
        """Wraps the performSave method.

        Opens a dialog for user to save the file if the lastFileName attribute
        is not set or if the saveas argument is True. Otherwise it overwrites
        the file which was opened.

        Arguments:
            saveAs (bool): Whether to save the graph to a different file path.

        """

        self.timeLine.pause()

        filePath = self.lastFileName
        if len(filePath) == 0 or saveAs:
            lastPresetFolder = str(self.settings.value(
                "mainWindow/lastPresetFolder"))
            if len(self.lastFileName) > 0:
                filePath = self.lastFileName
                if filePath.lower().endswith('.canvas'):
                    filePath = filePath[0:-7]
            else:
                filePath = lastPresetFolder

            filePath, _ = QtGui.QFileDialog.getSaveFileName(self, "Save graph",
                                                         filePath, "*.canvas")
            if len(filePath) == 0:
                return False
            if filePath.lower().endswith(".canvas.canvas"):
                filePath = filePath[0:-7]
            elif not filePath.lower().endswith(".canvas"):
                filePath += ".canvas"

        folder = QtCore.QDir(filePath)
        folder.cdUp()
        self.settings.setValue("mainWindow/lastPresetFolder", folder.path())

        binding = self.dfgWidget.getDFGController().getBinding()

        if self.performSave(binding, filePath):
            self.evalContext.currentFilePath = filePath

        self.lastFileName = filePath

        self.onFileNameChanged(filePath)

        self.lastSavedBindingVersion = binding.getVersion()

        return True

    def setBlockCompilations(self, blockCompilations):
        """Sets the graph to block compilations.

        Arguments:
            blockCompilations (bool): Whether to block compilations or not.

        """

        dfgController = self.dfgWidget.getDFGController()
        dfgController.setBlockCompilations(blockCompilations)

    def onFileNameChanged(self, fileName):
        """Callback for when the file name has changed.

        This method updates the window title to reflect the new file path.

        """

        if not fileName:
            self.setWindowTitle(self.windowTitle)
        else:
            self.setWindowTitle(self.windowTitle + " - " + fileName)

    def enableShortCuts(self, enabled):
        """Enables or disables shortcuts.

        enabled (bool): Whether or not to enable the shortcuts.

        """

        if self.newGraphAction:
            self.newGraphAction.blockSignals(enabled)
        if self.loadGraphAction:
            self.loadGraphAction.blockSignals(enabled)
        if self.saveGraphAction:
            self.saveGraphAction.blockSignals(enabled)
        if self.saveGraphAsAction:
            self.saveGraphAsAction.blockSignals(enabled)
        if self.quitAction:
            self.quitAction.blockSignals(enabled)
        if self.manipAction:
            self.manipAction.blockSignals(enabled)
        if self.setGridVisibleAction:
            self.setGridVisibleAction.blockSignals(enabled)
        if self.resetCameraAction:
            self.resetCameraAction.blockSignals(enabled)
        if self.clearLogAction:
            self.clearLogAction.blockSignals(enabled)
        if self.blockCompilationsAction:
            self.blockCompilationsAction.blockSignals(enabled)

    def onAdditionalMenuActionsRequested(self, name, menu, prefix):
        """Callback for when a request to add additional menu actions is called.

        Args:
            name (str): Name of the menu.
            menu (QMenu): Menu item to add to.
            prefix (str): Whether to add the prefixed menu items or not.

        """

        if name == 'File':
            if prefix:
                self.newGraphAction = QtGui.QAction('New Graph', menu)
                self.newGraphAction.setShortcut(QtGui.QKeySequence.New)
                self.loadGraphAction = QtGui.QAction('Load Graph...', menu)
                self.loadGraphAction.setShortcut(QtGui.QKeySequence.Open)
                self.saveGraphAction = QtGui.QAction('Save Graph', menu)
                self.saveGraphAction.setShortcut(QtGui.QKeySequence.Save)
                self.saveGraphAsAction = QtGui.QAction('Save Graph As...', menu)
                self.saveGraphAsAction.setShortcut(QtGui.QKeySequence.SaveAs)

                menu.addAction(self.newGraphAction)
                menu.addAction(self.loadGraphAction)
                menu.addAction(self.saveGraphAction)
                menu.addAction(self.saveGraphAsAction)

                self.newGraphAction.triggered.connect(self.execNewGraph)
                self.loadGraphAction.triggered.connect(self.onLoadGraph)
                self.saveGraphAction.triggered.connect(self.onSaveGraph)
                self.saveGraphAsAction.triggered.connect(self.onSaveGraphAs)
            else:
                menu.addSeparator()
                self.quitAction = QtGui.QAction('Quit', menu)
                self.quitAction.setShortcut(QtGui.QKeySequence.Quit)
                menu.addAction(self.quitAction)

                self.quitAction.triggered.connect(self.close)
        elif name == 'Edit':
            if prefix:
                undoAction = self.qUndoStack.createUndoAction(self)
                undoAction.setShortcut(QtGui.QKeySequence.Undo)
                menu.addAction(undoAction)
                redoAction = self.qUndoStack.createRedoAction(self)
                redoAction.setShortcut(QtGui.QKeySequence.Redo)
                menu.addAction(redoAction)
            else:
                menu.addSeparator()
                self.manipAction = QtGui.QAction(
                    DFG.DFGHotkeys.TOGGLE_MANIPULATION, self.viewport)
                self.manipAction.setShortcut(QtGui.QKeySequence(QtCore.Qt.Key_Q))
                self.manipAction.setShortcutContext(
                    QtCore.Qt.WidgetWithChildrenShortcut)
                self.manipAction.setCheckable(True)
                self.manipAction.setChecked(
                    self.viewport.isManipulationActive())
                self.manipAction.triggered.connect(
                    self.viewport.toggleManipulation)
                self.viewport.addAction(self.manipAction)
                menu.addAction(self.manipAction)
        elif name == 'View':
            if prefix:
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

                self.clearLogAction = QtGui.QAction('&Clear Log Messages', None)
                self.clearLogAction.triggered.connect(self.logWidget.clear)

                self.blockCompilationsAction = QtGui.QAction(
                    '&Block compilations', None)
                self.blockCompilationsAction.setCheckable(True)
                self.blockCompilationsAction.setChecked(False)
                self.blockCompilationsAction.triggered.connect(
                    self.setBlockCompilations)

                menu.addAction(self.setGridVisibleAction)
                menu.addSeparator()
                menu.addAction(self.resetCameraAction)
                menu.addSeparator()
                menu.addAction(self.clearLogAction)
                menu.addSeparator()
                menu.addAction(self.blockCompilationsAction)

    def onHotkeyPressed(self, key, modifiers, hotkey):
        """Callback for when a hot key is pressed.

        Arguments:
            key: Key that is pressed.
            modifiers: Modifiers that were pressed.
            hotkey (str): Hot key constant.

        """

        if hotkey == DFG.DFGHotkeys.EXECUTE:
            self.onDirty()
        elif hotkey == DFG.DFGHotkeys.NEW_SCENE:
            self.execNewGraph()
        elif hotkey == DFG.DFGHotkeys.OPEN_SCENE:
            self.onLoadGraph()
        elif hotkey == DFG.DFGHotkeys.SAVE_SCENE:
            self.saveGraph(False)
        elif hotkey == DFG.DFGHotkeys.TOGGLE_MANIPULATION:
            # Make sure we use the Action path, so menu's "checked" state is updated
            if self.manipAction:
                self.manipAction.trigger()
        else:
            self.dfgWidget.onHotkeyPressed(key, modifiers, hotkey)

    def onGraphSet(self, graph):
        """Callback when the graph is set.

        Args:
            graph: The graph that is being set as current.

        """

        if graph != self.currentGraph:
            graph = self.dfgWidget.getUIGraph()
            graph.defineHotkey(QtCore.Qt.Key_Delete, QtCore.Qt.NoModifier,
                               DFG.DFGHotkeys.DELETE_1)
            graph.defineHotkey(QtCore.Qt.Key_Backspace, QtCore.Qt.NoModifier,
                               DFG.DFGHotkeys.DELETE_2)
            graph.defineHotkey(QtCore.Qt.Key_F5, QtCore.Qt.NoModifier,
                               DFG.DFGHotkeys.EXECUTE)
            graph.defineHotkey(QtCore.Qt.Key_F, QtCore.Qt.NoModifier,
                               DFG.DFGHotkeys.FRAME_SELECTED)
            graph.defineHotkey(QtCore.Qt.Key_A, QtCore.Qt.NoModifier,
                               DFG.DFGHotkeys.FRAME_ALL)
            graph.defineHotkey(QtCore.Qt.Key_Tab, QtCore.Qt.NoModifier,
                               DFG.DFGHotkeys.TAB_SEARCH)
            graph.defineHotkey(QtCore.Qt.Key_A, QtCore.Qt.ControlModifier,
                               DFG.DFGHotkeys.SELECT_ALL)
            graph.defineHotkey(QtCore.Qt.Key_C, QtCore.Qt.ControlModifier,
                               DFG.DFGHotkeys.COPY)
            graph.defineHotkey(QtCore.Qt.Key_V, QtCore.Qt.ControlModifier,
                               DFG.DFGHotkeys.PASTE)
            graph.defineHotkey(QtCore.Qt.Key_X, QtCore.Qt.ControlModifier,
                               DFG.DFGHotkeys.CUT)
            graph.defineHotkey(QtCore.Qt.Key_N, QtCore.Qt.ControlModifier,
                               DFG.DFGHotkeys.NEW_SCENE)
            graph.defineHotkey(QtCore.Qt.Key_O, QtCore.Qt.ControlModifier,
                               DFG.DFGHotkeys.OPEN_SCENE)
            graph.defineHotkey(QtCore.Qt.Key_S, QtCore.Qt.ControlModifier,
                               DFG.DFGHotkeys.SAVE_SCENE)
            graph.defineHotkey(QtCore.Qt.Key_F2, QtCore.Qt.NoModifier,
                               DFG.DFGHotkeys.EDIT_PROPERTIES)
            graph.defineHotkey(QtCore.Qt.Key_R, QtCore.Qt.ControlModifier,
                               DFG.DFGHotkeys.RELAX_NODES)
            graph.defineHotkey(QtCore.Qt.Key_Q, QtCore.Qt.NoModifier,
                               DFG.DFGHotkeys.TOGGLE_MANIPULATION)
            graph.defineHotkey(QtCore.Qt.Key_0, QtCore.Qt.ControlModifier,
                               DFG.DFGHotkeys.RESET_ZOOM)
            graph.defineHotkey(QtCore.Qt.Key_1, QtCore.Qt.NoModifier,
                               DFG.DFGHotkeys.COLLAPSE_LEVEL_1)
            graph.defineHotkey(QtCore.Qt.Key_2, QtCore.Qt.NoModifier,
                               DFG.DFGHotkeys.COLLAPSE_LEVEL_2)
            graph.defineHotkey(QtCore.Qt.Key_3, QtCore.Qt.NoModifier,
                               DFG.DFGHotkeys.COLLAPSE_LEVEL_3)

            graph.nodeEditRequested.connect(self.onNodeEditRequested)

            self.currentGraph = graph

    def dragEnterEvent(self, event):
        # we accept the proposed action only if we
        # are dealing with a single '.canvas' file.
        if event.mimeData().hasUrls():
            urls = event.mimeData().urls()
            if len(urls) == 1:
                url = urls[0];
                filename = FabricUI.Util.GetFilenameForFileURL(url)
                if filename.endswith(".canvas"):
                    event.acceptProposedAction()
                    return

        DFG.DFGMainWindow.dragEnterEvent(self, event)

    def dropEvent(self, event):
        # The mimeData was already checked in the dragEnterEvent(), so we simply get the filepath and load the graph.
        # We also check if the Control key is down and, if it is, we load the scene without prompting.
        url = event.mimeData().urls()[0]
        event.acceptProposedAction()

        bypassUnsavedChanges = event.keyboardModifiers() & QtCore.Qt.ControlModifier
        self.onUrlDropped(url, bypassUnsavedChanges)

    def onUrlDropped(self, url, bypassUnsavedChanges):
        filename = FabricUI.Util.GetFilenameForFileURL(url)
        if not filename:
            return

        self.timeLine.pause()

        if not (bypassUnsavedChanges or self.checkUnsavedChanges()):
            return

        self.loadGraph(filename)
