// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtGui/QVBoxLayout>
#include <QtGui/QCursor>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtCore/QDebug>
#include <FabricCore.h>
#include "DFGWidget.h"
#include "DFGMainWindow.h"
#include "Dialogs/DFGGetStringDialog.h"
#include "Dialogs/DFGEditPortDialog.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGWidget::DFGWidget(
  QWidget * parent, 
  FabricCore::Client * client,
  ASTWrapper::KLASTManager * manager,
  DFGWrapper::Host * host,
  DFGWrapper::Binding binding,
  DFGWrapper::GraphExecutablePtr graph,
  Commands::CommandStack * stack,
  const DFGConfig & dfgConfig,
  bool overTakeBindingNotifications
)
: QWidget(parent)
, m_client(client)
, m_manager(manager)
, m_dfgGraph(graph)
, m_dfgBinding(binding)
{
  m_dfgConfig = dfgConfig;
  m_uiFactory = new DFGFactory(dfgConfig);
  m_uiGraph = new DFGGraph(NULL, dfgConfig.graphConfig, m_uiFactory);
  m_uiHeader = new GraphView::GraphHeaderWidget(this, "Graph", dfgConfig.graphConfig);
  m_uiGraphViewWidget = new DFGGraphViewWidget(this, dfgConfig.graphConfig, m_uiFactory, m_uiGraph);
  m_uiController = new DFGController(m_uiGraph, stack, client, NULL, m_manager, overTakeBindingNotifications);
  m_klEditor = new DFGKLEditorWidget(this, m_uiController, m_manager, m_dfgConfig);
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

  m_dfgHost = NULL;
  m_dfgView = NULL;

  setGraph(host, binding, graph);

  QObject::connect(m_uiHeader, SIGNAL(goUpPressed()), this, SLOT(onGoUpPressed()));

  QObject::connect(
    m_uiGraph, SIGNAL(hotkeyPressed(Qt::Key, Qt::KeyboardModifier, QString)), 
    this, SLOT(onHotkeyPressed(Qt::Key, Qt::KeyboardModifier, QString))
  );  
  QObject::connect(
    m_uiGraph, SIGNAL(hotkeyReleased(Qt::Key, Qt::KeyboardModifier, QString)), 
    this, SLOT(onHotkeyReleased(Qt::Key, Qt::KeyboardModifier, QString))
  );  
  QObject::connect(
    m_uiController, SIGNAL(nodeEditRequested(FabricUI::GraphView::Node *)), 
    this, SLOT(onNodeEditRequested(FabricUI::GraphView::Node *))
  );  

  m_uiGraph->defineHotkey(Qt::Key_Space, Qt::NoModifier, "PanGraph");
}

DFGWidget::~DFGWidget()
{

}

void DFGWidget::setGraph(DFGWrapper::Host * host, DFGWrapper::Binding binding, DFGWrapper::GraphExecutablePtr graph, bool clear)
{
  m_dfgHost = host;
  m_dfgBinding = binding;
  m_dfgGraph = graph;
  if(clear)
    m_execStack.clear();

  if(m_dfgView)
  {
    m_uiController->setView(NULL);
    delete(m_dfgView);
  }

  m_uiGraph->reset(m_dfgGraph->getGraphPath(), true);
  m_dfgView = new DFGView(m_dfgGraph, m_dfgConfig);
  m_uiController->setHost(m_dfgHost);
  m_uiController->setView(m_dfgView);
  m_uiHeader->setCaption(m_dfgGraph->getGraphPath());

  m_uiGraph->setGraphContextMenuCallback(&graphContextMenuCallback, this);
  m_uiGraph->setNodeContextMenuCallback(&nodeContextMenuCallback, this);
  m_uiGraph->setPortContextMenuCallback(&portContextMenuCallback, this);
  m_uiGraph->setSidePanelContextMenuCallback(&sidePanelContextMenuCallback, this);
}

DFGGraph * DFGWidget::getUIGraph()
{
  return m_uiGraph;
}

DFGController * DFGWidget::getUIController()
{
  return m_uiController;
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

  graphWidget->connect(result, SIGNAL(triggered(QAction*)), graphWidget, SLOT(onGraphAction(QAction*)));
  return result;
}

QMenu* DFGWidget::nodeContextMenuCallback(FabricUI::GraphView::Node* node, void* userData)
{
  DFGWidget * graphWidget = (DFGWidget*)userData;
  DFGGraph * graph = graphWidget->m_uiGraph;
  if(graph->controller() == NULL)
    return NULL;
  graphWidget->m_contextNode = node;

  FabricServices::DFGWrapper::NodePtr dfgNode = graphWidget->m_uiController->getNodeFromPath(node->path().toUtf8().constData());
  FEC_DFGCacheRule cacheRule = dfgNode->getCacheRule();
  if(cacheRule == FEC_DFGCacheRule_Unspecified)
    cacheRule = dfgNode->getExecutable()->getCacheRule();

  QMenu* result = new QMenu(NULL);
  QAction* action;
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

  const std::vector<GraphView::Node*> & nodes = graphWidget->getUIController()->graph()->selectedNodes();
  if(nodes.size() > 0)
  {
    result->addSeparator();
    result->addAction("Implode nodes");
  }

  graphWidget->connect(result, SIGNAL(triggered(QAction*)), graphWidget, SLOT(onNodeAction(QAction*)));
  return result;
}

QMenu* DFGWidget::portContextMenuCallback(FabricUI::GraphView::Port* port, void* userData)
{
  DFGWidget * graphWidget = (DFGWidget*)userData;
  DFGGraph * graph = graphWidget->m_uiGraph;
  if(graph->controller() == NULL)
    return NULL;
  graphWidget->m_contextPort = port;
  QMenu* result = new QMenu(NULL);
  result->addAction("Edit");
  result->addAction("Delete");

  graphWidget->connect(result, SIGNAL(triggered(QAction*)), graphWidget, SLOT(onPortAction(QAction*)));
  return result;
}

QMenu* DFGWidget::sidePanelContextMenuCallback(FabricUI::GraphView::SidePanel* panel, void* userData)
{
  DFGWidget * graphWidget = (DFGWidget*)userData;
  DFGGraph * graph = graphWidget->m_uiGraph;
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
  if(m_execStack.size() == 0)
    return;
  DFGWrapper::ExecutablePtr prevExec = m_execStack[m_execStack.size()-1];
  m_execStack.pop_back();
  if(!editNode(prevExec, false))
  {
    m_execStack.push_back(prevExec);
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

    m_uiController->addEmptyGraph(m_uiGraph->path(), text, QPointF(pos.x(), pos.y()));
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

    QString nodePath = m_uiController->addEmptyFunc(m_uiGraph->path(), text, QPointF(pos.x(), pos.y()));
    GraphView::Node * uiNode = m_uiGraph->nodeFromPath(nodePath);
    if(uiNode)
    {
      DFGWrapper::NodePtr dfgNode = m_uiController->getNodeFromPath(uiNode->path().toUtf8().constData());
      editNode(dfgNode->getExecutable(), true);
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

    m_uiController->implodeNodes(text);
  }
}

void DFGWidget::onNodeAction(QAction * action)
{
  if(m_contextNode == NULL)
    return;

  std::string nodePath = GraphView::relativePath(m_uiGraph->path(), m_contextNode->path()).toUtf8().constData();
  if(action->text() == "Edit")
  {
    DFGWrapper::NodePtr dfgNode = m_uiController->getNodeFromPath(m_contextNode->path().toUtf8().constData());
    editNode(dfgNode->getExecutable(), true);
  }
  else if(action->text() == "Rename")
  {
    DFGGetStringDialog dialog(this, m_contextNode->title(), m_dfgConfig);
    if(dialog.exec() != QDialog::Accepted)
      return;

    QString text = dialog.text();
    m_uiController->renameNode(m_contextNode, text);
  }
  else if(action->text() == "Delete")
  {
    m_uiController->removeNode(m_contextNode);
  }
  else if(action->text() == "Save as Preset")
  {
    DFGWrapper::Binding binding = m_uiController->getBinding();
    DFGWrapper::GraphExecutablePtr graph = DFGWrapper::GraphExecutablePtr::StaticCast(binding.getExecutable());
    DFGWrapper::NodePtr node = graph->getNode(m_contextNode->name().toUtf8().constData());
    DFGWrapper::ExecutablePtr exec = node->getExecutable();

    QString title = exec->getTitle();
    if(title.toLower().endsWith(".dfg.json"))
      title = title.left(title.length() - 9);

    QString filter = "DFG Preset (*.dfg.json)";
    QString filePath = QFileDialog::getSaveFileName(this, "Save preset", title, filter, &filter);
    if(filePath.length() == 0)
      return;
    if(filePath.toLower().endsWith(".dfg.json.dfg.json"))
      filePath = filePath.left(filePath.length() - 9);
    std::string filePathStr = filePath.toUtf8().constData();

    try
    {
      std::string json = exec->exportJSON();
      FILE * file = fopen(filePathStr.c_str(), "wb");
      if(file)
      {
        fwrite(json.c_str(), json.length(), 1, file);
        fclose(file);
      }

      exec->setImportPathname(filePathStr.c_str());
      exec->attachPreset("", exec->getTitle());

      emit newPresetSaved(filePathStr.c_str());
    }
    catch(FabricCore::Exception e)
    {
      printf("Exception: %s\n", e.getDesc_cstr());
    }
  }
  else if(action->text() == "Caching - Unspecified")
  {
    m_uiController->setNodeCacheRule(m_contextNode->path(), FEC_DFGCacheRule_Unspecified);
  }
  else if(action->text() == "Caching - Never")
  {
    m_uiController->setNodeCacheRule(m_contextNode->path(), FEC_DFGCacheRule_Never);
  }
  else if(action->text() == "Caching - Always")
  {
    m_uiController->setNodeCacheRule(m_contextNode->path(), FEC_DFGCacheRule_Always);
  }
  else if(action->text() == "Implode nodes")
  {
    DFGGetStringDialog dialog(this, "graph", m_dfgConfig);
    if(dialog.exec() != QDialog::Accepted)
      return;

    QString text = dialog.text();
    if(text.length() == 0)
      return;

    m_uiController->implodeNodes(text);
  }

  m_contextNode = NULL;
}

void DFGWidget::onNodeEditRequested(FabricUI::GraphView::Node * node)
{
  DFGWrapper::NodePtr dfgNode = m_uiController->getNodeFromPath(node->path().toUtf8().constData());
  editNode(dfgNode->getExecutable(), true);
}

void DFGWidget::onPortAction(QAction * action)
{
  if(m_contextPort == NULL)
    return;

  if(action->text() == "Delete")
  {
    m_uiController->removePort(GraphView::parentPath(m_contextPort->path()), m_contextPort->name());
  }
  else if(action->text() == "Edit")
  {
    try
    {
      DFGWrapper::EndPointPtr endPoint = m_uiController->getEndPointFromPath(m_contextPort->path().toUtf8().constData());
      if(!endPoint)
        return;
      DFGWrapper::PortPtr port = DFGWrapper::PortPtr::StaticCast(endPoint);
      
      DFGEditPortDialog dialog(this, false, m_dfgConfig);

      dialog.setTitle(port->getName());
      dialog.setDataType(port->getResolvedType());

      QString uiNativeArray = port->getMetadata("uiNativeArray");
      if(uiNativeArray == "true")
        dialog.setNative(true);
      QString uiHidden = port->getMetadata("uiHidden");
      if(uiHidden == "true")
        dialog.setHidden();
      QString uiOpaque = port->getMetadata("uiOpaque");
      if(uiOpaque == "true")
        dialog.setOpaque();
      QString uiRange = port->getMetadata("uiRange");
      if(uiRange.length() > 0)
      {
        QString filteredUiRange;
        for(unsigned int i=0;i<uiRange.length();i++)
        {
          char c = uiRange.toUtf8().constData()[i];
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
      QString uiCombo = port->getMetadata("uiCombo");
      if(uiCombo.length() > 0)
      {
        if(uiCombo[0] == '(');
          uiCombo = uiCombo.mid(1);
        if(uiCombo[uiCombo.length()-1] == ')');
          uiCombo = uiCombo.left(uiCombo.length()-1);

        QStringList parts = uiCombo.split(',');
        dialog.setHasCombo(true);
        dialog.setComboValues(parts);
      }

      if(dialog.exec() != QDialog::Accepted)
        return;

      if(dialog.native())
        port->setMetadata("uiNativeArray", "true", false);
      else if(uiNativeArray.length() > 0)
        port->setMetadata("uiNativeArray", NULL, false);
      if(dialog.hidden())
        port->setMetadata("uiHidden", "true", false);
      else if(uiHidden.length() > 0)
        port->setMetadata("uiHidden", NULL, false);
      if(dialog.opaque())
        port->setMetadata("uiOpaque", "true", false);
      else if(uiOpaque.length() > 0)
        port->setMetadata("uiOpaque", NULL, false);
      if(dialog.hasRange())
      {
        QString range = "(" + QString::number(dialog.rangeMin()) + ", " + QString::number(dialog.rangeMax()) + ")";
        port->setMetadata("uiRange", range.toUtf8().constData(), false);
      }
      else if(uiRange.length() > 0)
      {
        port->setMetadata("uiRange", NULL, false);
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
        port->setMetadata("uiCombo", flat.toUtf8().constData(), false);
      }
      else if(uiCombo.length() > 0)
      {
        port->setMetadata("uiCombo", NULL, false);
      }

      m_uiController->beginInteraction();
      if(dialog.dataType().length() > 0 && dialog.dataType() != port->getResolvedType())
      {
        if(m_uiController->isViewingRootGraph())
          m_uiController->setArg(m_contextPort->name(), dialog.dataType());
      }

      if(dialog.title() != port->getName())
      {
        m_uiController->renamePort(m_contextPort->path(), dialog.title());
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
    DFGEditPortDialog dialog(this, false, m_dfgConfig);
    if(dialog.exec() != QDialog::Accepted)
      return;

    QString title = dialog.title();
    QString dataType = dialog.dataType();
    QString extension = dialog.extension();

    if(title.length() > 0)
    {
      if(extension.length() > 0)
        m_uiController->addExtensionDependency(extension, m_uiGraph->path());
      QString portPath = m_uiController->addPort("", title, m_contextPortType, dataType);

      try
      {
        DFGWrapper::EndPointPtr endPoint = m_uiController->getEndPointFromPath(portPath.toUtf8().constData());
        if(endPoint)
        {
          DFGWrapper::PortPtr port = DFGWrapper::PortPtr::StaticCast(endPoint);

          if(dialog.native())
            port->setMetadata("uiNativeArray", "true", false);
          if(dialog.hidden())
            port->setMetadata("uiHidden", "true", false);
          if(dialog.opaque())
            port->setMetadata("uiOpaque", "true", false);
          if(dialog.hasRange())
          {
            QString range = "(" + QString::number(dialog.rangeMin()) + ", " + QString::number(dialog.rangeMax()) + ")";
            port->setMetadata("uiRange", range.toUtf8().constData(), false);
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
            port->setMetadata("uiCombo", flat.toUtf8().constData(), false);
          }

          emit m_uiController->structureChanged();
        }
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

bool DFGWidget::editNode(DFGWrapper::ExecutablePtr exec, bool pushExec)
{
  try
  {
    if(pushExec)
    {
      DFGWrapper::GraphExecutablePtr prevExec = m_uiController->getGraphExec();
      if(prevExec)
        m_execStack.push_back(DFGWrapper::ExecutablePtr::StaticCast(prevExec));
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


    if(exec->isGraph())
    {
      setGraph(m_dfgHost, m_dfgBinding, DFGWrapper::GraphExecutablePtr::StaticCast(exec), false);
      m_uiGraphViewWidget->show();
      m_uiGraphViewWidget->setFocus();
      m_klEditor->hide();      
    }
    else if(exec->isFunc())
    {
      m_uiHeader->setCaption(exec->getExecPath());
      m_uiGraphViewWidget->hide();
      m_klEditor->show();      
      m_klEditor->klEditor()->sourceCodeWidget()->setFocus();
      m_klEditor->setFunc(DFGWrapper::FuncExecutablePtr::StaticCast(exec));
    }
  }
  catch(FabricCore::Exception e)
  {
    printf("Exception: %s\n", e.getDesc_cstr());
    return false;
  }
  return true;  
}
