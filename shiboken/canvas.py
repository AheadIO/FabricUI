import optparse, os, sys
import FabricUI
from FabricUI import DFG, Style, Viewports
import FabricEngine.Core as Core
from PySide import QtCore, QtGui, QtOpenGL

def reportCallback(source, level, line):
  #DFG.DFGLogWidget.callback
  if source == Core.ReportSource.User or level == Core.ReportLevel.Error or 'Ignoring' in line:
    sys.stdout.write(line+"\n")
  else:
    sys.stderr.write(line+"\n")

class MainWindow(DFG.DFGMainWindow):
    contentChanged = QtCore.Signal()

    defaultFrameIn = 1
    defaultFrameOut = 50
    autosaveIntervalSecs = 30

    def __init__(self, settings, unguarded):
        super(MainWindow, self).__init__()

        self.settings = settings
        DFG.DFGWidget.setSettings(settings)

        self.currentGraph = None

        self.config = DFG.DFGConfig()

        self.autosaveFilename = os.path.join(fabricDir, 'autosave')
        if not os.path.exists(self.autosaveFilename):
            os.makedirs(self.autosaveFilename)
        autosaveBasename = 'autosave.'+str(os.getpid())+'.canvas'
        self.autosaveFilename = os.path.join(self.autosaveFilename, autosaveBasename)
        print 'Will autosave to '+self.autosaveFilename+' every '+str(MainWindow.autosaveIntervalSecs)+' seconds'

        autosaveTimer = QtCore.QTimer()
        autosaveTimer.timeout.connect(self.autosave)
        autosaveTimer.start(MainWindow.autosaveIntervalSecs*1000)

        statusBar = QtGui.QStatusBar(self)
        self.fpsLabel = QtGui.QLabel(statusBar)
        statusBar.addPermanentWidget(self.fpsLabel)
        self.setStatusBar(statusBar)
        statusBar.show()

        fpsTimer = QtCore.QTimer()
        fpsTimer.setInterval(1000)
        fpsTimer.timeout.connect(self.updateFPS)
        fpsTimer.start()

        client = Core.createClient({'unguarded': unguarded, 'reportCallback': reportCallback})
        #options.licenseType = FabricCore::ClientLicenseType_Interactive
        #options.rtValToJSONEncoder = &sRTValEncoder
        #options.rtValFromJSONDecoder = &sRTValDecoder
        client.loadExtension('Math')
        client.loadExtension('Parameters')
        client.loadExtension('Util')
        client.setStatusCallback(self.statusCallback)
        self.client = client

        self.qUndoStack = QtGui.QUndoStack()
        dfguiCommandHandler = DFG.DFGUICmdHandler_QUndo(self.qUndoStack)

        astManager = FabricUI.KLASTManager(client)

        self.evalContext = client.RT.types.EvalContext.create()
        self.evalContext = self.evalContext.getInstance('EvalContext')
        self.evalContext.host = 'Canvas'
        self.evalContext.graph = ''

        self.host = client.getDFGHost()
        binding = self.host.createBindingToNewGraph()
        self.lastSavedBindingVersion = binding.getVersion()
        self.lastAutosaveBindingVersion = self.lastSavedBindingVersion

        graph = binding.getExec()

        glFormat = QtOpenGL.QGLFormat()
        glFormat.setDoubleBuffer(True)
        glFormat.setDepth(True)
        glFormat.setAlpha(True)
        glFormat.setSampleBuffers(True)
        glFormat.setSamples(4)

        self.viewport = Viewports.GLViewportWidget(client, self.config.defaultWindowColor, glFormat, None, None)
        self.setCentralWidget(self.viewport)

        self.dfgWidget = DFG.DFGWidget(None, client._client, self.host, binding, '', graph, astManager, dfguiCommandHandler, self.config)

        self.contentChanged.connect(self.viewport.redraw)
        self.viewport.portManipulationRequested.connect(self.onPortManipulationRequested)

        dockFeatures = QtGui.QDockWidget.DockWidgetMovable | QtGui.QDockWidget.DockWidgetFloatable | QtGui.QDockWidget.DockWidgetClosable

        dfgDock = QtGui.QDockWidget('Canvas Graph', self)
        dfgDock.setObjectName('Canvas Graph')
        dfgDock.setFeatures(dockFeatures)
        dfgDock.setWidget(self.dfgWidget)
        self.addDockWidget(QtCore.Qt.BottomDockWidgetArea, dfgDock, QtCore.Qt.Vertical)

        self.timeLinePortIndex = 0
        self.timeLine = Viewports.TimeLineWidget()
        self.timeLine.setTimeRange(self.defaultFrameIn, self.defaultFrameOut)
        self.timeLine.updateTime(1)
        timeLineDock = QtGui.QDockWidget("TimeLine", self)
        timeLineDock.setObjectName("TimeLine")
        timeLineDock.setFeatures(dockFeatures)
        timeLineDock.setWidget(self.timeLine)
        self.addDockWidget(QtCore.Qt.BottomDockWidgetArea, timeLineDock, QtCore.Qt.Vertical)

        treeWidget = DFG.PresetTreeWidget(self.dfgWidget.getDFGController(), self.config, True, False, True)
        treeDock = QtGui.QDockWidget("Explorer", self)
        treeDock.setObjectName("Explorer")
        treeDock.setFeatures(dockFeatures)
        treeDock.setWidget(treeWidget)
        self.addDockWidget(QtCore.Qt.LeftDockWidgetArea, treeDock)

        self.dfgWidget.newPresetSaved.connect(treeWidget.refresh)

        self.dfgValueEditor = DFG.DFGValueEditor(self.dfgWidget.getDFGController(), self.config)
        dfgValueEditorDockWidget = QtGui.QDockWidget("Value Editor", self)
        dfgValueEditorDockWidget.setObjectName("Values")
        dfgValueEditorDockWidget.setFeatures(dockFeatures)
        dfgValueEditorDockWidget.setWidget(self.dfgValueEditor)
        self.addDockWidget(QtCore.Qt.RightDockWidgetArea, dfgValueEditorDockWidget)

        logWidget = DFG.DFGLogWidget(self.config)
        logDockWidget = QtGui.QDockWidget("Log Messages", self)
        logDockWidget.setObjectName("Log")
        logDockWidget.setFeatures(dockFeatures)
        logDockWidget.setWidget(logWidget)
        logDockWidget.hide()
        self.addDockWidget(QtCore.Qt.TopDockWidgetArea, logDockWidget, QtCore.Qt.Vertical)

        self.qUndoView = QtGui.QUndoView(self.qUndoStack)
        self.qUndoView.setEmptyLabel("New Graph")
        undoDockWidget = QtGui.QDockWidget("History", self)
        undoDockWidget.setObjectName("History")
        undoDockWidget.setFeatures(dockFeatures)
        undoDockWidget.setWidget(self.qUndoView)
        undoDockWidget.hide()
        self.addDockWidget(QtCore.Qt.LeftDockWidgetArea, undoDockWidget)

        controller = self.dfgWidget.getDFGController()
        controller.varsChanged.connect(treeWidget.refresh)
        controller.argsChanged.connect(self.onStructureChanged)
        controller.argValuesChanged.connect(self.onValueChanged)
        controller.defaultValuesChanged.connect(self.onValueChanged)
        self.dfgWidget.nodeInspectRequested.connect(self.onNodeInspectRequested)
        controller.dirty.connect(self.onDirty)
        controller.bindingChanged.connect(self.dfgValueEditor.setBinding)
        controller.nodeRemoved.connect(self.dfgValueEditor.onNodeRemoved)
        #self.dfgWidget.getTabSearchWidget().enabled.connect(self.enableShortCuts)
        self.timeLine.frameChanged.connect(self.onFrameChanged)
        #self.dfgWidget.onGraphSet.connect(self.onGraphSet)

        self.restoreGeometry(settings.value("mainWindow/geometry"))
        self.restoreState(settings.value("mainWindow/state"))

        #self.dfgWidget.additionalMenuActionsRequested.connect(self.onAdditionalMenuActionsRequested)
        
        self.dfgWidget.populateMenuBar(self.menuBar())
        windowMenu = self.menuBar().addMenu("&Window")

        toggleAction = dfgDock.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL+QtCore.Qt.Key_4)
        windowMenu.addAction(toggleAction)
        toggleAction = treeDock.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL+QtCore.Qt.Key_5)
        windowMenu.addAction(toggleAction)
        toggleAction = dfgValueEditorDockWidget.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL+QtCore.Qt.Key_6)
        windowMenu.addAction(toggleAction)
        toggleAction = timeLineDock.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL+QtCore.Qt.Key_9)
        windowMenu.addAction(toggleAction)
        windowMenu.addSeparator()
        toggleAction = undoDockWidget.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL+QtCore.Qt.Key_7)
        windowMenu.addAction(toggleAction)
        toggleAction = logDockWidget.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL+QtCore.Qt.Key_8)
        windowMenu.addAction(toggleAction)

        self.onFrameChanged(self.timeLine.getTime())
        #self.onGraphSet(self.dfgWidget.getUIGraph())
        #self.onSidePanelInspectRequested()

    def statusCallback(self, target, data):
        if target == "licensing":
            try:
                pass
                #FabricUI_HandleLicenseData(self, self.client, data, True)
            except Exception as e:
                self.dfgWidget.getDFGController().logError(str(e))

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
                message = "Port '"+portName
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
        self.onValueChanged()
        self.contentChanged.emit()

    def onValueChanged(self):
        try:
            self.dfgValueEditor.updateOutputs()
        except Exception as e:
            self.dfgWidget.getDFGController().logError(str(e))

    def loadGraph(self, filePath):
        self.timeLine.pause()
        self.timeLinePortIndex = -1

        try:
            dfgController = self.dfgWidget.getDFGController()
            binding = dfgController.getBinding()
            binding.deallocValues()

            self.dfgValueEditor.clear()
            self.host.flushUndoRedo()
            self.qUndoStack.clear()
            self.qUndoView.setEmptyLabel("Load Graph")
            self.viewport.clearInlineDrawing()

            QtCore.QCoreApplication.processEvents()

            json = open(filePath, 'rb').read()
            binding = self.host.createBindingFromJSON(json)
            self.lastSavedBindingVersion = binding.getVersion()
            dfgExec = binding.getExec()
            dfgController.setBindingExec(binding, "", dfgExec)
            self.onSidePanelInspectRequested()

            dfgController.checkErrors()
            self.evalContext.currentFilePath = filePath
            dfgController.bindUnboundRTVals()
            dfgController.execute()

            tl_start = dfgExec.getMetadata("timeline_start")
            tl_end = dfgExec.getMetadata("timeline_end")
            tl_loopMode = dfgExec.getMetadata("timeline_loopMode")
            tl_simulationMode = dfgExec.getMetadata("timeline_simMode")
            tl_current = dfgExec.getMetadata("timeline_current")

            if len(tl_start) > 0 and len(tl_end) > 0:
                self.timeLine.setTimeRange(int(tl_start), int(tl_end))
            else:
                self.timeLine.setTimeRange(self.defaultFrameIn, self.defaultFrameOut)

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
                    mat44 = self.client.RT.types.Mat44(camera_mat44)
                    focalDistance = self.client.RT.types.Float32(camera_focalDistance)

                    camera = self.viewport.getCamera()

                    camera.setFromMat44("", mat44)
                    camera.setFocalDistance("", focalDistance)
                except Exception as e:
                    sys.stderr.write("Exception: " + str(e) + "\n")

            self.contentChanged.emit()
            self.onStructureChanged()

            self.onFileNameChanged(filePath)

            # then set it to the current value if we still have it.
            # this will ensure that sim mode scenes will play correctly.
            if len(tl_current) > 0:
                self.timeLine.updateTime(int(tl_current), True)
            else:
                self.timeLine.updateTime(self.defaultFrameIn, True)

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
        self.settings.setValue("mainWindow/geometry", saveGeometry())
        self.settings.setValue("mainWindow/state", saveState())

        QMainWindow.closeEvent(self, event)

        #FTL::FSMaybeDeleteFile(self.autosaveFilename)

    def checkUnsavedChanges(self):
        binding = dfgWidget.getDFGController().getBinding()

        if binding.getVersion() != self.lastSavedBindingVersion:
            msgBox = QtGui.QMessageBox()
            msgBox.setText( "Do you want to save your changes?" )
            msgBox.setInformativeText( "Your changes will be lost if you don't save them." )
            msgBox.setStandardButtons( QtGui.QMessageBox.Save | QtGui.QMessageBox.Discard | QtGui.QMessageBox.Cancel )
            msgBox.setDefaultButton( QtGui.QMessageBox.Save )
            result = msgBox.exec_()
            if result == QtGui.QMessageBox.Discard:
                return True
            elif result == QtGui.QMessageBox.Cancel:
                return False
            else:
                return self.saveGraph(False)
        return True

    def onStructureChanged(self):
        if self.dfgWidget.getDFGController().isViewingRootGraph():
            self.timeLinePortIndex = -1
            try:
                graph = self.dfgWidget.getDFGController().getExec()
                portCount = graph.getExecPortCount()
                for i in range(0, portCount):
                    if graph.getExecPortType(i) == self.client.DFG.PortTypes.Out:
                        continue
                    portName = graph.getExecPortName(i)
                    if portName != "timeline":
                        continue
                    if not graph.isExecPortResolvedType(i, "SInt32") and not graph.isExecPortResolvedType(i, "UInt32") and not graph.isExecPortResolvedType(i, "Float32") and not graph.isExecPortResolvedType(i, "Float64"):
                        continue
                    self.timeLinePortIndex = i
                    break
            except Exception as e:
                self.dfgWidget.getDFGController().logError(str(e))

    def onNodeInspectRequested(self, node):
        if node.isBackDropNode():
            return

        dfgController = self.dfgWidget.getDFGController()
        self.dfgValueEditor.setNode(
            dfgController.getBinding(),
            dfgController.getExecPath(),
            dfgController.getExec(),
            node.name()
            )

    def onNodeEditRequested(self, node):
        self.dfgWidget.maybeEditNode(node)

    def onFrameChanged(self, frame):
        try:
            self.evalContext.time = frame
        except Exception as e:
            self.dfgWidget.getDFGController().logError(str(e))

        if self.timeLinePortIndex == -1:
            return

        try:
            binding = self.dfgWidget.getDFGController().getBinding()
            dfgExec = binding.getExec()
            if dfgExec.isExecPortResolvedType( self.timeLinePortIndex, "SInt32" ):
                binding.setArgValue(
                    self.timeLinePortIndex,
                    self.client.RT.types.SInt32(frame),
                    False
                    )
            elif dfgExec.isExecPortResolvedType( self.timeLinePortIndex, "UInt32" ):
                binding.setArgValue(
                    self.timeLinePortIndex,
                    self.client.RT.types.UInt32(frame),
                    False
                    )
            elif dfgExec.isExecPortResolvedType( self.timeLinePortIndex, "Float32" ):
                binding.setArgValue(
                    self.timeLinePortIndex,
                    self.client.RT.types.Float32(frame),
                    False
                    )
            elif dfgExec.isExecPortResolvedType( self.timeLinePortIndex, "Float64" ):
                binding.setArgValue(
                    self.timeLinePortIndex,
                    self.client.RT.types.Float64(frame),
                    False
                    )
        except Exception as e:
            self.dfgWidget.getDFGController().logError(str(e))

    def updateFPS(self):
        if not self.viewport:
            return

        caption = str(self.viewport.fps()) + " FPS"
        self.fpsLabel.setText( caption )

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
                    #FTL::FSMaybeMoveFile( tmpAutosaveFilename, self.autosaveFilename )
                    self.lastAutosaveBindingVersion = bindingVersion

    def onSidePanelInspectRequested(self):
        dfgController = self.dfgWidget.getDFGController()

        if dfgController.isViewingRootGraph():
            self.dfgValueEditor.setBinding(dfgController.getBinding())
        else:
            self.dfgValueEditor.clear()

    def onNewGraph(self):
        self.timeLine.pause()

        if not checkUnsavedChanged():
            return

        self.lastFileName = ""

        try:
            dfgController = self.dfgWidget.getDFGController()

            binding = dfgController.getBinding()
            binding.deallocValues()

            self.dfgValueEditor.clear()

            self.host.flushUndoRedo()
            self.qUndoStack.clear()
            self.viewport.clearInlineDrawing()
            QtCore.QCoreApplication.processEvents()

            # Note: the previous binding is no longer functional
            #             create the new one before resetting the timeline options

            binding = self.host.createBindingToNewGraph()
            self.lastSavedBindingVersion = binding.getVersion()
            dfgExec = binding.getExec()
            self.timeLinePortIndex = -1

            dfgController.setBindingExec(binding, '', dfgExec)

            self.timeLine.setTimeRange(defaultFrameIn, defaultFrameOut)
            self.timeLine.setLoopMode(1)
            self.timeLine.setSimulationMode(0)
            self.timeLine.updateTime(defaultFrameIn, True)

            QtCore.QCoreApplication.processEvents()
            self.qUndoView.setEmptyLabel("New Graph")

            self.onSidePanelInspectRequested()

            self.contentChanged.emit()
            self.onStructureChanged()

            self.onFileNameChanged('')

            self.viewport.update()

        except Exception as e:
            print 'Exception: '+str(e)

    def onLoadGraph(self):
        self.timeLine.pause()

        if not checkUnsavedChanged():
            return

        lastPresetFolder = self.settings.value("mainWindow/lastPresetFolder").toString()
        filePath = QtGui.QFileDialog.getOpenFileName(this, "Load graph", lastPresetFolder, "*.canvas")
        if len(filePath.length):
            folder  = QtCore.QDir(filePath)
            folder.cdUp()
            self.settings.setValue( "mainWindow/lastPresetFolder", folder.path() )
            loadGraph(filePath)

    def performSave(self, binding, filePath):
        graph = binding.getExec()

        '''
        QString num
        num.setNum(self.timeLine.getRangeStart())
        graph.setMetadata("timeline_start", num.toUtf8().constData(), False)
        num.setNum(self.timeLine.getRangeEnd())
        graph.setMetadata("timeline_end", num.toUtf8().constData(), False)
        num.setNum(self.timeLine.getTime())
        graph.setMetadata("timeline_current", num.toUtf8().constData(), False)
        num.setNum(self.timeLine.loopMode())
        graph.setMetadata("timeline_loopMode", num.toUtf8().constData(), False)
        num.setNum(self.timeLine.simulationMode())
        graph.setMetadata("timeline_simMode", num.toUtf8().constData(), False)
        '''
        try:
            camera = self.viewport.getCamera()
            mat44 = camera.getMat44('Mat44')
            focalDistance = camera.getFocalDistance('Float32')

            if mat44.isValid() and focalDistance.isValid():
                graph.setMetadata("camera_mat44", mat44.getJSON().getStringCString(), False)
                graph.setMetadata("camera_focalDistance", focalDistance.getJSON().getStringCString(), False)
        except Exception as e:
            print 'Exception: '+str(e)

        try:
            json = binding.exportJSON()
            jsonFile = open(filePath, "wb")
            if jsonFile:
                jsonFile.write(json)
                jsonFile.close()
        except Exception as e:
            print 'Exception: '+str(e)
            return False

        return True

    def saveGraph(self, saveAs):
        self.timeLine.pause()

        filePath = self.lastFileName
        if len(filePath) == 0 or saveAs:
            lastPresetFolder = self.settings.value("mainWindow/lastPresetFolder").toString()
            if len(self.lastFileName) > 0:
                filePath = self.lastFileName
                if filePath.toLower().endsWith(".canvas"):
                    filePath = filePath.left(filePath.length() - 7)
            else:
                filePath = lastPresetFolder

            filePath = QtGui.QFileDialog.getSaveFileName(self, "Save graph", filePath, "*.canvas")
            if len(filePath) == 0:
                return False
            if filePath.toLower().endsWith(".canvas.canvas"):
                filePath = filePath.left(filePath.length() - 7)
            elif not filePath.toLower().endsWith(".canvas"):
                filePath += ".canvas"

        folder = QtCore.QDir(filePath)
        folder.cdUp()
        self.settings.setValue("mainWindow/lastPresetFolder", folder.path())

        binding = self.dfgWidget.getDFGController().getBinding()

        if self.performSave(binding, filePath):
            self.evalContext.setMember("currentFilePath", self.client.RT.types.String(filePath))

        self.lastFileName = filePath

        self.onFileNameChanged(filePath)

        self.lastSavedBindingVersion = binding.getVersion()

        return True

    def setBlockCompilations(self, blockCompilations):
        dfgController = self.dfgWidget.getDFGController()
        dfgController.setBlockCompilations(blockCompilations)

    def onFileNameChanged(self, fileName):
        if fileName.isEmpty():
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
        #if self.setUsingStageAction:
        #    self.setUsingStageAction.blockSignals(enabled)
        if self.resetCameraAction:
            self.resetCameraAction.blockSignals(enabled)
        if self.clearLogAction:
            self.clearLogAction.blockSignals(enabled)
        if self.blockCompilationsAction:
            self.blockCompilationsAction.blockSignals(enabled)
     
    def onAdditionalMenuActionsRequested(self, name, menu, prefix):
        if name == "File":
            if prefix:
                self.newGraphAction = menu.addAction("New Graph")
                self.newGraphAction.setShortcut(QtGui.QKeySequence.New)
                self.loadGraphAction = menu.addAction("Load Graph...")
                self.loadGraphAction.setShortcut(QtGui.QKeySequence.Open)
                self.saveGraphAction = menu.addAction("Save Graph")
                self.saveGraphAction.setShortcut(QtGui.QKeySequence.Save)
                self.saveGraphAsAction = menu.addAction("Save Graph As...")
                self.saveGraphAsAction.setShortcut(QtGui.QKeySequence.SaveAs)

                self.newGraphAction.triggered.connect(self.onNewGraph)
                self.loadGraphAction.triggered.connect(self.onLoadGraph)
                self.saveGraphAction.triggered.connect(self.onSaveGraph)
                self.saveGraphAsAction.triggered.connect(self.onSaveGraphAs)
            else:
                menu.addSeparator()
                self.quitAction = menu.addAction("Quit")
                self.quitAction.setShortcut(QtGui.QKeySequence.Quit)
               
                self.quitAction.triggered.connect(self.close)
        elif name == "Edit":
            if prefix:
                undoAction = self.qUndoStack.createUndoAction(menu)
                undoAction.setShortcut(QtGui.QKeySequence.Undo)
                menu.addAction(undoAction)
                redoAction = self.qUndoStack.createRedoAction(menu)
                redoAction.setShortcut(QtGui.QKeySequence.Redo)
                menu.addAction(redoAction)
            else:
                menu.addSeparator()
                self.manipAction = QtGui.QAction(DFG_TOGGLE_MANIPULATION, self.viewport)
                self.manipAction.setShortcut(QtCore.Qt.Key_Q)
                self.manipAction.setShortcutContext(QtCore.Qt.WidgetWithChildrenShortcut)
                self.manipAction.setCheckable( True )
                self.manipAction.setChecked( self.viewport.isManipulationActive() )
                self.manipAction.triggered.connect(self.viewport.toggleManipulation)
                self.viewport.addAction( self.manipAction )
                menu.addAction( self.manipAction )
        elif name == "View":
            if prefix:
                self.setGridVisibleAction = QtGui.QAction("&Display Grid", 0)
                self.setGridVisibleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_G)
                self.setGridVisibleAction.setCheckable( True )
                self.setGridVisibleAction.setChecked( self.viewport.isStageVisible() )
                self.setGridVisibleAction.toggled.connect(self.viewport.setStageVisible)

                self.resetCameraAction = QtGui.QAction("&Reset Camera", self.viewport)
                self.resetCameraAction.setShortcut(QtCore.Qt.Key_R)
                self.resetCameraAction.setShortcutContext(QtCore.Qt.WidgetWithChildrenShortcut)
                self.resetCameraAction.triggered.connect(self.viewport.resetCamera)
                self.viewport.addAction(self.resetCameraAction)

                self.clearLogAction = QtGui.QAction("&Clear Log Messages", 0)
                self.clearLogAction.triggered.connect(self.logWidget.clear)

                self.blockCompilationsAction = QtGui.QAction( "&Block compilations", 0 )
                self.blockCompilationsAction.setCheckable( True )
                self.blockCompilationsAction.setChecked( False )
                self.blockCompilationsAction.triggered.connect(self.setBlockCompilations)

                # [Julien] FE-4965
                menu.addAction( self.setGridVisibleAction )
                menu.addSeparator()
                menu.addAction( self.resetCameraAction )
                menu.addSeparator()
                menu.addAction( self.clearLogAction )
                menu.addSeparator()
                menu.addAction( self.blockCompilationsAction )

    '''
    def onHotkeyPressed(self, key, modifiers, hotkey):
        if hotkey == DFG_EXECUTE:
            self.onDirty()
        elif hotkey == DFG_NEW_SCENE):
            self.onNewGraph()
        elif hotkey == DFG_OPEN_SCENE):
            self.onLoadGraph()
        elif hotkey == DFG_SAVE_SCENE):
            self.saveGraph(False)
        elif hotkey == DFG_TOGGLE_MANIPULATION):
            # Make sure we use the Action path, so menu's "checked" state is updated
            if self.manipAction:
                self.manipAction.trigger()
        else:
            self.dfgWidget.onHotkeyPressed(key, modifiers, hotkey)

    def onGraphSet(self, graph):
        if graph != self.currentGraph:
            graph = self.dfgWidget.getUIGraph()
            graph.defineHotkey(QtCore.Qt.Key_Delete, QtCore.Qt.NoModifier, DFG_DELETE)
            graph.defineHotkey(QtCore.Qt.Key_Backspace, QtCore.Qt.NoModifier, DFG_DELETE_2)
            graph.defineHotkey(QtCore.Qt.Key_F5, QtCore.Qt.NoModifier, DFG_EXECUTE)
            graph.defineHotkey(QtCore.Qt.Key_F, QtCore.Qt.NoModifier, DFG_FRAME_SELECTED)
            graph.defineHotkey(QtCore.Qt.Key_A, QtCore.Qt.NoModifier, DFG_FRAME_ALL)
            graph.defineHotkey(QtCore.Qt.Key_Tab, QtCore.Qt.NoModifier, DFG_TAB_SEARCH)
            graph.defineHotkey(QtCore.Qt.Key_A, QtCore.Qt.ControlModifier, DFG_SELECT_ALL)
            graph.defineHotkey(QtCore.Qt.Key_C, QtCore.Qt.ControlModifier, DFG_COPY)
            graph.defineHotkey(QtCore.Qt.Key_V, QtCore.Qt.ControlModifier, DFG_PASTE)
            graph.defineHotkey(QtCore.Qt.Key_X, QtCore.Qt.ControlModifier, DFG_CUT)
            graph.defineHotkey(QtCore.Qt.Key_N, QtCore.Qt.ControlModifier, DFG_NEW_SCENE)
            graph.defineHotkey(QtCore.Qt.Key_O, QtCore.Qt.ControlModifier, DFG_OPEN_SCENE)
            graph.defineHotkey(QtCore.Qt.Key_S, QtCore.Qt.ControlModifier, DFG_SAVE_SCENE)
            graph.defineHotkey(QtCore.Qt.Key_F2, QtCore.Qt.NoModifier, DFG_EDIT_PROPERTIES)
            graph.defineHotkey(QtCore.Qt.Key_R, QtCore.Qt.ControlModifier, DFG_RELAX_NODES)
            graph.defineHotkey(QtCore.Qt.Key_Q, QtCore.Qt.NoModifier, DFG_TOGGLE_MANIPULATION)
            graph.defineHotkey(QtCore.Qt.Key_0, QtCore.Qt.ControlModifier, DFG_RESET_ZOOM)
            graph.defineHotkey(QtCore.Qt.Key_1, QtCore.Qt.NoModifier, DFG_COLLAPSE_LEVEL_1)
            graph.defineHotkey(QtCore.Qt.Key_2, QtCore.Qt.NoModifier, DFG_COLLAPSE_LEVEL_2)
            graph.defineHotkey(QtCore.Qt.Key_3, QtCore.Qt.NoModifier, DFG_COLLAPSE_LEVEL_3)

            QObject::connect(graph, SIGNAL(nodeInspectRequested(FabricUI::GraphView::Node*)),
                this, SLOT(onNodeInspectRequested(FabricUI::GraphView::Node*)))
            QObject::connect(graph, SIGNAL(nodeEditRequested(FabricUI::GraphView::Node*)),
                this, SLOT(onNodeEditRequested(FabricUI::GraphView::Node*)))
            QObject::connect(graph, SIGNAL(sidePanelInspectRequested()),
                this, SLOT(onSidePanelInspectRequested()) )

            self.currentGraph = graph

    '''


app = QtGui.QApplication([])
app.setOrganizationName( 'Fabric Software Inc' )
app.setApplicationName( 'Fabric Canvas Standalone' )
app.setApplicationVersion( '2.0.0' )
#app.setStyle( Style.FabricStyle() )

fabricDir = os.environ.get('FABRIC_DIR', None)
if fabricDir:
    logoPath = os.path.join(fabricDir, 'Resources', 'fe_logo.png')
    app.setWindowIcon(QtGui.QIcon(logoPath))

opt_parser = optparse.OptionParser(
        usage='Usage: %prog [options] [graph]')
opt_parser.add_option('-u', '--unguarded',
                                            action='store_true',
                                            dest='unguarded',
                                            help='compile KL code in unguarded mode')
(opts, args) = opt_parser.parse_args()

unguarded = opts.unguarded

settings = QtCore.QSettings()
mainWin = MainWindow(settings, unguarded)
mainWin.show()

for arg in args:
    mainWin.loadGraph( arg )

app.exec_()


'''
FabricServices::Persistence::RTValToJSONEncoder sRTValEncoder
FabricServices::Persistence::RTValFromJSONDecoder sRTValDecoder

MainWindowEventFilter::MainWindowEventFilter(MainWindow * window)
: QObject(window)
    self.window = window

bool MainWindowEventFilter::eventFilter(
    QObject* object,
    QEvent* event
    )
    QEvent::Type eventType = event.type()

    if eventType == QEvent::KeyPress)
        QKeyEvent *keyEvent = dynamic_cast<QKeyEvent *>(event)

        # forward this to the hotkeyPressed functionality...
        if keyEvent.key() != QtCore.Qt.Key_Tab)
            self.window.self.viewport.onKeyPressed(keyEvent)
            if keyEvent.isAccepted())
                return True

            self.window.self.dfgWidget.onKeyPressed(keyEvent)
            if keyEvent.isAccepted())
                return True
    elif eventType == QEvent::KeyRelease)
        QKeyEvent *keyEvent = dynamic_cast<QKeyEvent *>(event)

        # forward this to the hotkeyReleased functionality...
        if keyEvent.key() != QtCore.Qt.Key_Tab)
            #For now the viewport isn't listening to key releases
            #self.window.self.viewport.onKeyReleased(keyEvent)
            #if keyEvent.isAccepted())
            #    return True

            self.window.self.dfgWidget.onKeyReleased(keyEvent)
            if keyEvent.isAccepted())
                return True

    return QObject::eventFilter(object, event)

'''

