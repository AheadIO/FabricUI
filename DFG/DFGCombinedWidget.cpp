
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>

#include "DFGCombinedWidget.h"

using namespace FabricUI::DFG;

DFGCombinedWidget::DFGCombinedWidget(QWidget * parent)
:QSplitter(parent)
{
  m_manager = NULL;
  m_host = NULL;
  m_treeWidget = NULL;
  m_dfgWidget = NULL;
  m_dfgValueEditor = NULL;
  m_hSplitter = NULL;
  m_dfgLogWidget = NULL;

  setOrientation(Qt::Vertical);
};

void DFGCombinedWidget::init(
  FabricCore::Client * client,
  FabricServices::ASTWrapper::KLASTManager * manager,
  FabricServices::DFGWrapper::Host * host,
  FabricServices::DFGWrapper::Binding binding,
  FabricServices::DFGWrapper::GraphExecutable graph,
  FabricServices::Commands::CommandStack * stack,
  bool overTakeBindingNotifications
) {

  if(m_dfgWidget)
    return;

  try
  {
    m_client = client;
    m_manager = manager;
    m_host = host;

    DFGConfig config;
    m_treeWidget = new DFG::PresetTreeWidget(this, m_host, config);
    m_dfgWidget = new DFG::DFGWidget(this, m_client, m_manager, m_host, binding, graph, stack, config, GraphView::GraphConfig(), overTakeBindingNotifications);
    m_dfgValueEditor = new DFG::DFGValueEditor(this, m_dfgWidget->getUIController(), config);

    m_dfgWidget->getUIController()->setLogFunc(DFGLogWidget::log);

    setContentsMargins(0, 0, 0, 0);

    m_hSplitter = new QSplitter(this);
    m_hSplitter->setOrientation(Qt::Horizontal);
    m_hSplitter->setContentsMargins(0, 0, 0, 0);

    m_hSplitter->addWidget(m_treeWidget);
    m_hSplitter->addWidget(m_dfgWidget);
    m_hSplitter->addWidget(m_dfgValueEditor);

    addWidget(m_hSplitter);

    m_dfgLogWidget = new DFGLogWidget(this, config);
    addWidget(m_dfgLogWidget);

    m_dfgWidget->getUIGraph()->defineHotkey(Qt::Key_Delete, Qt::NoModifier, "delete");
    m_dfgWidget->getUIGraph()->defineHotkey(Qt::Key_Backspace, Qt::NoModifier, "delete2");
    m_dfgWidget->getUIGraph()->defineHotkey(Qt::Key_F, Qt::NoModifier, "frameSelected");
    m_dfgWidget->getUIGraph()->defineHotkey(Qt::Key_A, Qt::NoModifier, "frameAll");
    m_dfgWidget->getUIGraph()->defineHotkey(Qt::Key_Tab, Qt::NoModifier, "tabSearch");

    QObject::connect(m_dfgValueEditor, SIGNAL(valueChanged(ValueItem*)), this, SLOT(onValueChanged()));
    QObject::connect(m_dfgWidget->getUIController(), SIGNAL(structureChanged()), this, SLOT(onStructureChanged()));
    QObject::connect(m_dfgWidget->getUIController(), SIGNAL(recompiled()), this, SLOT(onRecompilation()));
    QObject::connect(m_dfgWidget->getUIController(), SIGNAL(portRenamed(QString, QString)), this, SLOT(onPortRenamed(QString, QString)));
    QObject::connect(m_dfgWidget->getUIGraph(), SIGNAL(hotkeyPressed(Qt::Key, Qt::KeyboardModifier, QString)), 
      this, SLOT(hotkeyPressed(Qt::Key, Qt::KeyboardModifier, QString)));
    QObject::connect(m_dfgWidget->getUIGraph(), SIGNAL(nodeDoubleClicked(FabricUI::GraphView::Node*)), 
      this, SLOT(onNodeDoubleClicked(FabricUI::GraphView::Node*)));
  }
  catch(FabricCore::Exception e)
  {
    log(e.getDesc_cstr());
    return;
  }

  resize(1000, 500);
  QList<int> s = m_hSplitter->sizes();
  s[0] = 0;
  s[1] = 1000;
  s[2] = 0;
  m_hSplitter->setSizes(s);

  s = sizes();
  s[0] = 500;
  s[1] = 0;
  setSizes(s);
}

DFGCombinedWidget::~DFGCombinedWidget()
{
}

void DFGCombinedWidget::onValueChanged()
{
  onRecompilation();
}

void DFGCombinedWidget::onStructureChanged()
{
  onValueChanged();
}

void DFGCombinedWidget::hotkeyPressed(Qt::Key key, Qt::KeyboardModifier modifiers, QString hotkey)
{
  if(hotkey == "delete" || hotkey == "delete2")
  {
    std::vector<GraphView::Node *> nodes = m_dfgWidget->getUIGraph()->selectedNodes();
    m_dfgWidget->getUIController()->beginInteraction();
    for(size_t i=0;i<nodes.size();i++)
      m_dfgWidget->getUIController()->removeNode(nodes[i]);
    m_dfgWidget->getUIController()->endInteraction();
  }
  else if(hotkey == "frameSelected")
  {
    m_dfgWidget->getUIController()->frameSelectedNodes();
  }
  else if(hotkey == "frameAll")
  {
    m_dfgWidget->getUIController()->frameAllNodes();
  }
  else if(hotkey == "tabSearch")
  {
    QPoint pos = m_dfgWidget->getGraphViewWidget()->lastEventPos();
    pos = m_dfgWidget->getGraphViewWidget()->mapToGlobal(pos);
    m_dfgWidget->getTabSearchWidget()->showForSearch(pos);
  }
}

void DFGCombinedWidget::onNodeDoubleClicked(FabricUI::GraphView::Node * node)
{
  DFGWrapper::Node codeNode = m_dfgWidget->getUIController()->getNodeFromPath(node->path().toUtf8().constData());
  m_dfgValueEditor->setNode(codeNode);

  QList<int> s = m_hSplitter->sizes();
  if(s[2] == 0)
  {
    s[2] = (int)(float(s[1]) * 0.2f);
    s[1] = (int)(float(s[1]) * 0.8f);
    m_hSplitter->setSizes(s);
  }
}

void DFGCombinedWidget::setLogFunc(DFGController::LogFunc func)
{
  DFGLogWidget::setLogFunc(func);
}

void DFGCombinedWidget::log(const char * message)
{
  if(m_dfgWidget)
    m_dfgWidget->getUIController()->log(message);
  printf("%s\n", message);
}
