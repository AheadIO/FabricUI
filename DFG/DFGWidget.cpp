// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtGui/QVBoxLayout>
#include <QtGui/QCursor>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>
#include <FabricCore.h>
#include <FabricUI/DFG/DFGWidget.h>
#include <FabricUI/DFG/DFGMainWindow.h>
#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/DFG/Dialogs/DFGGetStringDialog.h>
#include <FabricUI/DFG/Dialogs/DFGEditPortDialog.h>
#include <FabricUI/DFG/Dialogs/DFGNewVariableDialog.h>
#include <assert.h>

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

QSettings * DFGWidget::g_settings = NULL;

DFGWidget::DFGWidget(
  QWidget * parent, 
  FabricCore::Client const &client,
  FabricCore::DFGHost const &host,
  FabricCore::DFGBinding const &binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec const &exec,
  FabricServices::ASTWrapper::KLASTManager * manager,
  DFGUICmdHandler *cmdHandler,
  Commands::CommandStack * stack,
  const DFGConfig & dfgConfig,
  bool overTakeBindingNotifications
  )
  : QWidget(parent)
  , m_client(client)
  , m_host( host )
  , m_binding( binding )
  , m_exec( exec )
  , m_manager(manager)
{
  m_dfgConfig = dfgConfig;
  m_uiGraph = NULL;
  m_uiHeader = new GraphView::GraphHeaderWidget(this, "Graph", dfgConfig.graphConfig);
  m_uiGraphViewWidget = new DFGGraphViewWidget(this, dfgConfig.graphConfig, NULL);
  m_uiController =
    new DFGController(
      NULL,
      m_client,
      m_manager,
      m_host,
      m_binding,
      cmdHandler,
      stack,
      overTakeBindingNotifications
      );
  m_klEditor = new DFGKLEditorWidget(this, m_uiController.get(), m_manager, m_dfgConfig);
  m_klEditor->hide();
  m_tabSearchWidget = new DFGTabSearchWidget(this, m_dfgConfig);
  m_tabSearchWidget->hide();


  QVBoxLayout * layout = new QVBoxLayout();
  layout->setSpacing(0);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(m_uiHeader);
  layout->addWidget(m_uiGraphViewWidget);
  layout->addWidget(m_klEditor);
  layout->setContentsMargins(0, 0, 0, 0);
  setLayout(layout);
  setContentsMargins(0, 0, 0, 0);

  m_router = NULL;

  setGraph( m_host, m_binding, m_execPath, m_exec );

  QObject::connect(
    m_uiHeader, SIGNAL(goUpPressed()),
    this, SLOT(onGoUpPressed())
    );

  QObject::connect(
    m_uiController.get(), SIGNAL(nodeEditRequested(FabricUI::GraphView::Node *)), 
    this, SLOT(onNodeEditRequested(FabricUI::GraphView::Node *))
  );  
}

DFGWidget::~DFGWidget()
{
}

void DFGWidget::setGraph(
  FabricCore::DFGHost const &host,
  FabricCore::DFGBinding const &binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec const &exec
  )
{
  m_host = host;
  m_binding = binding;
  m_execPath = execPath;
  m_exec = exec;

  if(m_router)
  {
    m_uiController->setRouter(NULL);
    delete(m_router);
    m_router = NULL;
  }

  if(m_exec.isValid())
  {
    m_uiGraph = new GraphView::Graph( NULL, m_dfgConfig.graphConfig );
    m_uiGraph->setController(m_uiController.get());
    m_uiController->setGraph(m_uiGraph);

    QObject::connect(
      m_uiGraph, SIGNAL(hotkeyPressed(Qt::Key, Qt::KeyboardModifier, QString)), 
      this, SLOT(onHotkeyPressed(Qt::Key, Qt::KeyboardModifier, QString))
    );  
    QObject::connect(
      m_uiGraph, SIGNAL(hotkeyReleased(Qt::Key, Qt::KeyboardModifier, QString)), 
      this, SLOT(onHotkeyReleased(Qt::Key, Qt::KeyboardModifier, QString))
    );  
    m_uiGraph->defineHotkey(Qt::Key_Space, Qt::NoModifier, "PanGraph");

    m_uiGraph->initialize();

    m_router =
      static_cast<DFGNotificationRouter *>(
        m_uiController->createRouter(
          m_binding,
          m_execPath,
          m_exec
          )
        );
    m_uiController->setHost(m_host);
    m_uiController->setBinding(m_binding);
    m_uiController->setRouter(m_router);
    m_uiHeader->setCaption(m_exec.getTitle());
  
    m_uiGraph->setGraphContextMenuCallback(&graphContextMenuCallback, this);
    m_uiGraph->setNodeContextMenuCallback(&nodeContextMenuCallback, this);
    m_uiGraph->setPortContextMenuCallback(&portContextMenuCallback, this);
    m_uiGraph->setSidePanelContextMenuCallback(&sidePanelContextMenuCallback, this);

    // FE-4277
    emit onGraphSet(m_uiGraph);
  }
  else
  {
    m_uiGraph = NULL;
    m_uiController->setGraph(NULL);
  }

  m_uiGraphViewWidget->setGraph(m_uiGraph);

  if ( m_uiGraph )
  {
    try
    {
      m_router->onGraphSet();
    }
    catch(FabricCore::Exception e)
    {
      printf( "%s\n", e.getDesc_cstr() );
    }

    // FE-4277
    emit onGraphSet(m_uiGraph);
  }
}

GraphView::Graph * DFGWidget::getUIGraph()
{
  return m_uiGraph;
}

DFGController * DFGWidget::getUIController()
{
  return m_uiController.get();
}

DFGTabSearchWidget * DFGWidget::getTabSearchWidget()
{
  return m_tabSearchWidget;
}

DFGGraphViewWidget * DFGWidget::getGraphViewWidget()
{
  return m_uiGraphViewWidget;
}

QMenu* DFGWidget::graphContextMenuCallback(FabricUI::GraphView::Graph* graph, void* userData)
{
  DFGWidget * graphWidget = (DFGWidget*)userData;
  if(graph->controller() == NULL)
    return NULL;
  QMenu* result = new QMenu(NULL);
  result->addAction("New empty graph");
  result->addAction("New empty function");

  const std::vector<GraphView::Node*> & nodes = graphWidget->getUIController()->graph()->selectedNodes();
  if(nodes.size() > 0)
  {
    result->addSeparator();
    result->addAction("Implode nodes");
  }

  result->addSeparator();
  result->addAction("New Variable");
  result->addAction("Read Variable (Get)");
  result->addAction("Write Variable (Set)");

  graphWidget->connect(result, SIGNAL(triggered(QAction*)), graphWidget, SLOT(onGraphAction(QAction*)));
  return result;
}

QMenu* DFGWidget::nodeContextMenuCallback(FabricUI::GraphView::Node* uiNode, void* userData)
{
  DFGWidget * graphWidget = (DFGWidget*)userData;
  GraphView::Graph * graph = graphWidget->m_uiGraph;
  if(graph->controller() == NULL)
    return NULL;
  graphWidget->m_contextNode = uiNode;

  char const * nodeName = uiNode->name().c_str();

  if(graphWidget->m_exec.getNodeType(nodeName) != FabricCore::DFGNodeType_Inst)
    return NULL;
  FabricCore::DFGExec subExec = graphWidget->m_exec.getSubExec(nodeName);

  QMenu* result = new QMenu(NULL);
  QAction* action;
  FEC_DFGCacheRule cacheRule = graphWidget->m_exec.getInstCacheRule(nodeName);
  if(cacheRule == FEC_DFGCacheRule_Unspecified)
    cacheRule = subExec.getCacheRule();

  action = result->addAction("Edit");
  action = result->addAction("Rename");
  action = result->addAction("Delete");
  action = result->addAction("Save as Preset");
  result->addSeparator();
  action = result->addAction("Caching - Unspecified");
  action->setCheckable(true);
  if(cacheRule == FEC_DFGCacheRule_Unspecified)
    action->setChecked(true);
  action = result->addAction("Caching - Never");
  action->setCheckable(true);
  if(cacheRule == FEC_DFGCacheRule_Never)
    action->setChecked(true);
  action = result->addAction("Caching - Always");
  action->setCheckable(true);
  if(cacheRule == FEC_DFGCacheRule_Always)
    action->setChecked(true);

  bool hasSep = false;
  const std::vector<GraphView::Node*> & nodes = graphWidget->getUIController()->graph()->selectedNodes();
  if(nodes.size() > 0)
  {
    if(!hasSep)
    {
      result->addSeparator();
      hasSep = true;
    }
    result->addAction("Implode nodes");
  }
  if(subExec.getType() == FabricCore::DFGExecType_Graph)
  {
    if(!hasSep)
    {
      result->addSeparator();
      hasSep = true;
    }
    result->addAction("Explode node");
  }

  if(subExec.getExtDepCount() > 0)
  {
    result->addSeparator();
    result->addAction("Reload Extension(s)");
  }

  graphWidget->connect(result, SIGNAL(triggered(QAction*)), graphWidget, SLOT(onNodeAction(QAction*)));
  return result;
}

QMenu* DFGWidget::portContextMenuCallback(FabricUI::GraphView::Port* port, void* userData)
{
  DFGWidget * graphWidget = (DFGWidget*)userData;
  GraphView::Graph * graph = graphWidget->m_uiGraph;
  if(graph->controller() == NULL)
    return NULL;
  graphWidget->m_contextPort = port;
  QMenu* result = new QMenu(NULL);
  result->addAction("Edit");
  result->addAction("Delete");

  graphWidget->connect(result, SIGNAL(triggered(QAction*)), graphWidget, SLOT(onExecPortAction(QAction*)));
  return result;
}

QMenu* DFGWidget::sidePanelContextMenuCallback(FabricUI::GraphView::SidePanel* panel, void* userData)
{
  DFGWidget * graphWidget = (DFGWidget*)userData;
  GraphView::Graph * graph = graphWidget->m_uiGraph;
  if(graph->controller() == NULL)
    return NULL;
  graphWidget->m_contextPortType = panel->portType();
  QMenu* result = new QMenu(NULL);
  result->addAction("Create Port");
  graphWidget->connect(result, SIGNAL(triggered(QAction*)), graphWidget, SLOT(onSidePanelAction(QAction*)));
  return result;
}

void DFGWidget::onGoUpPressed()
{
  if ( m_execPath.empty() )
    return;

  std::pair<FTL::StrRef, FTL::StrRef> split =
    FTL::StrRef( m_execPath ).rsplit('.');
  std::string newExecPath = split.first;

  FabricCore::DFGExec newExec =
    m_binding.getExec().getSubExec( newExecPath.c_str() );
  
  maybeEditNode( newExecPath, newExec );
}

void DFGWidget::onGraphAction(QAction * action)
{
  QPointF mouseOffset(-40, -15);
  QPointF pos = m_uiGraphViewWidget->mapToScene(m_uiGraphViewWidget->mapFromGlobal(QCursor::pos()));
  pos = m_uiGraph->itemGroup()->mapFromScene(pos);
  pos += mouseOffset;

  if(action->text() == "New empty graph")
  {
    DFGGetStringDialog dialog(this, "graph", m_dfgConfig);
    if(dialog.exec() != QDialog::Accepted)
      return;

    QString text = dialog.text();
    if(text.length() == 0)
      return;

    m_uiController->cmdAddInstWithEmptyGraph(
      text.toUtf8().constData(),
      QPointF(pos.x(), pos.y())
      );
  }
  else if(action->text() == "New empty function")
  {
    DFGGetStringDialog dialog(this, "func", m_dfgConfig);
    if(dialog.exec() != QDialog::Accepted)
      return;

    QString text = dialog.text();
    if(text.length() == 0)
      return;

    m_uiController->beginInteraction();

    static const FTL::CStrRef initialCode = FTL_STR("\
dfgEntry {\n\
  // result = a + b;\n\
}\n");
    FTL::CStrRef nodeName =
      m_uiController->cmdAddInstWithEmptyFunc(
        text.toUtf8().constData(),
        initialCode,
        QPointF(pos.x(), pos.y())
        );
    GraphView::Node * uiNode = m_uiGraph->node(nodeName);
    if(uiNode)
    {
      std::string newExecPath = m_execPath;
      if ( !newExecPath.empty() )
        newExecPath += '.';
      newExecPath += nodeName;

      FabricCore::DFGExec newExec = m_exec.getSubExec( nodeName.c_str() );

      maybeEditNode( newExecPath, newExec );
    }
    m_uiController->endInteraction();
  }
  else if(action->text() == "Implode nodes")
  {
    DFGGetStringDialog dialog(this, "graph", m_dfgConfig);
    if(dialog.exec() != QDialog::Accepted)
      return;

    QString text = dialog.text();
    if(text.length() == 0)
      return;

    const std::vector<GraphView::Node*> & nodes =
      m_uiController->graph()->selectedNodes();

    std::vector<FTL::CStrRef> nodeNames;
    nodeNames.reserve( nodes.size() );
    for ( size_t i = 0; i < nodes.size(); ++i )
      nodeNames.push_back( nodes[i]->name() );

    std::string newNodeName =
      m_uiController->cmdImplodeNodes(
        text.toUtf8().constData(),
        nodeNames
        );

    m_uiGraph->clearSelection();
    if ( GraphView::Node *uiNode = m_uiGraph->node( newNodeName ) )
      uiNode->setSelected( true );
  }
  else if(action->text() == "New Variable")
  {
    DFGController *controller = getUIController();
    FabricCore::Client client = controller->getClient();
    FabricCore::DFGBinding binding = controller->getBinding();

    DFGNewVariableDialog dialog(this, client, binding, controller->getExecPath());
    if(dialog.exec() != QDialog::Accepted)
      return;

    QString name = dialog.name();
    if(name.length() == 0)
      return;
    QString dataType = dialog.dataType();
    QString extension = dialog.extension();


    m_uiController->cmdAddVar(
      name.toUtf8().constData(), 
      dataType.toUtf8().constData(), 
      extension.toUtf8().constData(), 
      pos
      );

    pos += QPointF(30, 30);
  }
  else if(action->text() == "Read Variable (Get)")
  {
    // todo: show an auto completing text field
    m_uiController->cmdAddGet(
      "get",
      "",
      QPointF(pos.x(), pos.y())
      );
  }
  else if(action->text() == "Write Variable (Set)")
  {
    // todo: show an auto completing text field
    m_uiController->cmdAddSet(
      "set",
      "",
      QPointF(pos.x(), pos.y())
      );
  }
}

void DFGWidget::onNodeAction(QAction * action)
{
  if(m_contextNode == NULL)
    return;

  char const * nodeName = m_contextNode->name().c_str();
  if(action->text() == "Edit")
  {
    if(m_exec.getNodeType(nodeName) != FabricCore::DFGNodeType_Inst)
      return;

    std::string newExecPath = m_execPath;
    if ( !newExecPath.empty() )
      newExecPath += '.';
    newExecPath += nodeName;

    FabricCore::DFGExec newExec = m_exec.getSubExec( nodeName );

    maybeEditNode( newExecPath, newExec );
  }
  else if(action->text() == "Rename")
  {
    onNodeToBeRenamed(m_contextNode);
  }
  else if(action->text() == "Delete")
  {
    m_uiController->gvcDoRemoveNodes(m_contextNode);
  }
  else if(action->text() == "Save as Preset")
  {
    if(m_exec.getNodeType(nodeName) != FabricCore::DFGNodeType_Inst)
      return;
    FabricCore::DFGExec subExec = m_exec.getSubExec(nodeName);

    QString title = subExec.getTitle();
    if(title.toLower().endsWith(".dfg.json"))
      title = title.left(title.length() - 9);

    QString lastPresetFolder = title;
    if(getSettings())
    {
      lastPresetFolder = getSettings()->value("DFGWidget/lastPresetFolder").toString();
      lastPresetFolder += "/" + title;
    }

    QString filter = "DFG Preset (*.dfg.json)";
    QString filePath = QFileDialog::getSaveFileName(this, "Save preset", lastPresetFolder, filter, &filter);
    if(filePath.length() == 0)
      return;
    if(filePath.toLower().endsWith(".dfg.json.dfg.json"))
      filePath = filePath.left(filePath.length() - 9);

    if(getSettings())
    {
      QDir dir(filePath);
      dir.cdUp();
      getSettings()->setValue( "DFGWidget/lastPresetFolder", dir.path() );
    }

    std::string filePathStr = filePath.toUtf8().constData();

    try
    {
      // copy all defaults
      for(unsigned int i=0;i<subExec.getExecPortCount();i++)
      {
        std::string pinPath = nodeName;
        pinPath += ".";
        pinPath += subExec.getExecPortName(i);

        FTL::StrRef rType = m_exec.getNodePortResolvedType(pinPath.c_str());
        if(rType.size() == 0 || rType.find('$') >= 0)
          continue;
        FabricCore::RTVal val =
          m_exec.getInstPortResolvedDefaultValue(pinPath.c_str(), rType.data());
        if(val.isValid())
          subExec.setPortDefaultValue(subExec.getExecPortName(i), val);
      }

      std::string json = subExec.exportJSON().getCString();
      FILE * file = fopen(filePathStr.c_str(), "wb");
      if(file)
      {
        fwrite(json.c_str(), json.length(), 1, file);
        fclose(file);
      }

      subExec.setImportPathname(filePathStr.c_str());
      subExec.attachPresetFile("", subExec.getTitle());

      emit newPresetSaved(filePathStr.c_str());
    }
    catch(FabricCore::Exception e)
    {
      printf("Exception: %s\n", e.getDesc_cstr());
    }
  }
  else if(action->text() == "Caching - Unspecified")
  {
    m_uiController->setNodeCacheRule(nodeName, FEC_DFGCacheRule_Unspecified);
  }
  else if(action->text() == "Caching - Never")
  {
    m_uiController->setNodeCacheRule(nodeName, FEC_DFGCacheRule_Never);
  }
  else if(action->text() == "Caching - Always")
  {
    m_uiController->setNodeCacheRule(nodeName, FEC_DFGCacheRule_Always);
  }
  else if(action->text() == "Implode nodes")
  {
    DFGGetStringDialog dialog(this, "graph", m_dfgConfig);
    if(dialog.exec() != QDialog::Accepted)
      return;

    QString text = dialog.text();
    if(text.length() == 0)
      return;

    const std::vector<GraphView::Node*> & nodes =
      m_uiController->graph()->selectedNodes();

    std::vector<FTL::CStrRef> nodeNames;
    nodeNames.reserve( nodes.size() );
    for ( size_t i = 0; i < nodes.size(); ++i )
      nodeNames.push_back( nodes[i]->name() );

    std::string newNodeName =
      m_uiController->cmdImplodeNodes(
        text.toUtf8().constData(),
        nodeNames
        );

    m_uiGraph->clearSelection();
    if ( GraphView::Node *uiNode = m_uiGraph->node( newNodeName ) )
      uiNode->setSelected( true );
  }
  else if(action->text() == "Explode node")
  {
    std::vector<std::string> newNodeNames =
      m_uiController->cmdExplodeNode( nodeName );

    m_uiGraph->clearSelection();
    for ( std::vector<std::string>::const_iterator it = newNodeNames.begin();
      it != newNodeNames.end(); ++it )
    {
      if ( GraphView::Node *uiNode = m_uiGraph->node( *it ) )
        uiNode->setSelected( true );
    }
  }
  else if(action->text() == "Reload Extension(s)")
  {
    m_uiController->reloadExtensionDependencies(nodeName);
  }


  m_contextNode = NULL;
}

void DFGWidget::onNodeEditRequested(FabricUI::GraphView::Node * node)
{
  try
  {
    FTL::CStrRef nodeName = node->name();

    if ( m_exec.getNodeType( nodeName.c_str() ) != FabricCore::DFGNodeType_Inst )
      return;

    std::string newExecPath = m_execPath;
    if ( !newExecPath.empty() )
      newExecPath += '.';
    newExecPath += nodeName;

    FabricCore::DFGExec newExec = m_exec.getSubExec( nodeName.c_str() );

    maybeEditNode( newExecPath, newExec );
  }
  catch(FabricCore::Exception e)
  {
    printf("Exception: %s\n", e.getDesc_cstr());
  }
}

void DFGWidget::onExecPortAction(QAction * action)
{
  if(m_contextPort == NULL)
    return;

  char const * portName = m_contextPort->name().c_str();
  if(action->text() == "Delete")
  {
    m_uiController->cmdRemovePort( portName );
  }
  else if(action->text() == "Edit")
  {
    try
    {
      DFGEditPortDialog dialog(this, m_client, false, m_dfgConfig);

      dialog.setTitle(portName);
      dialog.setDataType(m_exec.getExecPortResolvedType(portName));

      FTL::StrRef uiNativeArray = m_exec.getExecPortMetadata(portName, "uiNativeArray");
      if(uiNativeArray == "true")
        dialog.setNative(true);
      FTL::StrRef uiHidden = m_exec.getExecPortMetadata(portName, "uiHidden");
      if(uiHidden == "true")
        dialog.setHidden();
      FTL::StrRef uiOpaque = m_exec.getExecPortMetadata(portName, "uiOpaque");
      if(uiOpaque == "true")
        dialog.setOpaque();
      FTL::StrRef uiRange = m_exec.getExecPortMetadata(portName, "uiRange");
      if(uiRange.size() > 0)
      {
        QString filteredUiRange;
        for(unsigned int i=0;i<uiRange.size();i++)
        {
          char c = uiRange[i];
          if(isalnum(c) || c == '.' || c == ',' || c == '-')
            filteredUiRange += c;
        }

        QStringList parts = filteredUiRange.split(',');
        if(parts.length() == 2)
        {
          float minimum = parts[0].toFloat();
          float maximum = parts[1].toFloat();
          dialog.setHasRange(true);
          dialog.setRangeMin(minimum);
          dialog.setRangeMax(maximum);
        }
      }
      FTL::StrRef uiCombo = m_exec.getExecPortMetadata(portName, "uiCombo");
      if(uiCombo.size() > 0)
      {
        if(uiCombo[0] == '(');
          uiCombo = uiCombo.substr(1);
        if(uiCombo[uiCombo.size()-1] == ')')
          uiCombo = uiCombo.substr(0, uiCombo.size()-1);

        QStringList parts = QString(uiCombo.data()).split(',');
        dialog.setHasCombo(true);
        dialog.setComboValues(parts);
      }

      if(dialog.exec() != QDialog::Accepted)
        return;

      if(dialog.native())
        m_exec.setExecPortMetadata(portName, "uiNativeArray", "true", false);
      else if(uiNativeArray.size() > 0)
        m_exec.setExecPortMetadata(portName, "uiNativeArray", NULL, false);
      if(dialog.hidden())
        m_exec.setExecPortMetadata(portName, "uiHidden", "true", false);
      else if(uiHidden.size() > 0)
        m_exec.setExecPortMetadata(portName, "uiHidden", NULL, false);
      if(dialog.opaque())
        m_exec.setExecPortMetadata(portName, "uiOpaque", "true", false);
      else if(uiOpaque.size() > 0)
        m_exec.setExecPortMetadata(portName, "uiOpaque", NULL, false);
      if(dialog.hasRange())
      {
        QString range = "(" + QString::number(dialog.rangeMin()) + ", " + QString::number(dialog.rangeMax()) + ")";
        m_exec.setExecPortMetadata(portName, "uiRange", range.toUtf8().constData(), false);
      }
      else if(uiRange.size() > 0)
      {
        m_exec.setExecPortMetadata(portName, "uiRange", NULL, false);
      }
      if(dialog.hasCombo())
      {
        QStringList combo = dialog.comboValues();
        QString flat = "(";
        for(unsigned int i=0;i<combo.length();i++)
        {
          if(i > 0)
            flat += ", ";
          flat += "\"" + combo[i] + "\"";
        }
        flat += ")";
        m_exec.setExecPortMetadata(portName, "uiCombo", flat.toUtf8().constData(), false);
      }
      else if(uiCombo.size() > 0)
      {
        m_exec.setExecPortMetadata(portName, "uiCombo", NULL, false);
      }

      m_uiController->beginInteraction();
      if(dialog.dataType().length() > 0 && dialog.dataType() != m_exec.getExecPortResolvedType(portName))
      {
        if(m_uiController->isViewingRootGraph())
          m_uiController->setArg(portName, dialog.dataType().toUtf8().constData());
      }

      if(dialog.title() != portName)
      {
        m_uiController->renamePort(portName, dialog.title().toUtf8().constData());
      }
      m_uiController->endInteraction();

      emit m_uiController->structureChanged();
    }
    catch(FabricCore::Exception e)
    {
      printf("Exception: %s\n", e.getDesc_cstr());
    }


    // QString dataType = dialog.dataType();
    // QString extension = dialog.extension();

    // if(extension.length() > 0)
    // {
    //   m_uiController->addExtensionDependency(extension, m_uiGraph->path());
    // }
    // if(m_uiController->setArg(m_contextPort->name(), dataType))
    // {
    //   // setup the value editor
    // }
  }

  m_contextPort = NULL;
}

void DFGWidget::onSidePanelAction(QAction * action)
{
  if(action->text() == "Create Port")
  {
    DFGEditPortDialog dialog(this, m_client, true, m_dfgConfig);
    if(m_contextPortType == FabricUI::GraphView::PortType_Output)
      dialog.setPortType("In");
    else
      dialog.setPortType("Out");
    if(dialog.exec() != QDialog::Accepted)
      return;

    QString title = dialog.title();
    QString dataType = dialog.dataType();
    QString extension = dialog.extension();

    if(title.length() > 0)
    {
      if(extension.length() > 0)
      {
        std::string errorMessage;
        if(!m_uiController->addExtensionDependency(extension.toUtf8().constData(), m_execPath.c_str(), errorMessage))
        {
          QMessageBox msg(QMessageBox::Warning, "Fabric Warning", 
            errorMessage.c_str());
          msg.addButton("Ok", QMessageBox::AcceptRole);
          msg.exec();
          return;
        }
      }

      FabricCore::DFGPortType portType = FabricCore::DFGPortType_Out;
      if ( dialog.portType() == "In" )
        portType = FabricCore::DFGPortType_In;
      else if ( dialog.portType() == "IO" )
        portType = FabricCore::DFGPortType_IO;

      std::string portName =
        m_uiController->cmdAddPort(
          title.toUtf8().constData(),
          portType, 
          dataType.toUtf8().constData(),
          FTL::CStrRef() // portToConnect
          );

      try
      {
        if(dialog.native())
          m_exec.setExecPortMetadata(portName.c_str(), "uiNativeArray", "true", false);
        if(dialog.hidden())
          m_exec.setExecPortMetadata(portName.c_str(), "uiHidden", "true", false);
        if(dialog.opaque())
          m_exec.setExecPortMetadata(portName.c_str(), "uiOpaque", "true", false);
        if(dialog.hasRange())
        {
          QString range = "(" + QString::number(dialog.rangeMin()) + ", " + QString::number(dialog.rangeMax()) + ")";
          m_exec.setExecPortMetadata(portName.c_str(), "uiRange", range.toUtf8().constData(), false);
        }
        if(dialog.hasCombo())
        {
          QStringList combo = dialog.comboValues();
          QString flat = "(";
          for(unsigned int i=0;i<combo.length();i++)
          {
            if(i > 0)
              flat += ", ";
            flat += "\"" + combo[i] + "\"";
          }
          flat += ")";
          m_exec.setExecPortMetadata(portName.c_str(), "uiCombo", flat.toUtf8().constData(), false);
        }

        emit m_uiController->structureChanged();
      }
      catch(FabricCore::Exception e)
      {
        printf("Exception: %s\n", e.getDesc_cstr());
      }
    }
  }
}

void DFGWidget::onHotkeyPressed(Qt::Key key, Qt::KeyboardModifier mod, QString name)
{
  if(name == "PanGraph")
  {
    m_uiGraph->mainPanel()->setSpaceBarDown(true);
  }
}

void DFGWidget::onHotkeyReleased(Qt::Key key, Qt::KeyboardModifier mod, QString name)
{
  if(name == "PanGraph")
  {
    m_uiGraph->mainPanel()->setSpaceBarDown(false);
  }
}

void DFGWidget::onNodeToBeRenamed(FabricUI::GraphView::Node* node)
{
  DFGGetStringDialog dialog(
    this,
    QString( node->title().c_str() ),
    m_dfgConfig
    );
  if(dialog.exec() != QDialog::Accepted)
    return;

  QString text = dialog.text();
  m_uiController->renameNode(node, text.toUtf8().constData());
}

void DFGWidget::onKeyPressed(QKeyEvent * event)
{
  if(getUIGraph() && getUIGraph()->pressHotkey((Qt::Key)event->key(), (Qt::KeyboardModifier)(int)event->modifiers()))
    event->accept();
  else
    keyPressEvent(event);  
}

bool DFGWidget::maybeEditNode(
  FTL::CStrRef newExecPath,
  FabricCore::DFGExec const &newExec
  )
{
  try
  {
    if(m_klEditor->isVisible() && m_klEditor->hasUnsavedChanges())
    {
      QMessageBox msg(QMessageBox::Warning, "Fabric Warning", 
        "You haven't compiled the code.\nYou are going to lose the changes.\nSure?");

      msg.addButton("Compile Now", QMessageBox::AcceptRole);
      msg.addButton("Ok", QMessageBox::NoRole);
      msg.addButton("Cancel", QMessageBox::RejectRole);

      msg.exec();

      QString clicked = msg.clickedButton()->text();
      if(clicked == "Cancel")
        return false;
      else if(clicked == "Compile Now")
      {
        m_klEditor->compile();
        if(m_klEditor->hasUnsavedChanges())
          return false;
      }
    }

    if(newExec.getType() == FabricCore::DFGExecType_Graph)
    {
      setGraph( m_host, m_binding, newExecPath, newExec );
      m_uiGraphViewWidget->show();
      m_uiGraphViewWidget->setFocus();
      m_klEditor->hide();      
    }
    else if(newExec.getType() == FabricCore::DFGExecType_Func)
    {
      std::pair<FTL::StrRef, FTL::CStrRef> split = newExecPath.rsplit('.');
      m_uiHeader->setCaption( split.second.c_str() );

      m_uiGraphViewWidget->hide();
      m_klEditor->show();      
      m_klEditor->klEditor()->sourceCodeWidget()->setFocus();

      setGraph( m_host, m_binding, newExecPath, newExec );
      m_klEditor->setFunc(newExec, newExecPath.c_str());
    }
  }
  catch(FabricCore::Exception e)
  {
    printf("Exception: %s\n", e.getDesc_cstr());
    return false;
  }
  return true;  
}

QSettings * DFGWidget::getSettings()
{
  return g_settings;
}

void DFGWidget::setSettings(QSettings * settings)
{
  g_settings = settings;
}
