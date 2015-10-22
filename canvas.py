import optparse, os, sys
from FabricUI import DFG, Viewports
import FabricEngine.Core as Core
from PySide import QtCore, QtGui, QtOpenGL

def reportCallback(source, level, line):
  #DFG.DFGLogWidget.callback
  if source == Core.ReportSource.User or level == Core.ReportLevel.Error or 'Ignoring' in line:
    sys.stdout.write(line+"\n")
  else:
    sys.stderr.write(line+"\n")

def statusCallback(target, data):
  #MainWindow.CoreStatusCallback
  if target == 'licensing':
    print 'Error handler for licensing: ' + str( data )
  else:
    print 'unknown status target'

class MainWindow(DFG.DFGMainWindow):
    def __init__(self, settings, unguarded):
        DFG.DFGMainWindow.__init__(self)

        self.settings = settings
        self.dfguiCommandHandler = None

        DFG.DFGWidget.setSettings(settings)

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
        client.loadExtension("Math")
        client.loadExtension("Parameters")
        client.loadExtension("Util")
        client.setStatusCallback(statusCallback)

        #astManager = ASTWrapper.KLASTManager(client)
        astManager = None

        evalContext = client.RT.types.EvalContext.create()
        evalContext = evalContext.getInstance('EvalContext')
        evalContext.host = 'Canvas'
        evalContext.graph = ''

        host = client.getDFGHost()
        binding = host.createBindingToNewGraph()
        self.lastSavedBindingVersion = binding.getVersion()
        self.lastAutosaveBindingVersion = self.lastSavedBindingVersion

        graph = binding.getExec()

        glFormat = QtOpenGL.QGLFormat()
        glFormat.setDoubleBuffer(True)
        glFormat.setDepth(True)
        glFormat.setAlpha(True)
        glFormat.setSampleBuffers(True)
        glFormat.setSamples(4)

        defaultWindowColor = QtGui.QColor()
        viewport = Viewports.GLViewportWidget(client._client, defaultWindowColor, glFormat, None, None)
        self.setCentralWidget(viewport)

        #dfgWidget = DFG.DFGWidget(None, client._client, host, binding, "", graph, astManager, self.dfguiCommandHandler, None)
        '''
        self.contentChanged.connect(viewport.redraw)
        viewport.portManipulationRequested.connect(this.onPortManipulationRequested)


        dockFeatures = QtGui.QDockWidget.DockWidgetMovable | QtGui.QDockWidget.DockWidgetFloatable | QtGui.QDockWidget.DockWidgetClosable

        dfgDock = QtGui.QDockWidget("Canvas Graph", self)
        dfgDock.setObjectName("Canvas Graph")
        dfgDock.setFeatures(dockFeatures)
        dfgDock.setWidget(dfgWidget)
        self.addDockWidget(Qt.BottomDockWidgetArea, dfgDock, Qt.Vertical)
        '''

    def updateFPS(self):
        print 'called updateFPS'



app = QtGui.QApplication([])
app.setOrganizationName( 'Fabric Software Inc' )
app.setApplicationName( 'Fabric Canvas Standalone' )
app.setApplicationVersion( '2.0.0' )
#app.setStyle( FabricUI.Style.FabricStyle() )

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

#for ( ; argi < argc; ++argi )
#    mainWin.loadGraph( argv[argi] );

#l = FabricUI.LastWarningDialog(None)
#l.show()
app.exec_()


'''
    try
    {
        // timeline
        m_timeLine = new Viewports::TimeLineWidget();
        m_timeLine->setTimeRange(TimeRange_Default_Frame_In, TimeRange_Default_Frame_Out);
        m_timeLine->updateTime(TimeRange_Default_Frame_In);
        QDockWidget *timeLineDock = new QDockWidget("TimeLine", this);
        timeLineDock->setObjectName( "TimeLine" );
        timeLineDock->setFeatures( dockFeatures );
        timeLineDock->setWidget(m_timeLine);
        addDockWidget(Qt::BottomDockWidgetArea, timeLineDock, Qt::Vertical);
 
        // [Julien] FE-5252
        // preset library
        // Because of a lack of performances, we don't expose the search tool of the PresetTreeWidget
        m_treeWidget = new DFG::PresetTreeWidget( m_dfgWidget->getDFGController(), config, true, false, true );
        QDockWidget *treeDock = new QDockWidget("Explorer", this);
        treeDock->setObjectName( "Explorer" );
        treeDock->setFeatures( dockFeatures );
        treeDock->setWidget(m_treeWidget);
        addDockWidget(Qt::LeftDockWidgetArea, treeDock);

        QObject::connect(m_dfgWidget, SIGNAL(newPresetSaved(QString)), m_treeWidget, SLOT(refresh()));

        // value editor
        m_dfgValueEditor =
            new DFG::DFGValueEditor(
                m_dfgWidget->getUIController(),
                config
                );
        QDockWidget *dfgValueEditorDockWidget =
            new QDockWidget(
                "Value Editor",
                this
                );
        dfgValueEditorDockWidget->setObjectName( "Values" );
        dfgValueEditorDockWidget->setFeatures( dockFeatures );
        dfgValueEditorDockWidget->setWidget( m_dfgValueEditor );
        addDockWidget( Qt::RightDockWidgetArea, dfgValueEditorDockWidget );

        // log widget
        m_logWidget = new DFG::DFGLogWidget;
        QDockWidget *logDockWidget = new QDockWidget( "Log Messages", this );
        logDockWidget->setObjectName( "Log" );
        logDockWidget->setFeatures( dockFeatures );
        logDockWidget->setWidget( m_logWidget );
        logDockWidget->hide();
        addDockWidget( Qt::TopDockWidgetArea, logDockWidget, Qt::Vertical );

        // History widget
        m_qUndoView = new QUndoView( &m_qUndoStack );
        m_qUndoView->setEmptyLabel( "New Graph" );
        QDockWidget *undoDockWidget = new QDockWidget("History", this);
        undoDockWidget->setObjectName( "History" );
        undoDockWidget->setFeatures( dockFeatures );
        undoDockWidget->setWidget(m_qUndoView);
        undoDockWidget->hide();
        addDockWidget(Qt::LeftDockWidgetArea, undoDockWidget);

        QObject::connect(
            m_dfgWidget->getUIController(), SIGNAL(varsChanged()),
            m_treeWidget, SLOT(refresh())
            );
        QObject::connect(
            m_dfgWidget->getUIController(), SIGNAL(argsChanged()),
            this, SLOT(onStructureChanged())
            );
        QObject::connect(
            m_dfgWidget->getUIController(), SIGNAL(argValuesChanged()),
            this, SLOT(onValueChanged())
            );
        QObject::connect(
            m_dfgWidget->getUIController(), SIGNAL(defaultValuesChanged()),
            this, SLOT(onValueChanged())
            );
        QObject::connect(
            m_dfgWidget, SIGNAL(nodeInspectRequested(FabricUI::GraphView::Node*)),
            this, SLOT(onNodeInspectRequested(FabricUI::GraphView::Node*))
            );
        QObject::connect(
            m_dfgWidget->getDFGController(), SIGNAL(dirty()),
            this, SLOT(onDirty())
            );

        QObject::connect(
            m_dfgWidget->getDFGController(), SIGNAL(bindingChanged(FabricCore::DFGBinding const &)),
            m_dfgValueEditor, SLOT(setBinding(FabricCore::DFGBinding const &))
            );
        QObject::connect(
            m_dfgWidget->getDFGController(), SIGNAL(nodeRemoved(FTL::CStrRef)),
            m_dfgValueEditor, SLOT(onNodeRemoved(FTL::CStrRef))
            );

        QObject::connect(
            m_dfgWidget->getTabSearchWidget(), SIGNAL(enabled(bool)),
            this, SLOT(enableShortCuts(bool))
            );

        QObject::connect(m_timeLine, SIGNAL(frameChanged(int)), this, SLOT(onFrameChanged(int)));
        // QObject::connect(m_manipAction, SIGNAL(triggered()), m_viewport, SLOT(toggleManipulation()));

        QObject::connect(m_dfgWidget, SIGNAL(onGraphSet(FabricUI::GraphView::Graph*)),
            this, SLOT(onGraphSet(FabricUI::GraphView::Graph*)));

        restoreGeometry( settings->value("mainWindow/geometry").toByteArray() );
        restoreState( settings->value("mainWindow/state").toByteArray() );

        // populate the menu bar
        QObject::connect(
            m_dfgWidget, 
            SIGNAL(additionalMenuActionsRequested(QString, QMenu*, bool)), 
            this, SLOT(onAdditionalMenuActionsRequested(QString, QMenu *, bool))
            );
        m_dfgWidget->populateMenuBar(menuBar());

        // window menu
        QMenu *windowMenu = menuBar()->addMenu(tr("&Window"));
        QAction * toggleAction = NULL;
        toggleAction = dfgDock->toggleViewAction();
        toggleAction->setShortcut( Qt::CTRL + Qt::Key_4 );
        windowMenu->addAction( toggleAction );
        toggleAction = treeDock->toggleViewAction();
        toggleAction->setShortcut( Qt::CTRL + Qt::Key_5 );
        windowMenu->addAction( toggleAction );
        toggleAction = dfgValueEditorDockWidget->toggleViewAction();
        toggleAction->setShortcut( Qt::CTRL + Qt::Key_6 );
        windowMenu->addAction( toggleAction );
        toggleAction = timeLineDock->toggleViewAction();
        toggleAction->setShortcut( Qt::CTRL + Qt::Key_9 );
        windowMenu->addAction( toggleAction );
        windowMenu->addSeparator();
        toggleAction = undoDockWidget->toggleViewAction();
        toggleAction->setShortcut( Qt::CTRL + Qt::Key_7 );
        windowMenu->addAction( toggleAction );
        toggleAction = logDockWidget->toggleViewAction();
        toggleAction->setShortcut( Qt::CTRL + Qt::Key_8 );
        windowMenu->addAction( toggleAction );

        onFrameChanged(m_timeLine->getTime());
        onGraphSet(m_dfgWidget->getUIGraph());
        onSidePanelInspectRequested();
    }
    catch(FabricCore::Exception e)
    {
        throw e;
    }

    installEventFilter(new MainWindowEventFilter(this));
}
'''



'''
//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#include "CanvasMainWindow.h"

#include <FabricServices/Persistence/RTValToJSONEncoder.hpp>
#include <FabricServices/Persistence/RTValFromJSONDecoder.hpp>
#include <FabricUI/Licensing/Licensing.h>
#include <FabricUI/DFG/DFGActions.h>

#include <FTL/CStrRef.h>
#include <FTL/FS.h>
#include <FTL/Path.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QTimer>
#include <QtGui/QAction>
#include <QtGui/QFileDialog>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QMessageBox>
#include <QtGui/QUndoView>
#include <QtGui/QVBoxLayout>

#include <sstream>

FabricServices::Persistence::RTValToJSONEncoder sRTValEncoder;
FabricServices::Persistence::RTValFromJSONDecoder sRTValDecoder;

void MainWindow::CoreStatusCallback(
    void *userdata,
    char const *destinationData, uint32_t destinationLength,
    char const *payloadData, uint32_t payloadLength
    )
{
    MainWindow *mainWindow = reinterpret_cast<MainWindow *>( userdata );
    FTL::StrRef destination( destinationData, destinationLength );
    FTL::StrRef payload( payloadData, payloadLength );
    if ( destination == FTL_STR( "licensing" ) )
    {
        try
        {
            FabricUI::HandleLicenseData(
                mainWindow,
                mainWindow->m_client,
                payload,
                true // modalDialogs
                );
        }
        catch ( FabricCore::Exception e )
        {
            DFG::DFGLogWidget::log( e.getDesc_cstr() );
        }
    }
}

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

void MainWindow::closeEvent( QCloseEvent *event )
{
    if(!checkUnsavedChanged())
    {
        event->ignore();
        return;    
    }
    m_viewport->setManipulationActive(false);
    m_settings->setValue( "mainWindow/geometry", saveGeometry() );
    m_settings->setValue( "mainWindow/state", saveState() );

    QMainWindow::closeEvent( event );
}

bool MainWindow::checkUnsavedChanged()
{
    FabricCore::DFGBinding binding = m_dfgWidget->getUIController()->getBinding();
    if ( binding.getVersion() != m_lastSavedBindingVersion )
    {
        QMessageBox msgBox;
        msgBox.setText( "Do you want to save your changes?" );
        msgBox.setInformativeText( "Your changes will be lost if you don't save them." );
        msgBox.setStandardButtons( QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel );
        msgBox.setDefaultButton( QMessageBox::Save );
        switch(msgBox.exec())
        {
            case QMessageBox::Discard:
                return true;
            case QMessageBox::Cancel:
                return false;
            case QMessageBox::Save:
            default:
                return saveGraph( false );
        }
    }
    return true;
}

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
        m_dfgWidget->getUIController()->logError(e.getDesc_cstr());
    }

    if ( m_timelinePortIndex == -1 )
        return;

    try
    {
        FabricCore::DFGBinding binding =
            m_dfgWidget->getUIController()->getBinding();
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
        m_dfgWidget->getUIController()->logError(e.getDesc_cstr());
    }
}

void MainWindow::onPortManipulationRequested(QString portName)
{
    try
    {
        DFG::DFGController * controller = m_dfgWidget->getUIController();
        FabricCore::DFGBinding &binding = controller->getBinding();
        FabricCore::DFGExec exec = binding.getExec();
        FTL::StrRef portResolvedType = exec.getExecPortResolvedType(portName.toUtf8().constData());
        FabricCore::RTVal value = m_viewport->getManipTool()->getLastManipVal();
        if(portResolvedType == "Xfo")
        {
            // pass
        }
        else if(portResolvedType == "Mat44")
            value = value.callMethod("Mat44", "toMat44", 0, 0);
        else if(portResolvedType == "Vec3")
            value = value.maybeGetMember("tr");
        else if(portResolvedType == "Quat")
            value = value.maybeGetMember("ori");
        else
        {
            QString message = "Port '"+portName;
            message += "'to be driven has unsupported type '";
            message += portResolvedType.data();
            message += "'.";
            m_dfgWidget->getUIController()->logError(message.toUtf8().constData());
            return;
        }
        controller->cmdSetArgValue(
            portName.toUtf8().constData(),
            value
            );
    }
    catch(FabricCore::Exception e)
    {
        m_dfgWidget->getUIController()->logError(e.getDesc_cstr());
    }
}

void MainWindow::onDirty()
{
    m_dfgWidget->getUIController()->execute();

    onValueChanged();

    emit contentChanged();
}

void MainWindow::onValueChanged()
{
    try
    {
        // FabricCore::DFGExec graph = m_dfgWidget->getUIController()->getGraph();
        // DFGWrapper::ExecPortList ports = graph->getPorts();
        // for(size_t i=0;i<ports.size();i++)
        // {
        //     if(ports[i]->getPortType() == FabricCore::DFGPortType_Out)
        //         continue;
        //     FabricCore::RTVal argVal = graph.getWrappedCoreBinding().getArgValue(ports[i]->getName());
        //     m_dfgWidget->getUIController()->log(argVal.getJSON().getStringCString());
        // }
        m_dfgValueEditor->updateOutputs();
    }
    catch(FabricCore::Exception e)
    {
        m_dfgWidget->getUIController()->logError(e.getDesc_cstr());
    }
}

void MainWindow::onStructureChanged()
{
    if(m_dfgWidget->getUIController()->isViewingRootGraph())
    {
        m_timelinePortIndex = -1;
        try
        {
            FabricCore::DFGExec graph =
                m_dfgWidget->getUIController()->getExec();
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
            m_dfgWidget->getUIController()->logError(e.getDesc_cstr());
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
        m_dfgWidget->getUIController();

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
        m_dfgWidget->getUIController();

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
            m_dfgWidget->getUIController();

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

void MainWindow::loadGraph( QString const &filePath )
{
    m_timeLine->pause();
    m_timelinePortIndex = -1;

    try
    {
        FabricUI::DFG::DFGController *dfgController =
            m_dfgWidget->getUIController();

        FabricCore::DFGBinding binding = dfgController->getBinding();
        binding.deallocValues();

        m_dfgValueEditor->clear();

        m_host.flushUndoRedo();
        m_qUndoStack.clear();
        m_qUndoView->setEmptyLabel( "Load Graph" );

        m_viewport->clearInlineDrawing();

        QCoreApplication::processEvents();

        FILE * file = fopen(filePath.toUtf8().constData(), "rb");
        if(file)
        {
            fseek( file, 0, SEEK_END );
            int fileSize = ftell( file );
            rewind( file );

            char * buffer = (char*) malloc(fileSize + 1);
            buffer[fileSize] = '\0';

            fread(buffer, 1, fileSize, file);

            fclose(file);

            std::string json = buffer;
            free(buffer);

            FabricCore::DFGBinding binding =
                m_host.createBindingFromJSON( json.c_str() );
            m_lastSavedBindingVersion = binding.getVersion();
            FabricCore::DFGExec exec = binding.getExec();
            dfgController->setBindingExec( binding, FTL::StrRef(), exec );
            onSidePanelInspectRequested();

            m_dfgWidget->getUIController()->checkErrors();

            m_evalContext.setMember("currentFilePath", FabricCore::RTVal::ConstructString(m_client, filePath.toUtf8().constData()));

            m_dfgWidget->getUIController()->bindUnboundRTVals();
            m_dfgWidget->getUIController()->execute();

            QString tl_start = exec.getMetadata("timeline_start");
            QString tl_end = exec.getMetadata("timeline_end");
            QString tl_loopMode = exec.getMetadata("timeline_loopMode");
            QString tl_simulationMode = exec.getMetadata("timeline_simMode");
            QString tl_current = exec.getMetadata("timeline_current");

            if(tl_start.length() > 0 && tl_end.length() > 0)
                m_timeLine->setTimeRange(tl_start.toInt(), tl_end.toInt());
            else
                m_timeLine->setTimeRange(TimeRange_Default_Frame_In, TimeRange_Default_Frame_Out);

            if(tl_loopMode.length() > 0)
                m_timeLine->setLoopMode(tl_loopMode.toInt());
            else
                m_timeLine->setLoopMode(1);

            if(tl_simulationMode.length() > 0)
                m_timeLine->setSimulationMode(tl_simulationMode.toInt());
            else
                m_timeLine->setSimulationMode(0);

            QString camera_mat44 = exec.getMetadata("camera_mat44");
            QString camera_focalDistance = exec.getMetadata("camera_focalDistance");
            if(camera_mat44.length() > 0 && camera_focalDistance.length() > 0)
            {
                try
                {
                    FabricCore::RTVal mat44 = FabricCore::ConstructRTValFromJSON(m_client, "Mat44", camera_mat44.toUtf8().constData());
                    FabricCore::RTVal focalDistance = FabricCore::ConstructRTValFromJSON(m_client, "Float32", camera_focalDistance.toUtf8().constData());
                    FabricCore::RTVal camera = m_viewport->getCamera();
                    camera.callMethod("", "setFromMat44", 1, &mat44);
                    camera.callMethod("", "setFocalDistance", 1, &focalDistance);
                }
                catch(FabricCore::Exception e)
                {
                    printf("Exception: %s\n", e.getDesc_cstr());
                }
            }

            emit contentChanged();
            onStructureChanged();

            onFileNameChanged( filePath );

            // then set it to the current value if we still have it.
            // this will ensure that sim mode scenes will play correctly.
            if(tl_current.length() > 0)
                m_timeLine->updateTime(tl_current.toInt(), true);
            else
                m_timeLine->updateTime(TimeRange_Default_Frame_In, true);

            m_viewport->update();
        }
    }
    catch(FabricCore::Exception e)
    {
        printf("Exception: %s\n", e.getDesc_cstr());
    }

    m_lastFileName = filePath;
    // m_saveGraphAction->setEnabled(true);
}

void MainWindow::onSaveGraph()
{
    saveGraph(false);
}

void MainWindow::onSaveGraphAs()
{
    saveGraph(true);
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
        m_dfgWidget->getUIController()->getBinding();

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
    if ( !m_dfgWidget || !m_dfgWidget->getUIController() )
        return;

    FabricCore::DFGBinding binding = m_dfgWidget->getUIController()->getBinding();
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

