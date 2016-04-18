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
    
    defaultFrameIn = 1
    defaultFrameOut = 50
    autosaveIntervalSecs = 30

    def __init__(self, settings, unguarded):
        self.settings = settings

        super(CanvasWindow, self).__init__()

        self.autosaveTimer = QtCore.QTimer()
        self.autosaveTimer.timeout.connect(self.autosave)
        self.autosaveTimer.start(CanvasWindow.autosaveIntervalSecs * 1000)
        self.dockFeatures = QtGui.QDockWidget.DockWidgetMovable | QtGui.QDockWidget.DockWidgetFloatable | QtGui.QDockWidget.DockWidgetClosable

        self.__init()
        self._initWindow()
        self._initKL(unguarded)
        self._initLog()
        self._initDFG()
        self._initTreeView()
        self._initGL()
        self._initValueEditor()
        self._initTimeLine()
        self._initDocksAndMenus()

        self.restoreGeometry(self.settings.value("mainWindow/geometry"))
        self.restoreState(self.settings.value("mainWindow/state"))
        self.onFrameChanged(self.timeLine.getTime())
        self.onGraphSet(self.dfgWidget.getUIGraph())
        self.valueEditor.initConnections()
        self.installEventFilter(CanvasWindowEventFilter(self))

    def __init(self):
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
        if self.dfgWidget:
            self.dfgWidget.getDFGController().log(line)
        else:
            if source == Core.ReportSource.User or 'Ignoring' in line:
                sys.stdout.write(line + "\n")
            else:
                sys.stderr.write(line + "\n")

    def __statusCallback(self, target, data):
        if target == "licensing":
            try:
                FabricUI.HandleLicenseData(self, self.client, data, True)
            except Exception as e:
                self.dfgWidget.getDFGController().logError(str(e))

    def _initKL(self, unguarded):
        clientOpts = {
          'guarded': not unguarded,
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
        self.scriptEditor = ScriptEditor(self.client, binding, self.qUndoStack, self.logWidget)
        self.dfguiCommandHandler = UICmdHandler(self.client, self.scriptEditor)
       
        self.dfgWidget = DFG.DFGWidget(None, self.client, self.host,
                                       binding, '', graph, astManager,
                                       self.dfguiCommandHandler, self.config)

        tabSearchWidget = self.dfgWidget.getTabSearchWidget()
        tabSearchWidget.enabled.connect(self.enableShortCuts)

        self.dfgWidget.onGraphSet.connect(self.onGraphSet)
        self.dfgWidget.additionalMenuActionsRequested.connect(self.onAdditionalMenuActionsRequested)

    def _initTreeView(self):
        controller = self.dfgWidget.getDFGController()
        self.treeWidget = DFG.PresetTreeWidget(controller, self.config, True, False, False, False, False, True)
        self.dfgWidget.newPresetSaved.connect(self.treeWidget.refresh)
        controller.varsChanged.connect(self.treeWidget.refresh)
        controller.dirty.connect(self.onDirty)

    def _initGL(self):
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
        self.valueEditor = FabricUI.DFG.DFGVEEditorOwner(self.dfgWidget)
     
    def _initLog(self):
        self.logWidget = DFG.DFGLogWidget(self.config)
        self.qUndoView = QtGui.QUndoView(self.qUndoStack)
        self.qUndoView.setEmptyLabel("New Graph")

    def _initTimeLine(self):
        self.timeLinePortIndex = -1
        self.timeLinePortPath = None
        self.timeLine = TimeLine.TimeLineWidget()
        self.timeLine.setTimeRange(CanvasWindow.defaultFrameIn, CanvasWindow.defaultFrameOut)
        self.timeLine.updateTime(1)
        self.timeLine.frameChanged.connect(self.onFrameChanged)
        self.timeLine.frameChanged.connect(self.valueEditor.onFrameChanged)

    def _initDocksAndMenus(self):

        undoDockWidget = QtGui.QDockWidget("History", self)
        undoDockWidget.setObjectName("History")
        undoDockWidget.setFeatures(self.dockFeatures)
        undoDockWidget.setWidget(self.qUndoView)
        undoDockWidget.hide()     
        self.addDockWidget(QtCore.Qt.LeftDockWidgetArea, undoDockWidget)

        logDockWidget = QtGui.QDockWidget("Log Messages", self)
        logDockWidget.setObjectName("Log")
        logDockWidget.setFeatures(self.dockFeatures)
        logDockWidget.setWidget(self.logWidget)
        logDockWidget.hide()
        self.addDockWidget(QtCore.Qt.TopDockWidgetArea, logDockWidget, QtCore.Qt.Vertical)

        valueEditorDockWidget = QtGui.QDockWidget("Value Editor", self)
        valueEditorDockWidget.setObjectName("Values")
        valueEditorDockWidget.setFeatures(self.dockFeatures)
        valueEditorDockWidget.setWidget(self.valueEditor.getWidget())
        self.addDockWidget(QtCore.Qt.RightDockWidgetArea, valueEditorDockWidget)

        dfgDock = QtGui.QDockWidget('Canvas Graph', self)
        dfgDock.setObjectName('Canvas Graph')
        dfgDock.setFeatures(self.dockFeatures)
        dfgDock.setWidget(self.dfgWidget)
        self.addDockWidget(QtCore.Qt.BottomDockWidgetArea, dfgDock, QtCore.Qt.Vertical)

        treeDock = QtGui.QDockWidget("Explorer", self)
        treeDock.setObjectName("Explorer")
        treeDock.setFeatures(self.dockFeatures)
        treeDock.setWidget(self.treeWidget)
        self.addDockWidget(QtCore.Qt.LeftDockWidgetArea, treeDock)

        self.timeLineDock = QtGui.QDockWidget("TimeLine", self)
        self.timeLineDock.setObjectName("TimeLine")
        self.timeLineDock.setFeatures(self.dockFeatures)
        self.timeLineDock.setWidget(self.timeLine)
        self.addDockWidget(QtCore.Qt.BottomDockWidgetArea, self.timeLineDock, QtCore.Qt.Vertical)

        scriptEditorDock = QtGui.QDockWidget('Script Editor', self)
        scriptEditorDock.setObjectName('Script Editor')
        scriptEditorDock.setFeatures(self.dockFeatures)
        scriptEditorDock.setWidget(self.scriptEditor)
        self.addDockWidget(QtCore.Qt.BottomDockWidgetArea, scriptEditorDock, QtCore.Qt.Vertical)

        self.dfgWidget.populateMenuBar(self.menuBar())
        windowMenu = self.menuBar().addMenu("&Window")

        toggleAction = dfgDock.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_1)
        windowMenu.addAction(toggleAction)
        toggleAction = treeDock.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_2)
        windowMenu.addAction(toggleAction)
        toggleAction = valueEditorDockWidget.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_6)
        windowMenu.addAction(toggleAction)
        toggleAction = self.timeLineDock.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_4)
        windowMenu.addAction(toggleAction)
        windowMenu.addSeparator()
        toggleAction = undoDockWidget.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_5)
        windowMenu.addAction(toggleAction)
        toggleAction = logDockWidget.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_6)
        windowMenu.addAction(toggleAction)
        toggleAction = scriptEditorDock.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_7)
        windowMenu.addAction(toggleAction)
    
    def _contentChanged(self) :
        self.valueEditor.onOutputsChanged()
        self.viewport.redraw()

    def onPortManipulationRequested(self, portName):
        try:
            controller = self.dfgWidget.getDFGController()
            binding = controller.getBinding()
            dfgExec = binding.getExec()
            portResolvedType = dfgExec.getExecPortResolvedType(portName)
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
        self.dfgWidget.getDFGController().execute()
        self._contentChanged()

    def loadGraph(self, filePath):
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
            dfgController.execute()

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
        self.saveGraph(False)

    def onSaveGraphAs(self):
        self.saveGraph(True)

    def closeEvent(self, event):
        if not self.checkUnsavedChanges():
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
        self.dfgWidget.maybeEditNode(node)

    def onFrameChanged(self, frame):
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
        if not self.viewport:
            return

        caption = str(round(self.viewport.fps(), 2)) + " FPS"
        self.fpsLabel.setText(caption)

    def autosave(self):
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

    def onNewGraph(self, skip_save=False):
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
        dfgController = self.dfgWidget.getDFGController()
        dfgController.setBlockCompilations(blockCompilations)

    def onFileNameChanged(self, fileName):
        if not fileName:
            self.setWindowTitle(self.windowTitle)
        else:
            self.setWindowTitle(self.windowTitle + " - " + fileName)

    def enableShortCuts(self, enabled):
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

                self.newGraphAction.triggered.connect(self.onNewGraph)
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
                undoAction = QtGui.QAction("Undo", self)
                def onUndo():
                    self.scriptEditor.eval("undo()")
                undoAction.triggered.connect(onUndo)
                undoAction.setEnabled(self.qUndoStack.canUndo())
                self.qUndoStack.canUndoChanged.connect(undoAction.setEnabled)
                undoAction.setShortcut(QtGui.QKeySequence.Undo)
                menu.addAction(undoAction)
                redoAction = QtGui.QAction("Redo", self)
                def onRedo():
                    self.scriptEditor.eval("redo()")
                redoAction.triggered.connect(onRedo)
                redoAction.setEnabled(self.qUndoStack.canRedo())
                self.qUndoStack.canRedoChanged.connect(redoAction.setEnabled)
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
        if hotkey == DFG.DFGHotkeys.EXECUTE:
            self.onDirty()
        elif hotkey == DFG.DFGHotkeys.NEW_SCENE:
            self.onNewGraph()
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
