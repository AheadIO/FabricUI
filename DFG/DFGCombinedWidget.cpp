
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>

#include "DFGCombinedWidget.h"
#include <FabricUI/Style/FabricStyle.h>
#include <FabricUI/DFG/Dialogs/DFGNodePropertiesDialog.h>

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
  bool overTakeBindingNotifications,
  DFGConfig config
  )
{
  if(m_dfgWidget)
    return;

  QMenuBar * menuBar = new QMenuBar(this);
  menuBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  addWidget(menuBar);

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
        config,
        overTakeBindingNotifications
        );

    // preset library
    // Because of a lack of performances, we don't expose the search tool of the PresetTreeWidget
    m_treeWidget =
      new DFG::PresetTreeWidget(
        m_dfgWidget->getDFGController(),
        config,
        true,
        false,
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
      QObject::connect(
        m_dfgValueEditor, SIGNAL(valueItemDelta(ValueItem*)),
        this, SLOT(onValueChanged())
        );
      QObject::connect(
        m_dfgValueEditor, SIGNAL(valueItemInteractionDelta(ValueItem*)),
        this, SLOT(onValueChanged())
        );
      QObject::connect(m_dfgWidget, SIGNAL(portEditDialogCreated(FabricUI::DFG::DFGBaseDialog*)), this, SLOT(onPortEditDialogCreated(FabricUI::DFG::DFGBaseDialog*)));
      QObject::connect(m_dfgWidget, SIGNAL(portEditDialogInvoked(FabricUI::DFG::DFGBaseDialog*, FTL::JSONObjectEnc<>*)), this, SLOT(onPortEditDialogInvoked(FabricUI::DFG::DFGBaseDialog*, FTL::JSONObjectEnc<>*)));

      QObject::connect(
        m_dfgWidget->getUIController(), SIGNAL(argsChanged()),
        this, SLOT(onStructureChanged())
        );
      QObject::connect(
        m_dfgWidget->getUIController(), SIGNAL(argValuesChanged()),
        this, SLOT(onValueChanged())
        );
      QObject::connect(
        m_dfgWidget->getUIController(), SIGNAL(varsChanged()),
        m_treeWidget, SLOT(refresh())
        );
      QObject::connect(
        m_dfgWidget->getUIController(), SIGNAL(defaultValuesChanged()),
        this, SLOT(onValueChanged())
        );
      QObject::connect(
        m_dfgWidget, SIGNAL(nodeInspectRequested(FabricUI::GraphView::Node*)),
        this, SLOT(onNodeInspectRequested(FabricUI::GraphView::Node*))
        );

      QObject::connect(m_dfgWidget, SIGNAL(onGraphSet(FabricUI::GraphView::Graph*)), 
        this, SLOT(onGraphSet(FabricUI::GraphView::Graph*)));
      QObject::connect(m_dfgWidget->getUIController(), SIGNAL(varsChanged()), m_treeWidget, SLOT(refresh()));
      QObject::connect(m_dfgWidget, SIGNAL(newPresetSaved(QString)), m_treeWidget, SLOT(refresh()));
    }

    // populate the menu bar
    QObject::connect(
      m_dfgWidget, 
      SIGNAL(additionalMenuActionsRequested(QString, QMenu*, bool)), 
      this, SLOT(onAdditionalMenuActionsRequested(QString, QMenu *, bool))
      );
    m_dfgWidget->populateMenuBar(menuBar);

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
  s[0] = 10;
  s[1] = 500;
  s[2] = 0;
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
  else if(hotkey == "frame selected")
  {
    m_dfgWidget->getUIController()->frameSelectedNodes();
  }
  else if(hotkey == "frame all")
  {
    m_dfgWidget->getUIController()->frameAllNodes();
  }
  else if(hotkey == "tab search")
  {
    QPoint pos = m_dfgWidget->getGraphViewWidget()->lastEventPos();
    pos = m_dfgWidget->getGraphViewWidget()->mapToGlobal(pos);
    m_dfgWidget->getTabSearchWidget()->showForSearch(pos);
  }
  else if(hotkey == "copy")
  {
    m_dfgWidget->getUIController()->copy();
  }
  else if(hotkey == "cut")
  {
    m_dfgWidget->getUIController()->cmdCut();
  }
  else if(hotkey == "paste")
  {
    m_dfgWidget->getUIController()->cmdPaste();
  }
  else if(hotkey == "toggle side panels")
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
  else if(hotkey == "edit properties")
  {
    m_dfgWidget->editPropertiesForCurrentSelection();
  }
  else if(hotkey == "relax nodes")
  {
    m_dfgWidget->getUIController()->relaxNodes();
  }
  else if(hotkey == "reset zoom")
  {
    m_dfgWidget->onResetZoom();
  }
  else if(hotkey == "collapse nodes level 1")
  {
    m_dfgWidget->getUIController()->collapseSelectedNodes(2);
  }
  else if(hotkey == "collapse nodes level 2")
  {
    m_dfgWidget->getUIController()->collapseSelectedNodes(1);
  }
  else if(hotkey == "collapse nodes level 3")
  {
    m_dfgWidget->getUIController()->collapseSelectedNodes(0);
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
      graph->defineHotkey(Qt::Key_F, Qt::NoModifier, "frame selected");
      graph->defineHotkey(Qt::Key_A, Qt::NoModifier, "frame all");
      graph->defineHotkey(Qt::Key_Tab, Qt::NoModifier, "tab search");
      graph->defineHotkey(Qt::Key_C, Qt::ControlModifier, "copy");
      graph->defineHotkey(Qt::Key_X, Qt::ControlModifier, "cut");
      graph->defineHotkey(Qt::Key_V, Qt::ControlModifier, "paste");
      graph->defineHotkey(Qt::Key_Tab, Qt::ControlModifier, "toggle side panels");
      graph->defineHotkey(Qt::Key_F2, Qt::NoModifier, "edit properties");
      graph->defineHotkey(Qt::Key_R, Qt::ControlModifier, "relax nodes");
      graph->defineHotkey(Qt::Key_0, Qt::ControlModifier, "reset zoom");
      graph->defineHotkey(Qt::Key_1, Qt::NoModifier, "collapse nodes level 1");
      graph->defineHotkey(Qt::Key_2, Qt::NoModifier, "collapse nodes level 2");
      graph->defineHotkey(Qt::Key_3, Qt::NoModifier, "collapse nodes level 3");

      QObject::connect(graph, SIGNAL(hotkeyPressed(Qt::Key, Qt::KeyboardModifier, QString)), 
        this, SLOT(hotkeyPressed(Qt::Key, Qt::KeyboardModifier, QString)));
      QObject::connect(graph, SIGNAL(nodeInspectRequested(FabricUI::GraphView::Node*)), 
        this, SLOT(onNodeInspectRequested(FabricUI::GraphView::Node*)));
      QObject::connect(graph, SIGNAL(nodeEditRequested(FabricUI::GraphView::Node*)), 
        this, SLOT(onNodeEditRequested(FabricUI::GraphView::Node*)));
    }
    m_setGraph = graph;
  }
}

void DFGCombinedWidget::onNodeInspectRequested(FabricUI::GraphView::Node * node)
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

  QList<int> s = m_hSplitter->sizes();
  if(s[2] == 0)
  {
    s[2] = (int)(float(s[1]) * 0.2f);
    s[1] -= s[2];
    m_hSplitter->setSizes(s);
  }
}

void DFGCombinedWidget::onNodeEditRequested(FabricUI::GraphView::Node * node)
{
  m_dfgWidget->maybeEditNode(node);
}

void DFGCombinedWidget::setLogFunc(DFGController::LogFunc func)
{
  DFGLogWidget::setLogFunc(func);
}

void DFGCombinedWidget::onPortEditDialogCreated(FabricUI::DFG::DFGBaseDialog * dialog)
{
  emit portEditDialogCreated(dialog);
}

void DFGCombinedWidget::onPortEditDialogInvoked(FabricUI::DFG::DFGBaseDialog * dialog, FTL::JSONObjectEnc<> * additionalMetaData)
{
  emit portEditDialogInvoked(dialog, additionalMetaData);
}

void DFGCombinedWidget::log(const char * message)
{
  if(m_dfgWidget)
    m_dfgWidget->getUIController()->log(message);
  printf("%s\n", message);
}

void DFGCombinedWidget::onAdditionalMenuActionsRequested(QString name, QMenu * menu, bool prefix)
{
  if(name == "File")
  {
    if(prefix)
    {
      // QAction * loadGraphAction = menu->addAction("Load Graph...");
      // loadGraphAction->setShortcut(QKeySequence::Open);

      // QAction * saveGraphAsAction = menu->addAction("Save Graph As...");
      // saveGraphAsAction->setShortcut(QKeySequence::SaveAs);
    
      // QObject::connect(loadGraphAction, SIGNAL(triggered()), this, SLOT(onLoadGraph()));
      // QObject::connect(saveGraphAsAction, SIGNAL(triggered()), this, SLOT(onSaveGraphAs()));
    }
  }
  else if(name == "Edit")
  {
    if(prefix)
    {
      QAction *undoAction = menu->addAction("Undo");
      undoAction->setShortcut( QKeySequence::Undo );
      // [Julien] When using shortcut in Qt, set the flag WidgetWithChildrenShortcut so the shortcut is specific to the widget
      undoAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
      menu->addAction( undoAction );
      QAction *redoAction = menu->addAction("Redo");
      redoAction->setShortcut( QKeySequence::Redo );
      // [Julien] When using shortcut in Qt, set the flag WidgetWithChildrenShortcut so the shortcut is specific to the widget
      redoAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
      menu->addAction( redoAction );

      QObject::connect(undoAction, SIGNAL(triggered()), this, SLOT(onUndo()));
      QObject::connect(redoAction, SIGNAL(triggered()), this, SLOT(onRedo()));
    }
  }
  else if(name == "View")
  {
    // todo: here we might add view options for the canvas graph
  }
}
