// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <assert.h>
#include <FabricCore.h>
#include <FabricUI/DFG/DFGMainWindow.h>
#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/DFG/DFGWidget.h>
#include <FabricUI/DFG/Dialogs/DFGEditPortDialog.h>
#include <FabricUI/DFG/Dialogs/DFGGetStringDialog.h>
#include <FabricUI/DFG/Dialogs/DFGGetTextDialog.h>
#include <FabricUI/DFG/Dialogs/DFGNewVariableDialog.h>
#include <FabricUI/DFG/Dialogs/DFGSavePresetDialog.h>
#include <FabricUI/GraphView/NodeBubble.h>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtGui/QColorDialog>
#include <QtGui/QCursor>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QVBoxLayout>

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

QSettings * DFGWidget::g_settings = NULL;

DFGWidget::DFGWidget(
  QWidget * parent, 
  FabricCore::Client &client,
  FabricCore::DFGHost &host,
  FabricCore::DFGBinding &binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec &exec,
  FabricServices::ASTWrapper::KLASTManager * manager,
  DFGUICmdHandler *cmdHandler,
  Commands::CommandStack * stack,
  const DFGConfig & dfgConfig,
  bool overTakeBindingNotifications
  )
  : QWidget( parent )
  , m_uiGraph( 0 )
  , m_router( 0 )
  , m_manager( manager )
  , m_dfgConfig( dfgConfig )
{
  m_uiController = new DFGController(
    0,
    this,
    client,
    m_manager,
    cmdHandler,
    stack,
    overTakeBindingNotifications
    );

  QObject::connect(
    m_uiController.get(), SIGNAL( execChanged() ),
    this, SLOT( onExecChanged() )
    );

  m_uiHeader =
    new DFGExecHeaderWidget(
      this,
      m_uiController.get(),
      "Graph",
      dfgConfig.graphConfig
      );
  QObject::connect(
    this, SIGNAL( execChanged() ),
    m_uiHeader, SLOT( refresh() )
    );

  m_uiGraphViewWidget = new DFGGraphViewWidget(this, dfgConfig.graphConfig, NULL);

  m_klEditor =
    new DFGKLEditorWidget(
      this,
      m_uiController.get(),
      m_manager,
      m_dfgConfig
      );

  m_isEditable = true;
  if(binding.isValid())
  {
    FTL::StrRef editable = binding.getMetadata("editable");
    if(editable == "false")
      m_isEditable = false;
  }

  if(m_isEditable)
  {
    m_tabSearchWidget = new DFGTabSearchWidget(this, m_dfgConfig);
    m_tabSearchWidget->hide();
  }

  QVBoxLayout * layout = new QVBoxLayout();
  layout->setSpacing(0);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(m_uiHeader);
  layout->addWidget(m_uiGraphViewWidget);
  layout->addWidget(m_klEditor);
  layout->setContentsMargins(0, 0, 0, 0);
  setLayout(layout);
  setContentsMargins(0, 0, 0, 0);

  if(m_isEditable)
  {
    QObject::connect(
      m_uiHeader, SIGNAL(goUpPressed()),
      this, SLOT(onGoUpPressed())
      );

    QObject::connect(
      m_uiController.get(), SIGNAL(nodeEditRequested(FabricUI::GraphView::Node *)), 
      this, SLOT(onNodeEditRequested(FabricUI::GraphView::Node *))
      );
  }

  m_uiController->setHostBindingExec( host, binding, execPath, exec );
}

DFGWidget::~DFGWidget()
{
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
  result->addAction("New backdrop");

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
  result->addAction("Cache Node");

  graphWidget->connect(result, SIGNAL(triggered(QAction*)), graphWidget, SLOT(onGraphAction(QAction*)));
  return result;
}

QMenu* DFGWidget::nodeContextMenuCallback(FabricUI::GraphView::Node* uiNode, void* userData)
{
  DFGWidget * graphWidget = (DFGWidget*)userData;
  FabricCore::DFGExec &exec = graphWidget->m_uiController->getExec();

  GraphView::Graph * graph = graphWidget->m_uiGraph;
  if(graph->controller() == NULL)
    return NULL;
  graphWidget->m_contextNode = uiNode;

  char const * nodeName = uiNode->name().c_str();

  if(uiNode->type() == GraphView::QGraphicsItemType_Node)
  {
    FabricCore::DFGNodeType dfgNodeType = exec.getNodeType( nodeName );
    if ( dfgNodeType != FabricCore::DFGNodeType_Inst
      && dfgNodeType != FabricCore::DFGNodeType_User )
      return NULL;
  }

  QMenu* result = new QMenu(NULL);
  QAction* action;

  if(uiNode->type() == GraphView::QGraphicsItemType_Node)
  {
    action = result->addAction("Edit");
  }
  action = result->addAction("Edit Title");
  action = result->addAction("Delete");

  if ( !uiNode->isBackDropNode() )
  {
    result->addSeparator();
    action = result->addAction("Save Preset");
    action = result->addAction("Export JSON");

    FabricCore::DFGExec subExec = exec.getSubExec( nodeName );

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
  }
  else
  {
    result->addSeparator();
    action = result->addAction("Change color");
  }

  result->addSeparator();
  action = result->addAction("Add Comment");
  action = result->addAction("Remove Comment");
  (void)action;

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
  result->addAction("Rename");
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
  FTL::StrRef execPath = m_uiController->getExecPath();
  if ( execPath.empty() )
    return;

  FTL::StrRef::Split split = execPath.rsplit('.');
  std::string parentExecPath = split.first;

  FabricCore::DFGBinding &binding = m_uiController->getBinding();
  FabricCore::DFGExec rootExec = binding.getExec();
  FabricCore::DFGExec parentExec =
    rootExec.getSubExec( parentExecPath.c_str() );

  maybeEditNode( parentExecPath, parentExec );
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
      std::string subExecPath = m_uiController->getExecPath();
      if ( !subExecPath.empty() )
        subExecPath += '.';
      subExecPath += uiNode->name();

      FabricCore::DFGExec subExec =
        m_uiController->getExec().getSubExec( uiNode->name().c_str() );

      maybeEditNode( subExecPath, subExec );
    }
    m_uiController->endInteraction();
  }
  else if(action->text() == "New backdrop")
  {
    DFGGetStringDialog dialog(this, "backdrop", m_dfgConfig);
    if(dialog.exec() != QDialog::Accepted)
      return;

    QString text = dialog.text();
    if(text.length() == 0)
      return;

    m_uiController->cmdAddBackDrop(
      text.toUtf8().constData(),
      QPointF( pos.x(), pos.y() )
      );
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
    FabricCore::DFGBinding &binding = controller->getBinding();
    FTL::CStrRef execPath = controller->getExecPath();

    DFGNewVariableDialog dialog( this, client, binding, execPath );
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
  else if(action->text() == "Cache Node")
  {
    DFGController * controller = getUIController();
    controller->cmdAddInstFromPreset("Fabric.Core.Data.Cache", QPointF(pos.x(), pos.y()));
    pos += QPointF(30, 30);
  }
}

void DFGWidget::onNodeAction(QAction * action)
{
  if(m_contextNode == NULL)
    return;

  char const * nodeName = m_contextNode->name().c_str();
  if(action->text() == "Edit")
  {
    FabricCore::DFGExec &exec = m_uiController->getExec();
    if ( exec.getNodeType(nodeName) != FabricCore::DFGNodeType_Inst )
      return;

    std::string subExecPath = m_uiController->getExecPath();
    if ( !subExecPath.empty() )
      subExecPath += '.';
    subExecPath += nodeName;

    FabricCore::DFGExec subExec = exec.getSubExec( nodeName );

    maybeEditNode( subExecPath, subExec );
  }
  else if(action->text() == "Edit Title")
  {
    onEditNodeTitle(m_contextNode);
  }
  else if(action->text() == "Delete")
  {
    m_uiController->gvcDoRemoveNodes(m_contextNode);
  }
  else if(action->text() == "Export JSON")
  {
    FabricCore::DFGExec &exec = m_uiController->getExec();
    if ( exec.getNodeType(nodeName) != FabricCore::DFGNodeType_Inst )
      return;

    FabricCore::DFGExec subExec = exec.getSubExec( nodeName );

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
    QString filePath = QFileDialog::getSaveFileName(this, "Export JSON", lastPresetFolder, filter, &filter);
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

        FTL::StrRef rType = exec.getNodePortResolvedType(pinPath.c_str());
        if(rType.size() == 0 || rType.find('$') >= 0)
          continue;
        if(rType.size() == 0 || rType.find('$') != rType.end())
          rType = subExec.getExecPortResolvedType(i);
        if(rType.size() == 0 || rType.find('$') != rType.end())
          rType = subExec.getExecPortTypeSpec(i);
        if(rType.size() == 0 || rType.find('$') != rType.end())
          continue;
        FabricCore::RTVal val =
          exec.getInstPortResolvedDefaultValue(pinPath.c_str(), rType.data());
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
    }
    catch(FabricCore::Exception e)
    {
      printf("Exception: %s\n", e.getDesc_cstr());
    }
  }
  else if(action->text() == "Save Preset")
  {
    FabricCore::DFGExec &exec = m_uiController->getExec();
    if ( exec.getNodeType( nodeName ) != FabricCore::DFGNodeType_Inst )
      return;

    try
    {
      FabricCore::DFGExec subExec = exec.getSubExec(nodeName);
      QString title = subExec.getTitle();

      FabricCore::DFGHost &host = m_uiController->getHost();

      DFGSavePresetDialog dialog( this, m_uiController.get(), title );

      while(true)
      {
        if(dialog.exec() != QDialog::Accepted)
          return;
  
        QString name = dialog.name();
        // QString version = dialog.version();
        QString location = dialog.location();

        if(name.length() == 0 || location.length() == 0)
        {
          QMessageBox msg(QMessageBox::Warning, "Fabric Warning", 
            "You need to provide a valid name and pick a valid location!");
          msg.addButton("Ok", QMessageBox::AcceptRole);
          msg.exec();
          continue;
        }

        if(location.startsWith("Fabric.") || location.startsWith("Variables.") ||
          location == "Fabric" || location == "Variables")
        {
          QMessageBox msg(QMessageBox::Warning, "Fabric Warning", 
            "You can't save a preset into a factory path (below Fabric).");
          msg.addButton("Ok", QMessageBox::AcceptRole);
          msg.exec();
          continue;
        }

        std::string filePathStr = host.getPresetImportPathname(location.toUtf8().constData());
        filePathStr += "/";
        filePathStr += name.toUtf8().constData();
        filePathStr += ".dfg.json";

        FILE * file = fopen(filePathStr.c_str(), "rb");
        if(file)
        {
          fclose(file);
          file = NULL;

          QMessageBox msg(QMessageBox::Warning, "Fabric Warning", 
            "The file "+QString(filePathStr.c_str())+" already exists.\nAre you sure to overwrite the file?");
          msg.addButton("Cancel", QMessageBox::RejectRole);
          msg.addButton("Ok", QMessageBox::AcceptRole);
          if(msg.exec() != QDialog::Accepted)
            continue;
        }

        subExec.setTitle(name.toUtf8().constData());

        // copy all defaults
        for(unsigned int i=0;i<subExec.getExecPortCount();i++)
        {
          std::string pinPath = nodeName;
          pinPath += ".";
          pinPath += subExec.getExecPortName(i);

          FTL::StrRef rType = exec.getNodePortResolvedType(pinPath.c_str());
          if(rType.size() == 0 || rType.find('$') != rType.end())
            rType = subExec.getExecPortResolvedType(i);
          if(rType.size() == 0 || rType.find('$') != rType.end())
            rType = subExec.getExecPortTypeSpec(i);
          if(rType.size() == 0 || rType.find('$') != rType.end())
            continue;
          FabricCore::RTVal val =
            exec.getInstPortResolvedDefaultValue(pinPath.c_str(), rType.data());
          if(val.isValid())
            subExec.setPortDefaultValue(subExec.getExecPortName(i), val);
        }

        std::string json = subExec.exportJSON().getCString();
        file = fopen(filePathStr.c_str(), "wb");
        if(!file)
        {
          QMessageBox msg(QMessageBox::Warning, "Fabric Warning", 
            "The file "+QString(filePathStr.c_str())+" cannot be opened for writing.");
          msg.addButton("Ok", QMessageBox::AcceptRole);
          msg.exec();
          continue;
        }

        fwrite(json.c_str(), json.length(), 1, file);
        fclose(file);

        subExec.setImportPathname(filePathStr.c_str());
        subExec.attachPresetFile(location.toUtf8().constData(), name.toUtf8().constData());

        emit newPresetSaved(filePathStr.c_str());
        // update the preset search paths within the controller
        m_uiController->onVariablesChanged();
        break;
      }
    }
    catch(FabricCore::Exception e)
    {
      QMessageBox msg(QMessageBox::Warning, "Fabric Warning", 
        e.getDesc_cstr());
      msg.addButton("Ok", QMessageBox::AcceptRole);
      msg.exec();
      return;
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
  else if(action->text() == "Change color")
  {
    QColor color = m_contextNode->color();
    color = QColorDialog::getColor(color, this);
    m_uiController->tintBackDropNode((GraphView::BackDropNode*)m_contextNode, color);
  }
  else if(action->text() == "Add Comment")
  {
    onBubbleEditRequested(m_contextNode);
  }
  else if(action->text() == "Remove Comment")
  {
    m_uiController->cmdSetNodeComment( m_contextNode->name(), NULL );
  }

  m_contextNode = NULL;
}

void DFGWidget::onNodeEditRequested(FabricUI::GraphView::Node * node)
{
  try
  {
    FTL::CStrRef nodeName = node->name();

    FabricCore::DFGExec &exec = m_uiController->getExec();
    if ( exec.getNodeType( nodeName.c_str() )
      != FabricCore::DFGNodeType_Inst )
      return;

    std::string subExecPath = m_uiController->getExecPath();
    if ( !subExecPath.empty() )
      subExecPath += '.';
    subExecPath += nodeName;

    FabricCore::DFGExec subExec = exec.getSubExec( nodeName.c_str() );
    maybeEditNode( subExecPath, subExec );
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
  else if ( action->text() == "Rename" )
  {
    DFGBaseDialog dialog( this );
    QLineEdit *lineEdit = new QLineEdit();
    lineEdit->setText( portName );
    lineEdit->selectAll();
    QObject::connect(
      lineEdit, SIGNAL(returnPressed()),
      &dialog, SLOT(accept())
      );
    dialog.addInput( lineEdit, "Desired name" );

    if ( dialog.exec() == QDialog::Accepted )
    {
      QString desiredNewName = lineEdit->text();
      m_uiController->cmdRenameExecPort(
        portName,
        desiredNewName.toUtf8().constData()
        );
    }
  }
  else if(action->text() == "Edit")
  {
    try
    {
      FabricCore::Client &client = m_uiController->getClient();
      FabricCore::DFGExec &exec = m_uiController->getExec();

      DFGEditPortDialog dialog( this, client, false, m_uiController->isViewingRootGraph(), m_dfgConfig );

      dialog.setTitle(portName);

      if(m_uiController->isViewingRootGraph())
        dialog.setDataType(exec.getExecPortResolvedType(portName));

      FTL::StrRef uiHidden = exec.getExecPortMetadata(portName, "uiHidden");
      if(uiHidden == "true")
        dialog.setHidden();
      FTL::StrRef uiOpaque = exec.getExecPortMetadata(portName, "uiOpaque");
      if(uiOpaque == "true")
        dialog.setOpaque();
      FTL::StrRef uiRange = exec.getExecPortMetadata(portName, "uiRange");
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
      FTL::StrRef uiCombo = exec.getExecPortMetadata(portName, "uiCombo");
      if(uiCombo.size() > 0)
      {
        if(uiCombo[0] == '(')
          uiCombo = uiCombo.substr(1);
        if(uiCombo[uiCombo.size()-1] == ')')
          uiCombo = uiCombo.substr(0, uiCombo.size()-1);

        QStringList parts = QString(uiCombo.data()).split(',');
        dialog.setHasCombo(true);
        dialog.setComboValues(parts);
      }

      emit portEditDialogCreated(&dialog);

      if(dialog.exec() != QDialog::Accepted)
        return;

      emit portEditDialogInvoked(&dialog);

      if(dialog.hidden())
        exec.setExecPortMetadata(portName, "uiHidden", "true", false);
      else if(uiHidden.size() > 0)
        exec.setExecPortMetadata(portName, "uiHidden", NULL, false);
      if(dialog.opaque())
        exec.setExecPortMetadata(portName, "uiOpaque", "true", false);
      else if(uiOpaque.size() > 0)
        exec.setExecPortMetadata(portName, "uiOpaque", NULL, false);
      if(dialog.hasRange())
      {
        QString range = "(" + QString::number(dialog.rangeMin()) + ", " + QString::number(dialog.rangeMax()) + ")";
        exec.setExecPortMetadata(portName, "uiRange", range.toUtf8().constData(), false);
      }
      else if(uiRange.size() > 0)
      {
        exec.setExecPortMetadata(portName, "uiRange", NULL, false);
      }
      if(dialog.hasCombo())
      {
        QStringList combo = dialog.comboValues();
        QString flat = "(";
        for(int i=0;i<combo.length();i++)
        {
          if(i > 0)
            flat += ", ";
          flat += "\"" + combo[i] + "\"";
        }
        flat += ")";
        exec.setExecPortMetadata(portName, "uiCombo", flat.toUtf8().constData(), false);
      }
      else if(uiCombo.size() > 0)
      {
        exec.setExecPortMetadata(portName, "uiCombo", NULL, false);
      }

      m_uiController->beginInteraction();
      if(dialog.dataType().length() > 0 && dialog.dataType() != exec.getExecPortResolvedType(portName))
      {
        if(m_uiController->isViewingRootGraph())
          m_uiController->setArg(portName, dialog.dataType().toUtf8().constData());
      }

      if(dialog.title() != portName)
      {
        m_uiController->cmdRenameExecPort(
          portName,
          dialog.title().toUtf8().constData()
          );
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
    FabricCore::Client &client = m_uiController->getClient();
    FTL::CStrRef execPath = m_uiController->getExecPath();
    FabricCore::DFGExec &exec = m_uiController->getExec();

    DFGEditPortDialog dialog( this, client, true, m_uiController->isViewingRootGraph(), m_dfgConfig );

    if(m_contextPortType == FabricUI::GraphView::PortType_Output)
      dialog.setPortType("In");
    else
      dialog.setPortType("Out");

    emit portEditDialogCreated(&dialog);

    if(dialog.exec() != QDialog::Accepted)
      return;

    emit portEditDialogInvoked(&dialog);

    QString title = dialog.title();

    QString dataType = "";
    QString extension = "";

    if(m_uiController->isViewingRootGraph())
    {
      dataType = dialog.dataType();
      extension = dialog.extension();
    }

    if(title.length() > 0)
    {
      if(extension.length() > 0)
      {
        std::string errorMessage;
        if ( !m_uiController->addExtensionDependency(
          extension.toUtf8().constData(),
          execPath.c_str(),
          errorMessage
          ) )
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
        if(dialog.hidden())
          exec.setExecPortMetadata(portName.c_str(), "uiHidden", "true", false);
        if(dialog.opaque())
          exec.setExecPortMetadata(portName.c_str(), "uiOpaque", "true", false);
        if(dialog.hasRange())
        {
          QString range = "(" + QString::number(dialog.rangeMin()) + ", " + QString::number(dialog.rangeMax()) + ")";
          exec.setExecPortMetadata(portName.c_str(), "uiRange", range.toUtf8().constData(), false);
        }
        if(dialog.hasCombo())
        {
          QStringList combo = dialog.comboValues();
          QString flat = "(";
          for(int i=0;i<combo.length();i++)
          {
            if(i > 0)
              flat += ", ";
            flat += "\"" + combo[i] + "\"";
          }
          flat += ")";
          exec.setExecPortMetadata(portName.c_str(), "uiCombo", flat.toUtf8().constData(), false);
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

void DFGWidget::onEditNodeTitle(
  FabricUI::GraphView::Node *node
  )
{
  DFGGetStringDialog dialog(
    this,
    QString( node->title().c_str() ),
    m_dfgConfig
    );
  if ( dialog.exec() == QDialog::Accepted )
  {
    QString text = dialog.text();
    m_uiController->cmdSetNodeTitle(
      node->name(),
      text.toUtf8().constData()
      );
  }
}

void DFGWidget::onKeyPressed(QKeyEvent * event)
{
  if(getUIGraph() && getUIGraph()->pressHotkey((Qt::Key)event->key(), (Qt::KeyboardModifier)(int)event->modifiers()))
    event->accept();
  else
    keyPressEvent(event);  
}

void DFGWidget::onBubbleEditRequested(FabricUI::GraphView::Node * node)
{
  GraphView::NodeBubble * bubble = node->bubble();
  bool visible = bubble->isVisible();
  bool collapsed = bubble->collapsed();
  bubble->show();
  bubble->expand();

  QString text = bubble->text();
  DFGGetTextDialog dialog(this, text);
  if ( dialog.exec() == QDialog::Accepted )
  {
    text = dialog.text();
    m_uiController->cmdSetNodeComment(
      node->name(),
      text.toUtf8().constData()
      );
  }
  else
  {
    if ( collapsed )
      bubble->collapse();
    bubble->setVisible( visible );
  }
}

bool DFGWidget::maybeEditNode(
  FTL::StrRef execPath,
  FabricCore::DFGExec &exec
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

    m_uiController->setExec( execPath, exec );
  }
  catch ( FabricCore::Exception e )
  {
    printf( "Exception: %s\n", e.getDesc_cstr() );
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

void DFGWidget::refreshExtDeps( FTL::CStrRef extDeps )
{
  m_uiHeader->refreshExtDeps( extDeps );
}

void DFGWidget::onExecChanged()
{
  if ( m_router )
  {
    m_uiController->setRouter( 0 );
    delete m_router;
    m_router = 0;
  }

  FabricCore::DFGExec &exec = m_uiController->getExec();

  if ( exec.isValid() )
  {
    m_uiGraph = new GraphView::Graph( NULL, m_dfgConfig.graphConfig );
    m_uiGraph->setController(m_uiController.get());
    m_uiGraph->setEditable(m_isEditable);
    m_uiController->setGraph(m_uiGraph);

    if(m_isEditable)
    {
      QObject::connect(
        m_uiGraph, SIGNAL(hotkeyPressed(Qt::Key, Qt::KeyboardModifier, QString)), 
        this, SLOT(onHotkeyPressed(Qt::Key, Qt::KeyboardModifier, QString))
      );  
      QObject::connect(
        m_uiGraph, SIGNAL(hotkeyReleased(Qt::Key, Qt::KeyboardModifier, QString)), 
        this, SLOT(onHotkeyReleased(Qt::Key, Qt::KeyboardModifier, QString))
      );  
      QObject::connect(
        m_uiGraph, SIGNAL(bubbleEditRequested(FabricUI::GraphView::Node*)), 
        this, SLOT(onBubbleEditRequested(FabricUI::GraphView::Node*))
      );  
      m_uiGraph->defineHotkey(Qt::Key_Space, Qt::NoModifier, "PanGraph");
    }

    m_uiGraph->initialize();

    m_router =
      static_cast<DFGNotificationRouter *>( m_uiController->createRouter() );
    m_uiController->setRouter(m_router);
  
    if(m_isEditable)
    {
      m_uiGraph->setGraphContextMenuCallback(&graphContextMenuCallback, this);
      m_uiGraph->setNodeContextMenuCallback(&nodeContextMenuCallback, this);
      m_uiGraph->setPortContextMenuCallback(&portContextMenuCallback, this);
      m_uiGraph->setSidePanelContextMenuCallback(&sidePanelContextMenuCallback, this);
    }

    if(exec.getType() == FabricCore::DFGExecType_Graph)
    {
      m_uiGraphViewWidget->show();
      m_uiGraphViewWidget->setFocus();
    }
    else if(exec.getType() == FabricCore::DFGExecType_Func)
    {
      m_uiGraphViewWidget->hide();
    }

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

  emit execChanged();
}
