
#include "VEEditorOwner.h"

#include <FabricUI/DFG/DFGController.h>
#include <FabricUI/DFG/DFGExecNotifier.h>
#include <FabricUI/DFG/DFGWidget.h>
#include <FabricUI/GraphView/Node.h>
#include <FabricUI/ModelItems/BindingModelItem.h>
#include <FabricUI/ModelItems/InstModelItem.h>
#include <FabricUI/ModelItems/VarModelItem.h>
#include <FabricUI/ValueEditor/VETreeWidget.h>
#include <FabricUI/ValueEditor/ItemMetadata.h>

using namespace FabricUI;
using namespace ValueEditor;
using namespace ModelItems;

VEEditorOwner::VEEditorOwner( ValueEditorBridgeOwner *owner )
  : m_timelinePortIndex( -1 )
  , m_owner( owner )
  , m_dfgValueEditor( NULL )
  , m_setGraph( NULL )
  , m_modelRoot( NULL )
{
  m_dfgValueEditor = new VETreeWidget();
}

VEEditorOwner::~VEEditorOwner()
{

}

QWidget* VEEditorOwner::getWidget() const
{
  return m_dfgValueEditor;
}

void VEEditorOwner::initConnections()
{
  connect(
    getUIController(),
    SIGNAL(bindingChanged(FabricCore::DFGBinding const &)),
    this,
    SLOT(onControllerBindingChanged(FabricCore::DFGBinding const &))
    );

  connect(
    getUIController(), SIGNAL( argsChanged() ),
    this, SLOT( onStructureChanged() )
    );

  connect(
    getDfgWidget(), SIGNAL( nodeInspectRequested( FabricUI::GraphView::Node* ) ),
    this, SLOT( onNodeInspectRequested( FabricUI::GraphView::Node* ) )
    );

  connect(
    this, SIGNAL( modelItemInserted( BaseModelItem*, int, const char* ) ),
    m_dfgValueEditor, SLOT( onModelItemChildInserted( BaseModelItem*, int, const char* ) )
    );
  connect(
    this, SIGNAL( modelItemTypeChange( BaseModelItem*, const char* ) ),
    m_dfgValueEditor, SLOT( onModelItemTypeChanged( BaseModelItem*, const char* ) )
    );
  connect(
    this, SIGNAL( modelItemRemoved( BaseModelItem* ) ),
    m_dfgValueEditor, SLOT( onModelItemRemoved( BaseModelItem* ) )
    );
  connect(
    this, SIGNAL( modelItemRenamed( BaseModelItem* ) ),
    m_dfgValueEditor, SLOT( onModelItemRenamed( BaseModelItem* ) )
    );
  connect(
    this, SIGNAL( modelItemChildrenReordered( BaseModelItem*, const QList<int>& ) ),
    m_dfgValueEditor, SLOT( onModelItemChildrenReordered( BaseModelItem*, const QList<int>& ) )
    );

  // connect(
//   getDFGController(), SIGNAL(bindingChanged(FabricCore::DFGBinding const &)),
//   m_dfgValueEditor, SLOT(setBinding(FabricCore::DFGBinding const &))
//   );
  connect(
    this, SIGNAL( replaceModelRoot( BaseModelItem* ) ),
    m_dfgValueEditor, SLOT( onSetModelItem( BaseModelItem* ) )
    );

  connect( getDfgWidget(), SIGNAL( onGraphSet( FabricUI::GraphView::Graph* ) ),
                    this, SLOT( onGraphSet( FabricUI::GraphView::Graph* ) ) );

  onGraphSet( getDfgWidget()->getUIGraph() );
}

FabricUI::DFG::DFGWidget * VEEditorOwner::getDfgWidget()
{
  return m_owner->getDfgWidget();
}
FabricUI::DFG::DFGController * VEEditorOwner::getUIController()
{
  return getDfgWidget()->getUIController();
}
FabricUI::DFG::DFGController * VEEditorOwner::getDFGController()
{
  return getDfgWidget()->getDFGController();
}

void VEEditorOwner::setModelRoot(
  FabricUI::DFG::DFGController *dfgController,
  FabricUI::ModelItems::BindingModelItem *bindingModelItem
  )
{
  m_subNotifier.clear();
  m_notifier.clear();

  delete m_modelRoot;
  m_modelRoot = bindingModelItem;

  if ( bindingModelItem )
  {
    m_notifier = dfgController->getBindingNotifier();

    connect(
      m_notifier.data(),
      SIGNAL( argInserted( unsigned, FTL::CStrRef, FTL::CStrRef ) ),
      this,
      SLOT( onBindingArgInserted( unsigned, FTL::CStrRef, FTL::CStrRef ) )
      );
    connect(
      m_notifier.data(),
      SIGNAL( argRenamed( unsigned, FTL::CStrRef, FTL::CStrRef ) ),
      this,
      SLOT( onBindingArgRenamed( unsigned, FTL::CStrRef, FTL::CStrRef ) )
      );
    connect(
      m_notifier.data(),
      SIGNAL( argRemoved( unsigned, FTL::CStrRef ) ),
      this,
      SLOT( onBindingArgRemoved( unsigned, FTL::CStrRef ) )
      );
    connect(
      m_notifier.data(),
      SIGNAL( argTypeChanged( unsigned, FTL::CStrRef, FTL::CStrRef ) ),
      this,
      SLOT( onBindingArgTypeChanged( unsigned, FTL::CStrRef, FTL::CStrRef ) )
      );
    connect(
      m_notifier.data(),
      SIGNAL( argValueChanged( unsigned, FTL::CStrRef ) ),
      this,
      SLOT( onBindingArgValueChanged( unsigned, FTL::CStrRef ) )
      );
    connect(
      m_notifier.data(),
      SIGNAL( argsReordered( FTL::ArrayRef<unsigned> ) ),
      this,
      SLOT( onBindingArgsReordered( FTL::ArrayRef<unsigned> ) )
      );

    FabricCore::DFGBinding binding = dfgController->getBinding();
    FabricCore::DFGExec rootExec = binding.getExec();

    m_subNotifier = DFG::DFGExecNotifier::Create( rootExec );

    connect(
      m_subNotifier.data(),
      SIGNAL(portMetadataChanged(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef)),
      this,
      SLOT(onExecPortMetadataChanged(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef))
      );
  }

  emit replaceModelRoot( m_modelRoot );
}

void VEEditorOwner::setModelRoot(
  FabricCore::DFGExec exec,
  FTL::CStrRef nodeName,
  FabricUI::ModelItems::NodeModelItem *nodeModelItem
  )
{
  m_notifier.clear();
  m_subNotifier.clear();

  delete m_modelRoot;
  m_modelRoot = nodeModelItem;

  if ( nodeModelItem )
  {
    m_notifier = DFG::DFGExecNotifier::Create( exec );

    connect(
      m_notifier.data(),
      SIGNAL(nodeRenamed(FTL::CStrRef, FTL::CStrRef)),
      this,
      SLOT(onExecNodeRenamed(FTL::CStrRef, FTL::CStrRef))
      );
    connect(
      m_notifier.data(),
      SIGNAL(nodeRemoved(FTL::CStrRef)),
      this,
      SLOT(onExecNodeRemoved(FTL::CStrRef))
      );
    connect(
      m_notifier.data(),
      SIGNAL(nodePortRenamed(FTL::CStrRef, unsigned, FTL::CStrRef, FTL::CStrRef)),
      this,
      SLOT(onExecNodePortRenamed(FTL::CStrRef, unsigned, FTL::CStrRef, FTL::CStrRef))
      );
    connect(
      m_notifier.data(),
      SIGNAL( portsConnected( FTL::CStrRef, FTL::CStrRef ) ),
      this,
      SLOT( onExecPortsConnectedOrDisconnected( FTL::CStrRef, FTL::CStrRef ) )
      );
    connect(
      m_notifier.data(),
      SIGNAL( portsDisconnected( FTL::CStrRef, FTL::CStrRef ) ),
      this,
      SLOT( onExecPortsConnectedOrDisconnected( FTL::CStrRef, FTL::CStrRef ) )
      );
    connect(
      m_notifier.data(),
      SIGNAL(nodePortDefaultValuesChanged(FTL::CStrRef, FTL::CStrRef)),
      this,
      SLOT(onExecNodePortDefaultValuesChanged(FTL::CStrRef, FTL::CStrRef))
      );
    connect(
      m_notifier.data(),
      SIGNAL(nodePortResolvedTypeChanged(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef)),
      this,
      SLOT(onExecNodePortResolvedTypeChanged(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef))
      );

    if ( exec.getNodeType( nodeName.c_str() ) == FabricCore::DFGNodeType_Inst )
    {
      FabricCore::DFGExec subExec = exec.getSubExec( nodeName.c_str() );

      m_subNotifier = DFG::DFGExecNotifier::Create( subExec );

      connect(
        m_subNotifier.data(),
        SIGNAL(portMetadataChanged(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef)),
        this,
        SLOT(onExecPortMetadataChanged(FTL::CStrRef, FTL::CStrRef, FTL::CStrRef))
        );
      connect(
        m_subNotifier.data(),
        SIGNAL(portDefaultValuesChanged(FTL::CStrRef)),
        this,
        SLOT(onExecPortDefaultValuesChanged(FTL::CStrRef))
        );
    }
  }

  emit replaceModelRoot( m_modelRoot );
}

void VEEditorOwner::onControllerBindingChanged(
  FabricCore::DFGBinding const &newBinding
  )
{
  onStructureChanged();
  onSidePanelInspectRequested();
}

void VEEditorOwner::onSidePanelInspectRequested()
{
  FabricUI::DFG::DFGController *dfgController = getUIController();
  FabricCore::DFGBinding binding = dfgController->getBinding();
  std::string path = dfgController->getExecPath();
  if ( path.empty() )
  {
    setModelRoot(
      dfgController,
      new FabricUI::ModelItems::BindingModelItem(
        dfgController->getCmdHandler(),
        binding
        )
      );
  }
  else
  {
    // We always show the instantiated values (ie, what
    // we would see if we were outside this node and clicked on it)
    FabricUI::DFG::DFGUICmdHandler *dfgUICmdHandler =
      dfgController->getCmdHandler();
    FabricCore::DFGBinding &binding = dfgController->getBinding();
    FabricCore::DFGExec exec = binding.getExec();
    std::string nodeName = SplitLast( path );

    if (!path.empty())
      exec = exec.getSubExec( path.c_str() );

    setModelRoot(
      exec,
      nodeName,
      new FabricUI::ModelItems::InstModelItem(
        dfgUICmdHandler,
        binding,
        path,
        exec,
        nodeName
        )
      );
  }
}

void VEEditorOwner::onNodeInspectRequested(
  FabricUI::GraphView::Node *node
  )
{
  if (node->isBackDropNode())
    return;

  FabricUI::DFG::DFGController *dfgController =
    getUIController();

  FabricUI::DFG::DFGUICmdHandler *dfgUICmdHandler =
    dfgController->getCmdHandler();
  FabricCore::DFGBinding &binding = dfgController->getBinding();
  FTL::CStrRef execPath = dfgController->getExecPath();
  FabricCore::DFGExec &exec = dfgController->getExec();
  FTL::CStrRef nodeName = node->name();

  // TODO: Check for re-inspecting the same node, and don't rebuild
  FabricUI::ModelItems::NodeModelItem *nodeModelItem = 0;

  FabricCore::DFGNodeType type = exec.getNodeType( nodeName.c_str() );
  switch (type)
  {
    case FabricCore::DFGNodeType_Inst:
    {
      nodeModelItem =
        new FabricUI::ModelItems::InstModelItem(
          dfgUICmdHandler,
          binding,
          execPath,
          exec,
          nodeName
          );
      break;
    }
    case FabricCore::DFGNodeType_Var:
    {
      nodeModelItem =
        new FabricUI::ModelItems::VarModelItem(
          dfgUICmdHandler,
          binding,
          execPath,
          exec,
          nodeName
          );
      break;
    }
    case FabricCore::DFGNodeType_User:
    case FabricCore::DFGNodeType_Get:
    case FabricCore::DFGNodeType_Set:
    {
      //const char* path = exec.getRefVarPath( nodeName.c_str() );
      //m_modelRoot = new FabricUI::ModelItems::VarModelItem( exec, path );
      break;
    }
    default:
      assert( 0 && "Implement Me" );
  }

  setModelRoot( exec, nodeName, nodeModelItem );
}

void VEEditorOwner::onBindingArgValueChanged(
  unsigned index,
  FTL::CStrRef name
  )
{
  assert( m_modelRoot );

  try
  {
    if (name == NULL)
    {
      // TODO: Update all children
      //int nChildren = m_modelRoot->NumChildren();
    }
    else
    {
      BaseModelItem* changingChild = m_modelRoot->getChild( name, false );
      if (changingChild != NULL)
      {
        QVariant val = changingChild->getValue();
        changingChild->emitModelValueChanged( val );
      }
    }
  }
  catch (FabricCore::Exception e)
  {
    m_owner->log( e.getDesc_cstr() );
  }
}

void VEEditorOwner::onExecPortDefaultValuesChanged(
  FTL::CStrRef portName
  )
{
  assert( m_modelRoot );

  try
  {
    if ( BaseModelItem *changingChild =
      m_modelRoot->getChild( portName, false ) )
    {
      QVariant val = changingChild->getValue();
      changingChild->emitModelValueChanged( val );
    }
  }
  catch (FabricCore::Exception e)
  {
    m_owner->log( e.getDesc_cstr() );
  }
}

void VEEditorOwner::onExecNodePortDefaultValuesChanged(
  FTL::CStrRef nodeName,
  FTL::CStrRef portName
  )
{
  assert( m_modelRoot );
  assert( m_modelRoot->isNode() );
  NodeModelItem *nodeModelItem =
    static_cast<NodeModelItem *>( m_modelRoot );
  if ( nodeModelItem->getNodeName() != nodeName )
    return;

  try
  {
    if ( BaseModelItem *changingChild =
      m_modelRoot->getChild( portName, false ) )
    {
      QVariant val = changingChild->getValue();
      changingChild->emitModelValueChanged( val );
    }
  }
  catch (FabricCore::Exception e)
  {
    m_owner->log( e.getDesc_cstr() );
  }
}

void VEEditorOwner::onExecNodePortResolvedTypeChanged(
  FTL::CStrRef nodeName,
  FTL::CStrRef portName,
  FTL::CStrRef newResolveTypeName
  )
{
  assert( m_modelRoot );
  assert( m_modelRoot->isNode() );
  NodeModelItem *nodeModelItem =
    static_cast<NodeModelItem *>( m_modelRoot );
  if ( nodeModelItem->getNodeName() != nodeName )
    return;

  try
  {
    if ( BaseModelItem *changingChild =
      m_modelRoot->getChild( portName, false ) )
    {
      emit modelItemTypeChange( changingChild, newResolveTypeName.c_str() );
    }
  }
  catch (FabricCore::Exception e)
  {
    m_owner->log( e.getDesc_cstr() );
  }
}

void VEEditorOwner::onOutputsChanged()
{
  if (m_modelRoot == NULL)
    return;

  // We need to update all -out- values to reflect the
  // result of the new calculation
  FabricUI::ModelItems::ChildVec::iterator itr =
    m_modelRoot->GetChildItrBegin();
  FabricUI::ModelItems::ChildVec::iterator end =
    m_modelRoot->GetChildItrEnd();
  for (; itr != end; itr++)
  {
    BaseModelItem *childModelItem = *itr;
    if ( ItemMetadata *childItemMetadata = childModelItem->getMetadata() )
    {
      FTL::CStrRef vePortType =
        childItemMetadata->getString( ItemMetadata::VEPortTypeKey.c_str() );
      if ( vePortType != FTL_STR("In") )
      {
        QVariant val = childModelItem->getValue();
        childModelItem->emitModelValueChanged( val );
      }
    }
  }
}

void VEEditorOwner::onBindingArgInserted( unsigned index, FTL::CStrRef name, FTL::CStrRef type )
{
  assert( m_modelRoot );

  if ( m_modelRoot->argInserted( index, name.c_str(), type.c_str() ) )
  {
    emit modelItemInserted( m_modelRoot, int( index ), name.c_str() );
  }
}

void VEEditorOwner::onBindingArgTypeChanged( unsigned index, FTL::CStrRef name, FTL::CStrRef newType )
{
  assert( m_modelRoot );

  if (m_modelRoot->argTypeChanged( index, name.c_str(), newType.c_str() ))
  {
    BaseModelItem* changingChild = m_modelRoot->getChild( name, false );
    if (changingChild != NULL)
      emit modelItemTypeChange( changingChild, newType.c_str() );
  }
}

void VEEditorOwner::onBindingArgRemoved( unsigned index, FTL::CStrRef name )
{
  assert( m_modelRoot );

  BaseModelItem* removedChild = m_modelRoot->getChild( name, false );
  if (removedChild != NULL)
  {
    emit modelItemRemoved( removedChild );
    m_modelRoot->argRemoved( int( index ), name.c_str() );
  }
}

void VEEditorOwner::onBindingArgsReordered( FTL::ArrayRef<unsigned> newOrder )
{
  assert( m_modelRoot );

  // The array will specify the new order of our base arrays children
  // We will need to keep track of 
  QList<int> newIntOrder;
#if QT_VERSION >= 0x040800
  newIntOrder.reserve( newOrder.size() );
#endif
  for (size_t i = 0; i < newOrder.size(); i++)
    newIntOrder.push_back( int( newOrder[i] ) );

  //m_modelRoot->reorderChildren( newIntOrder );
  emit modelItemChildrenReordered( m_modelRoot, newIntOrder );
}

void VEEditorOwner::onBindingArgRenamed(
  unsigned argIndex,
  FTL::CStrRef oldArgName,
  FTL::CStrRef newArgName
  )
{
  assert( m_modelRoot );

  if ( BaseModelItem *changingChild =
    m_modelRoot->onPortRenamed(
      argIndex,
      oldArgName,
      newArgName
      ) )
    emit modelItemRenamed( changingChild );
}

void VEEditorOwner::onExecNodePortRenamed(
  FTL::CStrRef nodeName,
  unsigned portIndex,
  FTL::CStrRef oldPortName,
  FTL::CStrRef newPortName
  )
{
  assert( m_modelRoot );

  if ( BaseModelItem *changingChild =
    m_modelRoot->onPortRenamed(
      portIndex,
      oldPortName,
      newPortName
      ) )
    emit modelItemRenamed( changingChild );
}

void VEEditorOwner::onExecPortMetadataChanged(
  FTL::CStrRef portName,
  FTL::CStrRef key,
  FTL::CStrRef value
  )
{
  // Find the appropriate execPort
  assert( m_modelRoot );

  if ( key == FTL_STR("uiPersistValue") )
    return;

  BaseModelItem* changingChild = m_modelRoot->getChild( portName, false );
  // Only update if the change isn't coming from the child itself
    if ( changingChild != NULL && !changingChild->isSettingMetadata() )
  {
    // Our changing metadata could mean a changing type
    emit modelItemTypeChange( changingChild, "" );
  }
}


void VEEditorOwner::onExecNodeRemoved(
  FTL::CStrRef nodeName
  )
{
  assert( m_modelRoot );
  assert( m_modelRoot->isNode() );
  NodeModelItem *nodeModelItem =
    static_cast<NodeModelItem *>( m_modelRoot );
  if ( nodeModelItem->getNodeName() == nodeName )
  {
    emit modelItemRemoved( m_modelRoot );
    onSidePanelInspectRequested();
  }
}

void VEEditorOwner::onExecNodeRenamed(
  FTL::CStrRef oldNodeName,
  FTL::CStrRef newNodeName
  )
{
  assert( m_modelRoot );

  NodeModelItem *nodeModelItem =
    static_cast<NodeModelItem *>( m_modelRoot );
  if ( nodeModelItem->getName() == oldNodeName )
  {
    nodeModelItem->onRenamed( oldNodeName, newNodeName );
    emit modelItemRenamed( nodeModelItem );
  }
}

void VEEditorOwner::onExecPortsConnectedOrDisconnected(
  FTL::CStrRef srcPort,
  FTL::CStrRef dstPort
  )
{
  assert( m_modelRoot );

  if ( m_modelRoot->isNode() )
  {
    NodeModelItem *nodeModelItem =
      static_cast<NodeModelItem *>( m_modelRoot );

    std::string nodeName = srcPort.c_str();
    std::string portName = SplitLast( nodeName );
    if ( nodeName == nodeModelItem->getNodeName() )
    {
      if ( BaseModelItem* destChild =
        m_modelRoot->getChild( portName, false ) )
        emit modelItemTypeChange( destChild, "" );
    }

    nodeName = dstPort.c_str();
    portName = SplitLast( nodeName );
    if ( nodeName == nodeModelItem->getNodeName() )
    {
      if ( BaseModelItem* destChild =
        m_modelRoot->getChild( portName, false ) )
        emit modelItemTypeChange( destChild, "" );
    }
  }
}

void VEEditorOwner::onStructureChanged()
{
  if (getUIController()->isViewingRootGraph())
  {
    m_timelinePortIndex = -1;
    try
    {
      FabricCore::DFGExec graph =
        getUIController()->getExec();
      unsigned portCount = graph.getExecPortCount();
      for (unsigned i = 0; i < portCount; i++)
      {
        if (graph.getExecPortType( i ) == FabricCore::DFGPortType_Out)
          continue;
        FTL::CStrRef portName = graph.getExecPortName( i );
        if (portName != FTL_STR( "timeline" ))
          continue;
        if (!graph.isExecPortResolvedType( i, "SInt32" )
             && !graph.isExecPortResolvedType( i, "UInt32" )
             && !graph.isExecPortResolvedType( i, "Float32" )
             && !graph.isExecPortResolvedType( i, "Float64" ))
          continue;
        m_timelinePortIndex = int( i );
        break;
      }
    }
    catch (FabricCore::Exception e)
    {
      m_owner->log( e.getDesc_cstr() );
    }
  }
}

void VEEditorOwner::onFrameChanged( int frame )
{
  if (m_timelinePortIndex == -1)
    return;

  try
  {
    FabricCore::DFGBinding binding =
      getUIController()->getBinding();
    FabricCore::DFGExec exec = binding.getExec();
    FabricCore::Context ctxt = binding.getHost().getContext();

    if (exec.isExecPortResolvedType( m_timelinePortIndex, "SInt32" ))
      binding.setArgValue(
        m_timelinePortIndex,
        FabricCore::RTVal::ConstructSInt32( ctxt, frame ),
        false
        );
    else if (exec.isExecPortResolvedType( m_timelinePortIndex, "UInt32" ))
      binding.setArgValue(
        m_timelinePortIndex,
        FabricCore::RTVal::ConstructUInt32( ctxt, frame ),
        false
        );
    else if (exec.isExecPortResolvedType( m_timelinePortIndex, "Float32" ))
      binding.setArgValue(
        m_timelinePortIndex,
        FabricCore::RTVal::ConstructFloat32( ctxt, frame ),
        false
        );
    else if (exec.isExecPortResolvedType( m_timelinePortIndex, "Float64" ))
      binding.setArgValue(
        m_timelinePortIndex,
        FabricCore::RTVal::ConstructFloat64( ctxt, frame ),
        false
        );
  }
  catch (FabricCore::Exception e)
  {
    m_owner->log( e.getDesc_cstr() );
  }
}

void VEEditorOwner::onGraphSet( FabricUI::GraphView::Graph * graph )
{
  if(graph != m_setGraph)
  {
    connect( 
      graph, SIGNAL( sidePanelInspectRequested() ),
      this, SLOT( onSidePanelInspectRequested() )
      );
    connect(
      graph, SIGNAL( nodeInspectRequested( FabricUI::GraphView::Node* ) ),
      this, SLOT( onNodeInspectRequested( FabricUI::GraphView::Node* ) )
      );

    //if (m_setGraph == NULL)
    onSidePanelInspectRequested();

    m_setGraph = graph;
  }
}
