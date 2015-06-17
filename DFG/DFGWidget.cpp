// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtGui/QVBoxLayout>
#include <QtGui/QCursor>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>
#include <FabricCore.h>
#include "DFGWidget.h"
#include "DFGMainWindow.h"
#include "Dialogs/DFGGetStringDialog.h"
#include "Dialogs/DFGEditPortDialog.h"
#include "Dialogs/DFGNewVariableDialog.h"
#include <assert.h>

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

QSettings * DFGWidget::g_settings = NULL;

DFGWidget::DFGWidget(
  QWidget * parent, 
  FabricCore::Client const &coreClient,
  FabricCore::DFGHost const &coreDFGHost,
  FabricCore::DFGBinding const &coreDFGBinding,
  FabricCore::DFGExec const &coreDFGGraph,
  FabricServices::ASTWrapper::KLASTManager * manager,
  Commands::CommandStack * stack,
  const DFGConfig & dfgConfig,
  bool overTakeBindingNotifications
  )
  : QWidget(parent)
  , m_coreClient(coreClient)
  , m_coreDFGHost( coreDFGHost )
  , m_coreDFGBinding( coreDFGBinding )
  , m_coreDFGExec( coreDFGGraph )
  , m_manager(manager)
{
  m_dfgConfig = dfgConfig;
  m_uiGraph = NULL;
  m_uiHeader = new GraphView::GraphHeaderWidget(this, "Graph", dfgConfig.graphConfig);
  m_uiGraphViewWidget = new DFGGraphViewWidget(this, dfgConfig.graphConfig, NULL);
  m_uiController = new DFGController(NULL, m_coreClient, m_manager, m_coreDFGHost, m_coreDFGBinding, m_coreDFGExec, stack, overTakeBindingNotifications);
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

  setGraph(m_coreDFGHost, m_coreDFGBinding, m_coreDFGExec);

  QObject::connect(m_uiHeader, SIGNAL(goUpPressed()), this, SLOT(onGoUpPressed()));

  QObject::connect(
    m_uiController.get(), SIGNAL(nodeEditRequested(FabricUI::GraphView::Node *)), 
    this, SLOT(onNodeEditRequested(FabricUI::GraphView::Node *))
  );  
}

DFGWidget::~DFGWidget()
{
}

void DFGWidget::setGraph(
  FabricCore::DFGHost const &coreDFGHost,
  FabricCore::DFGBinding const &coreDFGBinding,
  FabricCore::DFGExec const &coreDFGGraph,
  bool clear
  )
{
  m_coreDFGHost = coreDFGHost;
  m_coreDFGBinding = coreDFGBinding;
  m_coreDFGExec = coreDFGGraph;

  if(clear)
    m_coreDFGExecStack.clear();

  if(m_router)
  {
    m_uiController->setRouter(NULL);
    delete(m_router);
    m_router = NULL;
  }

  if(m_coreDFGExec.isValid())
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
          m_coreDFGBinding,
          m_coreDFGExec
          )
        );
    m_uiController->setHost(m_coreDFGHost);
    m_uiController->setBinding(m_coreDFGBinding);
    m_uiController->setRouter(m_router);
    m_uiHeader->setCaption(m_coreDFGExec.getTitle());
  
    m_uiGraph->setGraphContextMenuCallback(&graphContextMenuCallback, this);
    m_uiGraph->setNodeContextMenuCallback(&nodeContextMenuCallback, this);
    m_uiGraph->setPortContextMenuCallback(&portContextMenuCallback, this);
    m_uiGraph->setSidePanelContextMenuCallback(&sidePanelContextMenuCallback, this);


    if(m_coreDFGExec.getType() == FabricCore::DFGExecType_Graph)
    {
      m_uiGraphViewWidget->show();
      m_uiGraphViewWidget->setFocus();
      m_klEditor->hide();      
    }
    else if(m_coreDFGExec.getType() == FabricCore::DFGExecType_Func)
    {
      m_uiGraphViewWidget->hide();
      m_klEditor->show();      
      m_klEditor->klEditor()->sourceCodeWidget()->setFocus();

      m_klEditor->setFunc(m_coreDFGExec, m_coreDFGExecPath.c_str());
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
  result->addAction("Cache Node");

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

  if(graphWidget->m_coreDFGExec.getNodeType(nodeName) != FabricCore::DFGNodeType_Inst)
    return NULL;
  FabricCore::DFGExec subExec = graphWidget->m_coreDFGExec.getSubExec(nodeName);

  QMenu* result = new QMenu(NULL);
  QAction* action;
  FEC_DFGCacheRule cacheRule = graphWidget->m_coreDFGExec.getInstCacheRule(nodeName);
  if(cacheRule == FEC_DFGCacheRule_Unspecified)
    cacheRule = subExec.getCacheRule();

  action = result->addAction("Edit");
  action = result->addAction("Rename");
  action = result->addAction("Delete");
  action = result->addAction("Save as Preset");
  // result->addSeparator();
  // action = result->addAction("Caching - Unspecified");
  // action->setCheckable(true);
  // if(cacheRule == FEC_DFGCacheRule_Unspecified)
  //   action->setChecked(true);
  // action = result->addAction("Caching - Never");
  // action->setCheckable(true);
  // if(cacheRule == FEC_DFGCacheRule_Never)
  //   action->setChecked(true);
  // action = result->addAction("Caching - Always");
  // action->setCheckable(true);
  // if(cacheRule == FEC_DFGCacheRule_Always)
  //   action->setChecked(true);

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
  if(m_coreDFGExecStack.size() == 0)
    return;
  FabricCore::DFGExec prevExec = m_coreDFGExecStack[m_coreDFGExecStack.size()-1];
  m_coreDFGExecStack.pop_back();
  if(!editNode(prevExec, "", false))
  {
    m_coreDFGExecStack.push_back(prevExec);
  }
  else
  {
    int pos = m_coreDFGExecPath.rfind('.');
    if(pos != std::string::npos)
      m_coreDFGExecPath = m_coreDFGExecPath.substr(0, pos);
    else
      m_coreDFGExecPath = "";
    m_uiController->setExecPath(m_coreDFGExecPath.c_str());
  }
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

    m_uiController->addEmptyGraph(text.toUtf8().constData(), QPointF(pos.x(), pos.y()));
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

    std::string nodePath = m_uiController->addEmptyFunc(text.toUtf8().constData(), QPointF(pos.x(), pos.y()));
    GraphView::Node * uiNode = m_uiGraph->node(nodePath.c_str());
    if(uiNode)
    {
      char const * nodeName = uiNode->name().c_str();
      FabricCore::DFGExec subExec = m_coreDFGExec.getSubExec(nodeName);
      editNode(subExec, nodeName, true);
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

    m_uiController->implodeNodes(text.toUtf8().constData());
  }
  else if(action->text() == "New Variable")
  {
    DFGController * controller = getUIController();
    FabricCore::Client client = controller->getClient();
    FabricCore::DFGBinding binding = controller->getCoreDFGBinding();

    DFGNewVariableDialog dialog(this, client, binding, controller->getExecPath());
    if(dialog.exec() != QDialog::Accepted)
      return;

    QString name = dialog.name();
    if(name.length() == 0)
      return;
    QString dataType = dialog.dataType();
    QString extension = dialog.extension();


    controller->addDFGVar(
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
    getUIController()->addDFGGet("get", "", QPointF(pos.x(), pos.y()));
  }
  else if(action->text() == "Write Variable (Set)")
  {
    // todo: show an auto completing text field
    getUIController()->addDFGSet("set", "", QPointF(pos.x(), pos.y()));
  }
  else if(action->text() == "Cache Node")
  {
    DFGController * controller = getUIController();
    controller->addDFGNodeFromPreset("Fabric.Core.Data.Cache", QPointF(pos.x(), pos.y()));
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
    if(m_coreDFGExec.getNodeType(nodeName) != FabricCore::DFGNodeType_Inst)
      return;
    FabricCore::DFGExec subExec = m_coreDFGExec.getSubExec(nodeName);
    editNode(subExec, nodeName, true);
  }
  else if(action->text() == "Rename")
  {
    onNodeToBeRenamed(m_contextNode);
  }
  else if(action->text() == "Delete")
  {
    m_uiController->removeNode(m_contextNode);
  }
  else if(action->text() == "Save as Preset")
  {
    if(m_coreDFGExec.getNodeType(nodeName) != FabricCore::DFGNodeType_Inst)
      return;
    FabricCore::DFGExec subExec = m_coreDFGExec.getSubExec(nodeName);

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

        FTL::StrRef rType = m_coreDFGExec.getNodePortResolvedType(pinPath.c_str());
        if(rType.size() == 0 || rType.find('$') >= 0)
          continue;
        FabricCore::RTVal val =
          m_coreDFGExec.getInstPortResolvedDefaultValue(pinPath.c_str(), rType.data());
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

    m_uiController->implodeNodes(text.toUtf8().constData());
  }
  else if(action->text() == "Explode node")
  {
    m_uiController->explodeNode(nodeName);
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
    char const * nodeName = node->name().c_str();
    if(m_coreDFGExec.getNodeType(nodeName) != FabricCore::DFGNodeType_Inst)
      return;
    FabricCore::DFGExec subExec = m_coreDFGExec.getSubExec(nodeName);
    editNode(subExec, nodeName, true);
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
    m_uiController->removePort(portName);
  }
  else if(action->text() == "Edit")
  {
    try
    {
      DFGEditPortDialog dialog(this, m_coreClient, false, m_dfgConfig);

      dialog.setTitle(portName);
      dialog.setDataType(m_coreDFGExec.getExecPortResolvedType(portName));

      FTL::StrRef uiNativeArray = m_coreDFGExec.getExecPortMetadata(portName, "uiNativeArray");
      if(uiNativeArray == "true")
        dialog.setNative(true);
      FTL::StrRef uiHidden = m_coreDFGExec.getExecPortMetadata(portName, "uiHidden");
      if(uiHidden == "true")
        dialog.setHidden();
      FTL::StrRef uiOpaque = m_coreDFGExec.getExecPortMetadata(portName, "uiOpaque");
      if(uiOpaque == "true")
        dialog.setOpaque();
      FTL::StrRef uiRange = m_coreDFGExec.getExecPortMetadata(portName, "uiRange");
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
      FTL::StrRef uiCombo = m_coreDFGExec.getExecPortMetadata(portName, "uiCombo");
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

      if(dialog.exec() != QDialog::Accepted)
        return;

      if(dialog.native())
        m_coreDFGExec.setExecPortMetadata(portName, "uiNativeArray", "true", false);
      else if(uiNativeArray.size() > 0)
        m_coreDFGExec.setExecPortMetadata(portName, "uiNativeArray", NULL, false);
      if(dialog.hidden())
        m_coreDFGExec.setExecPortMetadata(portName, "uiHidden", "true", false);
      else if(uiHidden.size() > 0)
        m_coreDFGExec.setExecPortMetadata(portName, "uiHidden", NULL, false);
      if(dialog.opaque())
        m_coreDFGExec.setExecPortMetadata(portName, "uiOpaque", "true", false);
      else if(uiOpaque.size() > 0)
        m_coreDFGExec.setExecPortMetadata(portName, "uiOpaque", NULL, false);
      if(dialog.hasRange())
      {
        QString range = "(" + QString::number(dialog.rangeMin()) + ", " + QString::number(dialog.rangeMax()) + ")";
        m_coreDFGExec.setExecPortMetadata(portName, "uiRange", range.toUtf8().constData(), false);
      }
      else if(uiRange.size() > 0)
      {
        m_coreDFGExec.setExecPortMetadata(portName, "uiRange", NULL, false);
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
        m_coreDFGExec.setExecPortMetadata(portName, "uiCombo", flat.toUtf8().constData(), false);
      }
      else if(uiCombo.size() > 0)
      {
        m_coreDFGExec.setExecPortMetadata(portName, "uiCombo", NULL, false);
      }

      m_uiController->beginInteraction();
      if(dialog.dataType().length() > 0 && dialog.dataType() != m_coreDFGExec.getExecPortResolvedType(portName))
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
    DFGEditPortDialog dialog(this, m_coreClient, true, m_dfgConfig);
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
        if(!m_uiController->addExtensionDependency(extension.toUtf8().constData(), m_coreDFGExecPath.c_str(), errorMessage))
        {
          QMessageBox msg(QMessageBox::Warning, "Fabric Warning", 
            errorMessage.c_str());
          msg.addButton("Ok", QMessageBox::AcceptRole);
          msg.exec();
          return;
        }
      }

      GraphView::PortType portType = GraphView::PortType_Input;
      if(dialog.portType() == "In")
        portType = GraphView::PortType_Output;
      else if(dialog.portType() == "IO")
        portType = GraphView::PortType_IO;

      std::string portName =
        m_uiController->addPort(
          m_uiController->getExecPath(),
          title.toUtf8().constData(),
          portType, 
          dataType.toUtf8().constData()
          );

      try
      {
        if(dialog.native())
          m_coreDFGExec.setExecPortMetadata(portName.c_str(), "uiNativeArray", "true", false);
        if(dialog.hidden())
          m_coreDFGExec.setExecPortMetadata(portName.c_str(), "uiHidden", "true", false);
        if(dialog.opaque())
          m_coreDFGExec.setExecPortMetadata(portName.c_str(), "uiOpaque", "true", false);
        if(dialog.hasRange())
        {
          QString range = "(" + QString::number(dialog.rangeMin()) + ", " + QString::number(dialog.rangeMax()) + ")";
          m_coreDFGExec.setExecPortMetadata(portName.c_str(), "uiRange", range.toUtf8().constData(), false);
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
          m_coreDFGExec.setExecPortMetadata(portName.c_str(), "uiCombo", flat.toUtf8().constData(), false);
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

bool DFGWidget::editNode(FabricCore::DFGExec exec, char const * name, bool pushExec)
{
  try
  {
    if(pushExec)
    {
      FabricCore::DFGExec prevExec = m_uiController->getCoreDFGExec();
      if(prevExec)
        m_coreDFGExecStack.push_back(prevExec);
      if(m_coreDFGExecPath.length() > 0)
        m_coreDFGExecPath += ".";
      m_coreDFGExecPath += name;
      m_uiController->setExecPath(m_coreDFGExecPath.c_str());
    }

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

    setGraph(m_coreDFGHost, m_coreDFGBinding, exec, false);
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
