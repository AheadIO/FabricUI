
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>

#include "DFGCombinedWidget.h"
#include <FabricUI/Style/FabricStyle.h>

using namespace FabricUI::DFG;

DFGCombinedWidget::DFGCombinedWidget(QWidget * parent)
:QSplitter(parent)
{
  m_manager = NULL;
  m_treeWidget = NULL;
  m_dfgWidget = NULL;
  m_dfgValueEditor = NULL;
  m_hSplitter = NULL;
  m_dfgLogWidget = NULL;
  m_setGraph = NULL;

  setStyle(new FabricUI::Style::FabricStyle());
  setOrientation(Qt::Vertical);
};

void DFGCombinedWidget::init(
  FabricCore::Client &client,
  FabricServices::ASTWrapper::KLASTManager * manager,
  FabricCore::DFGHost &host,
  FabricCore::DFGBinding &binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec &exec,
  DFGUICmdHandler *cmdHandler,
  FabricServices::Commands::CommandStack * stack,
  bool overTakeBindingNotifications,
  DFGConfig config
  )
{

  if(m_dfgWidget)
    return;

  try
  {
    m_client = client;
    m_manager = manager;

    m_dfgWidget =
      new DFG::DFGWidget(
        this,
        m_client,
        host,
        binding,
        execPath,
        exec,
        m_manager,
        cmdHandler,
        stack,
        config,
        overTakeBindingNotifications
        );

    m_treeWidget =
      new DFG::PresetTreeWidget(
        m_dfgWidget->getDFGController(),
        config,
        true,
        true,
        true
        );

    m_dfgValueEditor =
      new DFG::DFGValueEditor( m_dfgWidget->getUIController(), config );

    m_dfgWidget->getUIController()->setLogFunc(DFGLogWidget::log);

    setContentsMargins(0, 0, 0, 0);

    m_hSplitter = new QSplitter(this);
    m_hSplitter->setOrientation(Qt::Horizontal);
    m_hSplitter->setContentsMargins(0, 0, 0, 0);

    m_hSplitter->addWidget(m_treeWidget);
    m_hSplitter->addWidget(m_dfgWidget);
    m_hSplitter->addWidget(m_dfgValueEditor);

    addWidget(m_hSplitter);

    m_dfgLogWidget = new DFGLogWidget( config );
    addWidget(m_dfgLogWidget);

    if(m_dfgWidget->isEditable())
    {
      QObject::connect(m_dfgValueEditor, SIGNAL(valueChanged(ValueItem*)), this, SLOT(onValueChanged()));
      QObject::connect(m_dfgWidget, SIGNAL(portEditDialogCreated(FabricUI::DFG::DFGBaseDialog*)), this, SLOT(onPortEditDialogCreated(FabricUI::DFG::DFGBaseDialog*)));
      QObject::connect(m_dfgWidget, SIGNAL(portEditDialogInvoked(FabricUI::DFG::DFGBaseDialog*)), this, SLOT(onPortEditDialogInvoked(FabricUI::DFG::DFGBaseDialog*)));
      QObject::connect(m_dfgWidget->getUIController(), SIGNAL(structureChanged()), this, SLOT(onStructureChanged()));
      QObject::connect(m_dfgWidget->getUIController(), SIGNAL(recompiled()), this, SLOT(onRecompilation()));
      QObject::connect(m_dfgWidget->getUIController(), SIGNAL(execPortRenamed(QString, QString)), this, SLOT(onExecPortRenamed(QString, QString)));
      QObject::connect(m_dfgWidget, SIGNAL(onGraphSet(FabricUI::GraphView::Graph*)), 
        this, SLOT(onGraphSet(FabricUI::GraphView::Graph*)));
      QObject::connect(m_dfgWidget->getUIController(), SIGNAL(variablesChanged()), m_treeWidget, SLOT(refresh()));
      QObject::connect(m_dfgWidget, SIGNAL(newPresetSaved(QString)), m_treeWidget, SLOT(refresh()));
    }

    onGraphSet(m_dfgWidget->getUIGraph());
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

void DFGCombinedWidget::keyPressEvent(QKeyEvent * event)
{
  if(event->modifiers().testFlag(Qt::ControlModifier))
  {
    // filter out undo redo
    if(event->key() == Qt::Key_Z || event->key() == Qt::Key_Y)
    {
      return QSplitter::keyPressEvent(event);
    }
  }
  event->accept();
}

void DFGCombinedWidget::onValueChanged()
{
  onRecompilation();
  emit valueChanged();
}

void DFGCombinedWidget::onStructureChanged()
{
  onValueChanged();
}

void DFGCombinedWidget::hotkeyPressed(Qt::Key key, Qt::KeyboardModifier modifiers, QString hotkey)
{
  if(hotkey == "delete" || hotkey == "delete2")
  {
    m_dfgWidget->getUIController()->gvcDoRemoveNodes(
      m_dfgWidget->getUIGraph()->selectedNodes()
      );
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
  else if(hotkey == "copy")
  {
    m_dfgWidget->getUIController()->copy();
  }
  else if(hotkey == "paste")
  {
    m_dfgWidget->getUIController()->cmdPaste();
  }
  else if(hotkey == "toggleSidePanels")
  {
    QList<int> s = m_hSplitter->sizes();
    if(s[0] != 0 || s[2] != 0)
    {
      s[1] += s[0] + s[2];
      s[0] = 0;
      s[2] = 0;
    }
    else
    {
      s[0] = (int)(float(s[1]) * 0.2f);
      s[2] = s[0];
      s[1] -= s[0] + s[2];
    }
    m_hSplitter->setSizes(s);
  }
  else if(hotkey == "rename node")
  {
    std::vector<GraphView::Node *> nodes = m_dfgWidget->getUIGraph()->selectedNodes();
    if(nodes.size() > 0)
      m_dfgWidget->onEditNodeTitle(nodes[0]);
  }
  else if(hotkey == "relax nodes")
  {
    m_dfgWidget->getUIController()->relaxNodes();
  }
}

void DFGCombinedWidget::onGraphSet(FabricUI::GraphView::Graph * graph)
{
  if(graph != m_setGraph)
  {
    if(m_dfgWidget->isEditable())
    {
      graph->defineHotkey(Qt::Key_Delete, Qt::NoModifier, "delete");
      graph->defineHotkey(Qt::Key_Backspace, Qt::NoModifier, "delete2");
      graph->defineHotkey(Qt::Key_F, Qt::NoModifier, "frameSelected");
      graph->defineHotkey(Qt::Key_A, Qt::NoModifier, "frameAll");
      graph->defineHotkey(Qt::Key_Tab, Qt::NoModifier, "tabSearch");
      graph->defineHotkey(Qt::Key_C, Qt::ControlModifier, "copy");
      graph->defineHotkey(Qt::Key_V, Qt::ControlModifier, "paste");
      graph->defineHotkey(Qt::Key_Tab, Qt::ControlModifier, "toggleSidePanels");
      graph->defineHotkey(Qt::Key_F2, Qt::NoModifier, "rename node");
      graph->defineHotkey(Qt::Key_R, Qt::ControlModifier, "relax nodes");

      QObject::connect(graph, SIGNAL(hotkeyPressed(Qt::Key, Qt::KeyboardModifier, QString)), 
        this, SLOT(hotkeyPressed(Qt::Key, Qt::KeyboardModifier, QString)));
      QObject::connect(graph, SIGNAL(nodeDoubleClicked(FabricUI::GraphView::Node*)), 
        this, SLOT(onNodeDoubleClicked(FabricUI::GraphView::Node*)));
    }
    m_setGraph = graph;
  }
}

void DFGCombinedWidget::onNodeDoubleClicked(FabricUI::GraphView::Node * node)
{
  if ( node->isBackDropNode() )
    return;
  
  m_dfgValueEditor->setNodeName(node->name());

  QList<int> s = m_hSplitter->sizes();
  if(s[2] == 0)
  {
    s[2] = (int)(float(s[1]) * 0.2f);
    s[1] -= s[2];
    m_hSplitter->setSizes(s);
  }
}

void DFGCombinedWidget::setLogFunc(DFGController::LogFunc func)
{
  DFGLogWidget::setLogFunc(func);
}

void DFGCombinedWidget::onPortEditDialogCreated(FabricUI::DFG::DFGBaseDialog * dialog)
{
  emit portEditDialogCreated(dialog);
}

void DFGCombinedWidget::onPortEditDialogInvoked(FabricUI::DFG::DFGBaseDialog * dialog)
{
  emit portEditDialogInvoked(dialog);
}

void DFGCombinedWidget::log(const char * message)
{
  if(m_dfgWidget)
    m_dfgWidget->getUIController()->log(message);
  printf("%s\n", message);
}
