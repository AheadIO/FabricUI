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

    def __init__(self, settings, unguarded):
        super(MainWindow, self).__init__()

        self.settings = settings
        DFG.DFGWidget.setSettings(settings)

        self.config = DFG.DFGConfig()

        # autosaveBasename = 'autosave.'+str(os.getpid())+'.canvas'

        statusBar = QtGui.QStatusBar(self)
        fpsLabel = QtGui.QLabel(statusBar)
        statusBar.addPermanentWidget(fpsLabel)
        self.setStatusBar(statusBar)
        statusBar.show()

        fpsTimer = QtCore.QTimer()
        fpsTimer.setInterval(1000)
        fpsTimer.timeout.connect(self.updateFPS)
        fpsTimer.start()

        client = Core.createClient({'unguarded': unguarded, 'reportCallback': reportCallback})
        #options.licenseType = FabricCore::ClientLicenseType_Interactive;
        #options.rtValToJSONEncoder = &sRTValEncoder;
        #options.rtValFromJSONDecoder = &sRTValDecoder;
        client.loadExtension('Math')
        client.loadExtension('Parameters')
        client.loadExtension('Util')
        client.setStatusCallback(self.statusCallback)

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

        viewport = Viewports.GLViewportWidget(client, self.config.defaultWindowColor, glFormat, None, None)
        self.setCentralWidget(viewport)

        self.dfgWidget = DFG.DFGWidget(None, client._client, self.host, binding, '', graph, astManager, dfguiCommandHandler, self.config)

        self.contentChanged.connect(viewport.redraw)
        viewport.portManipulationRequested.connect(self.onPortManipulationRequested)

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
        #dfgValueEditorDockWidget.setWidget(self.dfgValueEditor)
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

        '''
        controller = self.dfgWidget.getDFGController()
        controller.varsChanged.connect(treeWidget.refresh)
        controller.argsChanged.connect(self.onStructureChanged)
        controller.argValuesChanged.connect(self.onValueChanged)
        controller.defaultValuesChanged.connect(self.onValueChanged)
        self.dfgWidget.nodeInspectRequested.connect(self.onNodeInspectRequested)
        controller.dirty.connect(self.dirty)
        controller.bindingChanged.connect(dfgValueEditor.setBinding)
        controller.nodeRemoved.connect(dfgValueEditor.onNodeRemoved)
        self.dfgWidget.getTabSearchWidget().enabled.connect(self.enableShortCuts)
        timeLine.frameChanged.connect(self.onFrameChanged)
        self.dfgWidget.onGraphSet.connect(self.onGraphSet)

        self.restoreGeometry(settings.value("mainWindow/geometry"))
        self.restoreState(settings.value("mainWindow/state"))

        self.dfgWidget.additionalMenuActionsRequested.connect(self.onAdditionalMenuActionsRequested)
        '''
        
        self.dfgWidget.populateMenuBar(self.menuBar())
        windowMenu = self.menuBar().addMenu("&Window")

        '''
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

        onFrameChanged(timeLine.getTime())
        onGraphSet(self.dfgWidget.getUIGraph())
        onSidePanelInspectRequested()
        '''

    def statusCallback(self, target, data):
        if target == "licensing":
            try:
                pass
                #FabricUI_HandleLicenseData(self, self.client, data, True)
            except Exception as e:
                self.dfgWidget.getDFGController().logError(e)

    def updateFPS(self):
        print 'called updateFPS'

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
            self.dfgWidget.getDFGController().logError(e)

    def onDirty(self):
        self.dfgWidget.getDFGController().execute()
        onValueChanged()
        self.contentChanged.emit()


    def onValueChanged(self):
        try:
            self.dfgValueEditor.updateOutputs()
        except Exception as e:
            self.dfgWidget.getDFGController().logError(e)

    def loadGraph(self, filePath):
        self.timeLine.pause()
        self.timelinePortIndex = -1

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

    def checkUnsavedChanged(self):
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

    '''
MainWindow::~MainWindow()
{
    if(m_manager)
        delete(m_manager);

    FTL::FSMaybeDeleteFile( m_autosaveFilename );
}

void MainWindow::onHotkeyPressed(Qt::Key key, Qt::KeyboardModifier modifiers, QString hotkey)
{
    if(hotkey == DFG_EXECUTE)
    {
        onDirty();
    }
    else if(hotkey == DFG_NEW_SCENE)
    {
        onNewGraph();
    }
    else if(hotkey == DFG_OPEN_SCENE)
    {
        onLoadGraph();
    }
    else if(hotkey == DFG_SAVE_SCENE)
    {
        saveGraph(false);
    }
    else if(hotkey == DFG_TOGGLE_MANIPULATION)
    {
        // Make sure we use the Action path, so menu's "checked" state is updated
        if( m_manipAction )
            m_manipAction->trigger();
    }
    else
    {
        m_dfgWidget->onHotkeyPressed(key, modifiers, hotkey);
    }
}

void MainWindow::onFrameChanged(int frame)
{
    try
    {
        m_evalContext.setMember("time", FabricCore::RTVal::ConstructFloat32(m_client, frame));
    }
    catch(FabricCore::Exception e)
    {
        m_dfgWidget->getDFGController()->logError(e.getDesc_cstr());
    }

    if ( m_timelinePortIndex == -1 )
        return;

    try
    {
        FabricCore::DFGBinding binding =
            m_dfgWidget->getDFGController()->getBinding();
        FabricCore::DFGExec exec = binding.getExec();
        if ( exec.isExecPortResolvedType( m_timelinePortIndex, "SInt32" ) )
            binding.setArgValue(
                m_timelinePortIndex,
                FabricCore::RTVal::ConstructSInt32( m_client, frame ),
                false
                );
        else if ( exec.isExecPortResolvedType( m_timelinePortIndex, "UInt32" ) )
            binding.setArgValue(
                m_timelinePortIndex,
                FabricCore::RTVal::ConstructUInt32( m_client, frame ),
                false
                );
        else if ( exec.isExecPortResolvedType( m_timelinePortIndex, "Float32" ) )
            binding.setArgValue(
                m_timelinePortIndex,
                FabricCore::RTVal::ConstructFloat32( m_client, frame ),
                false
                );
        else if ( exec.isExecPortResolvedType( m_timelinePortIndex, "Float64" ) )
            binding.setArgValue(
                m_timelinePortIndex,
                FabricCore::RTVal::ConstructFloat64( m_client, frame ),
                false
                );
    }
    catch(FabricCore::Exception e)
    {
        m_dfgWidget->getDFGController()->logError(e.getDesc_cstr());
    }
}

void MainWindow::onStructureChanged()
{
    if(m_dfgWidget->getDFGController()->isViewingRootGraph())
    {
        m_timelinePortIndex = -1;
        try
        {
            FabricCore::DFGExec graph =
                m_dfgWidget->getDFGController()->getExec();
            unsigned portCount = graph.getExecPortCount();
            for(unsigned i=0;i<portCount;i++)
            {
                if ( graph.getExecPortType(i) == FabricCore::DFGPortType_Out )
                    continue;
                FTL::CStrRef portName = graph.getExecPortName( i );
                if ( portName != FTL_STR("timeline") )
                    continue;
                if ( !graph.isExecPortResolvedType( i, "SInt32" )
                    && !graph.isExecPortResolvedType( i, "UInt32" )
                    && !graph.isExecPortResolvedType( i, "Float32" )
                    && !graph.isExecPortResolvedType( i, "Float64" ) )
                    continue;
                m_timelinePortIndex = int( i );
                break;
            }
        }
        catch(FabricCore::Exception e)
        {
            m_dfgWidget->getDFGController()->logError(e.getDesc_cstr());
        }
    }
}

void MainWindow::updateFPS()
{
    if ( !m_viewport )
        return;

    QString caption;
    caption.setNum(m_viewport->fps(), 'f', 2);
    caption += " FPS";
    m_fpsLabel->setText( caption );
}

void MainWindow::onGraphSet(FabricUI::GraphView::Graph * graph)
{
    if(graph != m_setGraph)
    {
        GraphView::Graph * graph = m_dfgWidget->getUIGraph();
        graph->defineHotkey(Qt::Key_Delete,         Qt::NoModifier,             DFG_DELETE);
        graph->defineHotkey(Qt::Key_Backspace,    Qt::NoModifier,             DFG_DELETE_2);
        graph->defineHotkey(Qt::Key_F5,                 Qt::NoModifier,             DFG_EXECUTE);
        graph->defineHotkey(Qt::Key_F,                    Qt::NoModifier,             DFG_FRAME_SELECTED);
        graph->defineHotkey(Qt::Key_A,                    Qt::NoModifier,             DFG_FRAME_ALL);
        graph->defineHotkey(Qt::Key_Tab,                Qt::NoModifier,             DFG_TAB_SEARCH);
        graph->defineHotkey(Qt::Key_A,                    Qt::ControlModifier,    DFG_SELECT_ALL);
        graph->defineHotkey(Qt::Key_C,                    Qt::ControlModifier,    DFG_COPY);
        graph->defineHotkey(Qt::Key_V,                    Qt::ControlModifier,    DFG_PASTE);
        graph->defineHotkey(Qt::Key_X,                    Qt::ControlModifier,    DFG_CUT);
        graph->defineHotkey(Qt::Key_N,                    Qt::ControlModifier,    DFG_NEW_SCENE);
        graph->defineHotkey(Qt::Key_O,                    Qt::ControlModifier,    DFG_OPEN_SCENE);
        graph->defineHotkey(Qt::Key_S,                    Qt::ControlModifier,    DFG_SAVE_SCENE);
        graph->defineHotkey(Qt::Key_F2,                 Qt::NoModifier,             DFG_EDIT_PROPERTIES);
        graph->defineHotkey(Qt::Key_R,                    Qt::ControlModifier,    DFG_RELAX_NODES);
        graph->defineHotkey(Qt::Key_Q,                    Qt::NoModifier,             DFG_TOGGLE_MANIPULATION);
        graph->defineHotkey(Qt::Key_0,                    Qt::ControlModifier,    DFG_RESET_ZOOM);
        graph->defineHotkey(Qt::Key_1,                    Qt::NoModifier,             DFG_COLLAPSE_LEVEL_1);
        graph->defineHotkey(Qt::Key_2,                    Qt::NoModifier,             DFG_COLLAPSE_LEVEL_2);
        graph->defineHotkey(Qt::Key_3,                    Qt::NoModifier,             DFG_COLLAPSE_LEVEL_3);

        // we don't need to connect this signal, because we are invoking the slot
        // in our own hotkeypressed method                    
        // QObject::connect(graph, SIGNAL(hotkeyPressed(Qt::Key, Qt::KeyboardModifier, QString)),
        //     this, SLOT(onHotkeyPressed(Qt::Key, Qt::KeyboardModifier, QString)));

        QObject::connect(graph, SIGNAL(nodeInspectRequested(FabricUI::GraphView::Node*)),
            this, SLOT(onNodeInspectRequested(FabricUI::GraphView::Node*)));
        QObject::connect(graph, SIGNAL(nodeEditRequested(FabricUI::GraphView::Node*)),
            this, SLOT(onNodeEditRequested(FabricUI::GraphView::Node*)));
        QObject::connect(graph, SIGNAL(sidePanelInspectRequested()),
            this, SLOT(onSidePanelInspectRequested()) );

        m_setGraph = graph;
    }
}

void MainWindow::onNodeInspectRequested(
    FabricUI::GraphView::Node *node
    )
{
    if ( node->isBackDropNode() )
        return;

    FabricUI::DFG::DFGController *dfgController =
        m_dfgWidget->getDFGController();

    m_dfgValueEditor->setNode(
        dfgController->getBinding(),
        dfgController->getExecPath(),
        dfgController->getExec(),
        node->name()
        );
}

void MainWindow::onNodeEditRequested(
    FabricUI::GraphView::Node *node
    )
{
    m_dfgWidget->maybeEditNode(node);
}

void MainWindow::onSidePanelInspectRequested()
{
    FabricUI::DFG::DFGController *dfgController =
        m_dfgWidget->getDFGController();

    if ( dfgController->isViewingRootGraph() )
        m_dfgValueEditor->setBinding( dfgController->getBinding() );
    else
        m_dfgValueEditor->clear();
}

void MainWindow::onNewGraph()
{
    m_timeLine->pause();

    if(!checkUnsavedChanged())
        return;

    m_lastFileName = "";
    // m_saveGraphAction->setEnabled(false);

    try
    {
        FabricUI::DFG::DFGController *dfgController =
            m_dfgWidget->getDFGController();

        FabricCore::DFGBinding binding = dfgController->getBinding();
        binding.deallocValues();

        m_dfgValueEditor->clear();

        m_host.flushUndoRedo();
        m_qUndoStack.clear();
        m_viewport->clearInlineDrawing();
        QCoreApplication::processEvents();

        // Note: the previous binding is no longer functional;
        //             create the new one before resetting the timeline options

        binding = m_host.createBindingToNewGraph();
        m_lastSavedBindingVersion = binding.getVersion();
        FabricCore::DFGExec exec = binding.getExec();
        m_timelinePortIndex = -1;

        dfgController->setBindingExec( binding, FTL::StrRef(), exec );

        m_timeLine->setTimeRange(TimeRange_Default_Frame_In, TimeRange_Default_Frame_Out);
        m_timeLine->setLoopMode(1);
        m_timeLine->setSimulationMode(0);
        m_timeLine->updateTime(TimeRange_Default_Frame_In, true);

        QCoreApplication::processEvents();
        m_qUndoView->setEmptyLabel( "New Graph" );

        onSidePanelInspectRequested();

        emit contentChanged();
        onStructureChanged();

        onFileNameChanged( "" );

        m_viewport->update();
    }
    catch(FabricCore::Exception e)
    {
        printf("Exception: %s\n", e.getDesc_cstr());
    }
}

void MainWindow::onLoadGraph()
{
    m_timeLine->pause();

    if(!checkUnsavedChanged())
        return;

    QString lastPresetFolder = m_settings->value("mainWindow/lastPresetFolder").toString();
    QString filePath = QFileDialog::getOpenFileName(this, "Load graph", lastPresetFolder, "*.canvas");
    if ( filePath.length() )
    {
        QDir dir(filePath);
        dir.cdUp();
        m_settings->setValue( "mainWindow/lastPresetFolder", dir.path() );
        loadGraph( filePath );
    }
    // m_saveGraphAction->setEnabled(true);
}

bool MainWindow::performSave(
    FabricCore::DFGBinding &binding,
    QString const &filePath
    )
{
    FabricCore::DFGExec graph = binding.getExec();

    QString num;
    num.setNum(m_timeLine->getRangeStart());
    graph.setMetadata("timeline_start", num.toUtf8().constData(), false);
    num.setNum(m_timeLine->getRangeEnd());
    graph.setMetadata("timeline_end", num.toUtf8().constData(), false);
    num.setNum(m_timeLine->getTime());
    graph.setMetadata("timeline_current", num.toUtf8().constData(), false);
    num.setNum(m_timeLine->loopMode());
    graph.setMetadata("timeline_loopMode", num.toUtf8().constData(), false);
    num.setNum(m_timeLine->simulationMode());
    graph.setMetadata("timeline_simMode", num.toUtf8().constData(), false);
    try
    {
        FabricCore::RTVal camera = m_viewport->getCamera();
        FabricCore::RTVal mat44 = camera.callMethod("Mat44", "getMat44", 0, 0);
        FabricCore::RTVal focalDistance = camera.callMethod("Float32", "getFocalDistance", 0, 0);

        if(mat44.isValid() && focalDistance.isValid())
        {
            graph.setMetadata("camera_mat44", mat44.getJSON().getStringCString(), false);
            graph.setMetadata("camera_focalDistance", focalDistance.getJSON().getStringCString(), false);
        }
    }
    catch(FabricCore::Exception e)
    {
        printf("Exception: %s\n", e.getDesc_cstr());
    }

    try
    {
        FabricCore::DFGStringResult json = binding.exportJSON();
        char const *jsonData;
        uint32_t jsonSize;
        json.getStringDataAndLength( jsonData, jsonSize );
        FILE * file = fopen(filePath.toUtf8().constData(), "wb");
        if(file)
        {
            fwrite(jsonData, jsonSize, 1, file);
            fclose(file);
        }
    }
    catch(FabricCore::Exception e)
    {
        printf("Exception: %s\n", e.getDesc_cstr());
        return false;
    }

    return true;
}

bool MainWindow::saveGraph(bool saveAs)
{
    m_timeLine->pause();

    QString filePath = m_lastFileName;
    if(filePath.length() == 0 || saveAs)
    {
        QString lastPresetFolder = m_settings->value("mainWindow/lastPresetFolder").toString();
        if(m_lastFileName.length() > 0)
        {
            filePath = m_lastFileName;
            if(filePath.toLower().endsWith(".canvas"))
                filePath = filePath.left(filePath.length() - 7);
        }
        else
            filePath = lastPresetFolder;

        filePath = QFileDialog::getSaveFileName(this, "Save graph", filePath, "*.canvas");
        if(filePath.length() == 0)
            return false;
        if(filePath.toLower().endsWith(".canvas.canvas"))
            filePath = filePath.left(filePath.length() - 7);
        else if(!filePath.toLower().endsWith(".canvas"))
            filePath += ".canvas";
    }

    QDir dir(filePath);
    dir.cdUp();
    m_settings->setValue( "mainWindow/lastPresetFolder", dir.path() );

    FabricCore::DFGBinding &binding =
        m_dfgWidget->getDFGController()->getBinding();

    if ( performSave( binding, filePath ) )
        m_evalContext.setMember("currentFilePath", FabricCore::RTVal::ConstructString(m_client, filePath.toUtf8().constData()));

    m_lastFileName = filePath;

    onFileNameChanged( filePath );

    // m_saveGraphAction->setEnabled(true);

    m_lastSavedBindingVersion = binding.getVersion();

    return true;
}

void MainWindow::setBlockCompilations( bool blockCompilations )
{
    FabricUI::DFG::DFGController *dfgController =
        m_dfgWidget->getDFGController();
    dfgController->setBlockCompilations( blockCompilations );
}

void MainWindow::onFileNameChanged(QString fileName)
{
    if(fileName.isEmpty())
        setWindowTitle( m_windowTitle );
    else
        setWindowTitle( m_windowTitle + " - " + fileName );
}

void MainWindow::enableShortCuts(bool enabled)
{
    if(m_newGraphAction)
        m_newGraphAction->blockSignals(enabled);
    if(m_loadGraphAction)
        m_loadGraphAction->blockSignals(enabled);
    if(m_saveGraphAction)
        m_saveGraphAction->blockSignals(enabled);
    if(m_saveGraphAsAction)
        m_saveGraphAsAction->blockSignals(enabled);
    if(m_quitAction)
        m_quitAction->blockSignals(enabled);
    if(m_manipAction)
        m_manipAction->blockSignals(enabled);
    if(m_setGridVisibleAction)
        m_setGridVisibleAction->blockSignals(enabled);
    if(m_setUsingStageAction)
        m_setUsingStageAction->blockSignals(enabled);
    if(m_resetCameraAction)
        m_resetCameraAction->blockSignals(enabled);
    if(m_clearLogAction)
        m_clearLogAction->blockSignals(enabled);
    if(m_blockCompilationsAction)
        m_blockCompilationsAction->blockSignals(enabled);
}
 
void MainWindow::onAdditionalMenuActionsRequested(QString name, QMenu * menu, bool prefix)
{
    if(name == "File")
    {
        if(prefix)
        {
            m_newGraphAction = menu->addAction("New Graph");
            m_newGraphAction->setShortcut(QKeySequence::New);
            m_loadGraphAction = menu->addAction("Load Graph...");
            m_loadGraphAction->setShortcut(QKeySequence::Open);
            m_saveGraphAction = menu->addAction("Save Graph");
            m_saveGraphAction->setShortcut(QKeySequence::Save);
            m_saveGraphAsAction = menu->addAction("Save Graph As...");
            m_saveGraphAsAction->setShortcut(QKeySequence::SaveAs);
        
            QObject::connect(m_newGraphAction, SIGNAL(triggered()), this, SLOT(onNewGraph()));
            QObject::connect(m_loadGraphAction, SIGNAL(triggered()), this, SLOT(onLoadGraph()));
            QObject::connect(m_saveGraphAction, SIGNAL(triggered()), this, SLOT(onSaveGraph()));
            QObject::connect(m_saveGraphAsAction, SIGNAL(triggered()), this, SLOT(onSaveGraphAs()));
        }
        else
        {
            menu->addSeparator();
            m_quitAction = menu->addAction("Quit");
            m_quitAction->setShortcut(QKeySequence::Quit);
            
            QObject::connect(m_quitAction, SIGNAL(triggered()), this, SLOT(close()));
        }
    }
    else if(name == "Edit")
    {
        if(prefix)
        {
            QAction *undoAction = m_qUndoStack.createUndoAction( menu );
            undoAction->setShortcut( QKeySequence::Undo );
            menu->addAction( undoAction );
            QAction *redoAction = m_qUndoStack.createRedoAction( menu );
            redoAction->setShortcut( QKeySequence::Redo );
            menu->addAction( redoAction );
        }
        else
        {
            menu->addSeparator();
            m_manipAction = new QAction( DFG_TOGGLE_MANIPULATION, m_viewport );
            m_manipAction->setShortcut(Qt::Key_Q);
            m_manipAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
            m_manipAction->setCheckable( true );
            m_manipAction->setChecked( m_viewport->isManipulationActive() );
            QObject::connect(
                m_manipAction, SIGNAL(triggered()),
                m_viewport, SLOT(toggleManipulation())
                );
            m_viewport->addAction( m_manipAction );
            menu->addAction( m_manipAction );
        }
    }
    else if(name == "View")
    {
        if(prefix)
        {
            m_setGridVisibleAction = new QAction("&Display Grid", 0 );
            m_setGridVisibleAction->setShortcut(Qt::CTRL + Qt::Key_G);
            m_setGridVisibleAction->setCheckable( true );
            m_setGridVisibleAction->setChecked( m_viewport->isStageVisible() );
         
            QObject::connect(
                m_setGridVisibleAction, SIGNAL(toggled(bool)),
                m_viewport, SLOT(setStageVisible(bool)) 
                );
            
            /*
                m_setUsingStageAction = new QAction( "Use &Stage", 0 );
                m_setUsingStageAction->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_G);
                m_setUsingStageAction->setCheckable( true );
                m_setUsingStageAction->setChecked( m_viewport->isUsingStage() );
                QObject::connect(
                    m_setUsingStageAction, SIGNAL(toggled(bool)),
                    m_viewport, SLOT(setUsingStage(bool))
                    );
            */

            m_resetCameraAction = new QAction( "&Reset Camera", m_viewport );
            m_resetCameraAction->setShortcut(Qt::Key_R);
            m_resetCameraAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
            QObject::connect(
                m_resetCameraAction, SIGNAL(triggered()),
                m_viewport, SLOT(resetCamera())
                );
            m_viewport->addAction(m_resetCameraAction);

            m_clearLogAction = new QAction( "&Clear Log Messages", 0 );
            QObject::connect(
                m_clearLogAction, SIGNAL(triggered()),
                m_logWidget, SLOT(clear())
                );

            m_blockCompilationsAction = new QAction( "&Block compilations", 0 );
            m_blockCompilationsAction->setCheckable( true );
            m_blockCompilationsAction->setChecked( false );
            QObject::connect(
                m_blockCompilationsAction, SIGNAL(toggled(bool)),
                this, SLOT(setBlockCompilations(bool))
                );

            // [Julien] FE-4965
            menu->addAction( m_setGridVisibleAction );
            //menu->addAction( m_setUsingStageAction );
            menu->addSeparator();
            menu->addAction( m_resetCameraAction );
            menu->addSeparator();
            menu->addAction( m_clearLogAction );
            menu->addSeparator();
            menu->addAction( m_blockCompilationsAction );
        }
    }
}

void MainWindow::autosave()
{
    // [andrew 20150909] can happen if this triggers while the licensing
    // dialogs are up
    if ( !m_dfgWidget || !m_dfgWidget->getDFGController() )
        return;

    FabricCore::DFGBinding binding = m_dfgWidget->getDFGController()->getBinding();
    if ( !!binding )
    {
        uint32_t bindingVersion = binding.getVersion();
        if ( bindingVersion != m_lastAutosaveBindingVersion )
        {
            std::string tmpAutosaveFilename = m_autosaveFilename;
            tmpAutosaveFilename += FTL_STR(".tmp");

            if ( performSave( binding, tmpAutosaveFilename.c_str() ) )
            {
                FTL::FSMaybeMoveFile( tmpAutosaveFilename, m_autosaveFilename );

                m_lastAutosaveBindingVersion = bindingVersion;
            }
        }
    }
}
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
FabricServices::Persistence::RTValToJSONEncoder sRTValEncoder;
FabricServices::Persistence::RTValFromJSONDecoder sRTValDecoder;

MainWindowEventFilter::MainWindowEventFilter(MainWindow * window)
: QObject(window)
{
    m_window = window;
}

bool MainWindowEventFilter::eventFilter(
    QObject* object,
    QEvent* event
    )
{
    QEvent::Type eventType = event->type();

    if (eventType == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = dynamic_cast<QKeyEvent *>(event);

        // forward this to the hotkeyPressed functionality...
        if(keyEvent->key() != Qt::Key_Tab)
        {
            m_window->m_viewport->onKeyPressed(keyEvent);
            if(keyEvent->isAccepted())
                return true;

            m_window->m_dfgWidget->onKeyPressed(keyEvent);
            if(keyEvent->isAccepted())
                return true;
        }
    }
    else if (eventType == QEvent::KeyRelease)
    {
        QKeyEvent *keyEvent = dynamic_cast<QKeyEvent *>(event);

        // forward this to the hotkeyReleased functionality...
        if(keyEvent->key() != Qt::Key_Tab)
        {
            //For now the viewport isn't listening to key releases
            //m_window->m_viewport->onKeyReleased(keyEvent);
            //if(keyEvent->isAccepted())
            //    return true;

            m_window->m_dfgWidget->onKeyReleased(keyEvent);
            if(keyEvent->isAccepted())
                return true;
        }
    }

    return QObject::eventFilter(object, event);
};

'''

