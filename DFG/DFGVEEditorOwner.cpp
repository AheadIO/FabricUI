//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "DFGVEEditorOwner.h"

#include <FabricUI/DFG/DFGController.h>
#include <FabricUI/DFG/DFGExecNotifier.h>
#include <FabricUI/DFG/DFGWidget.h>
#include <FabricUI/GraphView/Node.h>
#include <FabricUI/ModelItems/BindingModelItem.h>
#include <FabricUI/ModelItems/GetModelItem.h>
#include <FabricUI/ModelItems/InstModelItem.h>
#include <FabricUI/ModelItems/SetModelItem.h>
#include <FabricUI/ModelItems/VarModelItem.h>
#include <FabricUI/ValueEditor/ItemMetadata.h>
#include <FabricUI/ValueEditor/BaseViewItem.h>
#include <FabricUI/ValueEditor/VETreeWidget.h>
#include <FabricUI/ValueEditor/VETreeWidgetItem.h>

using namespace FabricUI;
using namespace DFG;
using namespace ModelItems;

DFGVEEditorOwner::DFGVEEditorOwner( DFGWidget * dfgWidget )
  : m_timelinePortIndex( -1 )
  , m_dfgWidget(dfgWidget)
  , m_setGraph( NULL )
{
}

DFGVEEditorOwner::~DFGVEEditorOwner()
{
}

void DFGVEEditorOwner::initConnections()
{
  VEEditorOwner::initConnections();
  connect(
    getDFGController(),
    SIGNAL(bindingChanged(FabricCore::DFGBinding const &)),
    this,
    SLOT(onControllerBindingChanged(FabricCore::DFGBinding const &))
    );

  connect(
    getDFGController(), SIGNAL( argsChanged() ),
    this, SLOT( onStructureChanged() )
    );
  connect(  // [FE-6010]
    this, SIGNAL( modelItemRenamed( BaseModelItem* ) ),
    this, SLOT( onStructureChanged() )
    );

  connect(
    getDfgWidget(), SIGNAL( nodeInspectRequested( FabricUI::GraphView::Node* ) ),
    this, SLOT( onNodeInspectRequested( FabricUI::GraphView::Node* ) )
    );

  connect( getDfgWidget(), SIGNAL( onGraphSet( FabricUI::GraphView::Graph* ) ),
                    this, SLOT( onGraphSet( FabricUI::GraphView::Graph* ) ) );

  onGraphSet( getDfgWidget()->getUIGraph() );
}

FabricUI::DFG::DFGWidget * DFGVEEditorOwner::getDfgWidget()
{
  return m_dfgWidget;
}

FabricUI::DFG::DFGController * DFGVEEditorOwner::getDFGController()
{
  return getDfgWidget()->getDFGController();
}

void DFGVEEditorOwner::setModelRoot(
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

void DFGVEEditorOwner::setModelRoot(
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
      SIGNAL(nodePortInserted(FTL::CStrRef, unsigned, FTL::CStrRef)),
      this,
      SLOT(onExecNodePortInserted(FTL::CStrRef, unsigned, FTL::CStrRef))
      );
    connect(
      m_notifier.data(),
      SIGNAL(nodePortRenamed(FTL::CStrRef, unsigned, FTL::CStrRef, FTL::CStrRef)),
      this,
      SLOT(onExecNodePortRenamed(FTL::CStrRef, unsigned, FTL::CStrRef, FTL::CStrRef))
      );
    connect(
      m_notifier.data(),
      SIGNAL(nodePortRemoved(FTL::CStrRef, unsigned, FTL::CStrRef)),
      this,
      SLOT(onExecNodePortRemoved(FTL::CStrRef, unsigned, FTL::CStrRef))
      );
    connect(
      m_notifier.data(),
      SIGNAL(nodePortsReordered(FTL::CStrRef, FTL::ArrayRef<unsigned>)),
      this,
      SLOT(onExecNodePortsReordered(FTL::CStrRef, FTL::ArrayRef<unsigned>))
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
      SIGNAL( refVarPathChanged( FTL::CStrRef, FTL::CStrRef ) ),
      this,
      SLOT( onExecRefVarPathChanged( FTL::CStrRef, FTL::CStrRef ) )
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

void DFGVEEditorOwner::onControllerBindingChanged(
  FabricCore::DFGBinding const &newBinding
  )
{
  onStructureChanged();
  onSidePanelInspectRequested();
}

void DFGVEEditorOwner::onSidePanelInspectRequested()
{
  FabricUI::DFG::DFGController *dfgController = getDFGController();
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

void DFGVEEditorOwner::onNodeInspectRequested(
  FabricUI::GraphView::Node *node
  )
{
  if (node->isBackDropNode())
    return;

  FabricUI::DFG::DFGController *dfgController =
    getDFGController();

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
      nodeModelItem =
        new FabricUI::ModelItems::InstModelItem(
          dfgUICmdHandler,
          binding,
          execPath,
          exec,
          nodeName
          );
      break;
    
    case FabricCore::DFGNodeType_Var:
      nodeModelItem =
        new FabricUI::ModelItems::VarModelItem(
          dfgUICmdHandler,
          binding,
          execPath,
          exec,
          nodeName
          );
      break;
    
    case FabricCore::DFGNodeType_Get:
      nodeModelItem =
        new FabricUI::ModelItems::GetModelItem(
          dfgUICmdHandler,
          binding,
          execPath,
          exec,
          nodeName
          );
      break;
    
    case FabricCore::DFGNodeType_Set:
      nodeModelItem =
        new FabricUI::ModelItems::SetModelItem(
          dfgUICmdHandler,
          binding,
          execPath,
          exec,
          nodeName
          );
      break;
    
    case FabricCore::DFGNodeType_User:
      break;
    
    default:
      assert( 0 && "Implement Me" );
      break;
  }

  setModelRoot( exec, nodeName, nodeModelItem );
}

void DFGVEEditorOwner::onBindingArgValueChanged(
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
    emit log( e.getDesc_cstr() );
  }
}

void DFGVEEditorOwner::onExecPortDefaultValuesChanged(
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
    emit log( e.getDesc_cstr() );
  }
}

void DFGVEEditorOwner::onExecNodePortDefaultValuesChanged(
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
    emit log( e.getDesc_cstr() );
  }
}

void DFGVEEditorOwner::onExecNodePortResolvedTypeChanged(
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
    emit log( e.getDesc_cstr() );
  }
}

void DFGVEEditorOwner::onOutputsChanged()
{
  VEEditorOwner::onOutputsChanged();
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

void DFGVEEditorOwner::onBindingArgInserted( unsigned index, FTL::CStrRef name, FTL::CStrRef type )
{
  assert( m_modelRoot );
  if ( m_modelRoot->isBinding() )
  {
    emit modelItemInserted( m_modelRoot, int( index ), name.c_str() );
  }
}

void DFGVEEditorOwner::onBindingArgTypeChanged( unsigned index, FTL::CStrRef name, FTL::CStrRef newType )
{
  assert( m_modelRoot );
  if ( m_modelRoot->isBinding() )
  {
    BaseModelItem *changingChild = m_modelRoot->getChild( name, false );
    if ( changingChild != NULL )
      emit modelItemTypeChange( changingChild, newType.c_str() );
  }
}

void DFGVEEditorOwner::onBindingArgRemoved( unsigned index, FTL::CStrRef name )
{
  assert( m_modelRoot );
  if ( m_modelRoot->isBinding() )
  {
    BindingModelItem *bindingModelItem =
      static_cast<BindingModelItem *>( m_modelRoot );
    BaseModelItem* removedChild = m_modelRoot->getChild( name, false );
    if ( removedChild != NULL )
    {
      emit modelItemRemoved( removedChild );
      bindingModelItem->argRemoved( index, name );
    }
  }
}

void DFGVEEditorOwner::onBindingArgsReordered( FTL::ArrayRef<unsigned> newOrder )
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

void DFGVEEditorOwner::onBindingArgRenamed(
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

void DFGVEEditorOwner::onExecNodePortInserted(
  FTL::CStrRef nodeName,
  unsigned portIndex,
  FTL::CStrRef portName
  )
{
  assert( m_modelRoot );
  if ( m_modelRoot->isNode() )
  {
    emit modelItemInserted( m_modelRoot, int( portIndex ), portName.c_str() );
  }
}

void DFGVEEditorOwner::onExecNodePortRenamed(
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

void DFGVEEditorOwner::onExecNodePortRemoved(
  FTL::CStrRef nodeName,
  unsigned portIndex,
  FTL::CStrRef portName
  )
{
  assert( m_modelRoot );
  if ( m_modelRoot->isNode() )
  {
    BindingModelItem *bindingModelItem =
      static_cast<BindingModelItem *>( m_modelRoot );
    BaseModelItem* removedChild = m_modelRoot->getChild( portName, false );
    if ( removedChild != NULL )
    {
      emit modelItemRemoved( removedChild );
      bindingModelItem->argRemoved( portIndex, portName );
    }
  }
}

void DFGVEEditorOwner::onExecNodePortsReordered(
  FTL::CStrRef nodeName,
  FTL::ArrayRef<unsigned> newOrder
  )
{
  assert( m_modelRoot );
  if ( m_modelRoot->isNode() )
  {
    QList<int> newIntOrder;
    #if QT_VERSION >= 0x040800
      newIntOrder.reserve( newOrder.size() );
    #endif
    for (size_t i = 0; i < newOrder.size(); i++)
      newIntOrder.push_back( int( newOrder[i] ) );

    emit modelItemChildrenReordered( m_modelRoot, newIntOrder );
  }
}

void DFGVEEditorOwner::onExecPortMetadataChanged(
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

void DFGVEEditorOwner::onExecNodeRemoved(
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

void DFGVEEditorOwner::onExecNodeRenamed(
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

void DFGVEEditorOwner::onExecPortsConnectedOrDisconnected(
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

void DFGVEEditorOwner::onExecRefVarPathChanged(
  FTL::CStrRef refName,
  FTL::CStrRef newVarPath
  )
{
  assert( m_modelRoot );

  if ( m_modelRoot->isNode() )
  {
    NodeModelItem *nodeModelItem =
      static_cast<NodeModelItem *>( m_modelRoot );
    if ( nodeModelItem->isRef() )
    {
      RefModelItem *refModelItem =
        static_cast<RefModelItem *>( nodeModelItem );
      if ( refModelItem->getNodeName() == refName )
      {
        if ( BaseModelItem *changingChild =
          refModelItem->getChild( FTL_STR("varPath"), false ) )
        {
          QVariant val = changingChild->getValue();
          changingChild->emitModelValueChanged( val );
        }
      }
    }
  }
}

void DFGVEEditorOwner::onStructureChanged()
{
  if (getDFGController()->isViewingRootGraph())
  {
    m_timelinePortIndex = -1;
    try
    {
      FabricCore::DFGExec graph =
        getDFGController()->getExec();
      unsigned portCount = graph.getExecPortCount();
      for (unsigned i = 0; i < portCount; i++)
      {
        if (graph.getExecPortType( i ) == FabricCore::DFGPortType_Out)
          continue;
        FTL::CStrRef portName = graph.getExecPortName( i );
        if (portName != FTL_STR( "timeline" ))
          continue;
        if (    !graph.isExecPortResolvedType( i, "SInt32" )
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
      emit log( e.getDesc_cstr() );
    }
  }
}

void DFGVEEditorOwner::onFrameChanged( int frame )
{
  VEEditorOwner::onFrameChanged(frame);
  if (m_timelinePortIndex == -1)
    return;

  try
  {
    FabricCore::DFGBinding binding =
      getDFGController()->getBinding();
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
    emit log( e.getDesc_cstr() );
  }
}

void DFGVEEditorOwner::onGraphSet( FabricUI::GraphView::Graph * graph )
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

    
    onSidePanelInspectRequested();

    m_setGraph = graph;
  }
}
