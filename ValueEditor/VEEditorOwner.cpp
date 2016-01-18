
#include "VEEditorOwner.h"
#include <FabricUI/ValueEditor/VETreeWidget.h>
#include <FabricUI/GraphView/Node.h>
#include <FabricUI/DFG/DFGWidget.h>
#include <FabricUI/DFG/DFGController.h>

#include <FabricUI/ModelItems/NodeModelItem.h>
#include <FabricUI/ModelItems/VarModelItem.h>
#include <FabricUI/ModelItems/BindingModelItem.h>
#include <FabricUI/ModelItems/PresetModelItem.h>

using namespace FabricUI;
using namespace ValueEditor;

VEEditorOwner::VEEditorOwner( ValueEditorBridgeOwner& owner )
  : m_owner( owner )
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
  // new DFG::DFGValueEditor(
  //   getController(),
  //   config
  //   );
  QObject::connect(
    getUIController(), SIGNAL( argInserted( int, const char*, const char* ) ),
    this, SLOT( onArgInserted( int, const char*, const char* ) )
    );
  QObject::connect(
    getUIController(), SIGNAL( argTypeChanged( int, const char*, const char* ) ),
    this, SLOT( onArgTypeChanged( int, const char*, const char* ) )
    );

  QObject::connect(
    getUIController(), SIGNAL( argRemoved( int, const char* ) ),
    this, SLOT( onArgRemoved( int, const char* ) )
    );

  QObject::connect(
    getUIController(), SIGNAL( argsChanged() ),
    this, SLOT( onStructureChanged() )
    );
  QObject::connect(
    getUIController(), SIGNAL( argValuesChanged( int, const char* ) ),
    this, SLOT( onValueChanged( int, const char* ) )
    );
  QObject::connect(
    getUIController(), SIGNAL( argsReordered( const FTL::JSONArray* ) ),
    this, SLOT( onArgsReordered( const FTL::JSONArray* ) )
    );

  QObject::connect(
    getUIController(), SIGNAL( defaultValuesChanged( int, const char* ) ),
    this, SLOT( onValueChanged( int, const char* ) )
    );
  QObject::connect(
    getDfgWidget(), SIGNAL( nodeInspectRequested( FabricUI::GraphView::Node* ) ),
    this, SLOT( onNodeInspectRequested( FabricUI::GraphView::Node* ) )
    );
  //QObject::connect(
  //  getDFGController(), SIGNAL( dirty() ),
  //  this, SLOT( onDirty() )
  //  );

  QObject::connect(
    getDFGController(), SIGNAL( execPortRenamed( const char* , const char* ) ),
    this, SLOT( onExecPortRenamed( const char*, const char* ) )
    );

  QObject::connect(
    getDFGController(), SIGNAL( nodeRemoved( FTL::CStrRef, FTL::CStrRef ) ),
    this, SLOT( onNodeRemoved( FTL::CStrRef, FTL::CStrRef ) )
    );
  QObject::connect(
    getDFGController(), SIGNAL( nodeRenamed( FTL::CStrRef, FTL::CStrRef, FTL::CStrRef ) ),
    this, SLOT( onNodeRenamed( FTL::CStrRef, FTL::CStrRef, FTL::CStrRef ) )
    );

  QObject::connect(
    getDFGController(), SIGNAL( portsConnected( FTL::CStrRef, FTL::CStrRef ) ),
    this, SLOT( onPortsConnected( FTL::CStrRef, FTL::CStrRef ) )
    );
  QObject::connect(
    getDFGController(), SIGNAL( portsDisconnected( FTL::CStrRef, FTL::CStrRef ) ),
    this, SLOT( onPortsDisconnected( FTL::CStrRef, FTL::CStrRef ) )
    );

  QObject::connect(
    this, SIGNAL( modelItemInserted( BaseModelItem*, int, const char* ) ),
    m_dfgValueEditor, SLOT( onModelItemChildInserted( BaseModelItem*, int, const char* ) )
    );
  QObject::connect(
    this, SIGNAL( modelItemTypeChange( BaseModelItem*, const char* ) ),
    m_dfgValueEditor, SLOT( onModelItemTypeChanged( BaseModelItem*, const char* ) )
    );
  QObject::connect(
    this, SIGNAL( modelItemRemoved( BaseModelItem* ) ),
    m_dfgValueEditor, SLOT( onModelItemRemoved( BaseModelItem* ) )
    );
  QObject::connect(
    this, SIGNAL( modelItemRenamed( BaseModelItem* ) ),
    m_dfgValueEditor, SLOT( onModelItemRenamed( BaseModelItem* ) )
    );
  QObject::connect(
    this, SIGNAL( modelItemChildrenReordered( BaseModelItem*, const QList<int>& ) ),
    m_dfgValueEditor, SLOT( onModelItemChildrenReordered( BaseModelItem*, const QList<int>& ) )
    );
  // QObject::connect(
//   getDFGController(), SIGNAL(bindingChanged(FabricCore::DFGBinding const &)),
//   m_dfgValueEditor, SLOT(setBinding(FabricCore::DFGBinding const &))
//   );
  QObject::connect(
    this, SIGNAL( replaceModelRoot( BaseModelItem* ) ),
    m_dfgValueEditor, SLOT( onSetModelItem( BaseModelItem* ) )
    );

  QObject::connect( getDfgWidget(), SIGNAL( onGraphSet( FabricUI::GraphView::Graph* ) ),
                    this, SLOT( onGraphSet( FabricUI::GraphView::Graph* ) ) );

  onGraphSet( getDfgWidget()->getUIGraph() );
}

FabricUI::DFG::DFGWidget * VEEditorOwner::getDfgWidget()
{
  return m_owner.getDfgWidget();
}
FabricUI::DFG::DFGController * VEEditorOwner::getUIController()
{
  return getDfgWidget()->getUIController();
}
FabricUI::DFG::DFGController * VEEditorOwner::getDFGController()
{
  return getDfgWidget()->getDFGController();
}

void VEEditorOwner::onSidePanelInspectRequested()
{
  FabricUI::DFG::DFGController *dfgController =
    getUIController();

  FTL::CStrRef path = dfgController->getExecPath();
  if (path.empty())
  {
    FabricCore::DFGBinding binding = dfgController->getBinding();
    m_modelRoot =
      new FabricUI::ModelItems::BindingModelItem(
        dfgController->getCmdHandler(),
        binding
        );
    emit replaceModelRoot( m_modelRoot );
  }
  else
  {
    FabricCore::DFGExec exec = dfgController->getExec();
    m_modelRoot = new FabricUI::ModelItems::PresetModelItem( exec );
    emit replaceModelRoot( m_modelRoot );
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
  FabricUI::ModelItems::RootModelItem* newItem = NULL;

  FabricCore::DFGNodeType type = exec.getNodeType( nodeName.c_str() );
  switch (type)
  {
    case FabricCore::DFGNodeType_Inst:
    case FabricCore::DFGNodeType_User:
    {
      newItem =
        new FabricUI::ModelItems::NodeModelItem(
          dfgUICmdHandler,
          binding,
          execPath,
          exec,
          nodeName,
          QString::fromUtf8( nodeName.data(), nodeName.size() )
          );
      break;
    }
    case FabricCore::DFGNodeType_Var:
    {
      newItem =
        new FabricUI::ModelItems::VarModelItem(
          dfgUICmdHandler,
          binding,
          execPath,
          exec,
          nodeName,
          QString::fromUtf8( nodeName.data(), nodeName.size() )
          );
      break;
    }
    // case FabricCore::DFGNodeType_Get:
    // case FabricCore::DFGNodeType_Set:
    // {
    //   const char* path = exec.getRefVarPath( nodeName.c_str() );
    //   m_modelRoot = new FabricUI::ModelItems::VarModelItem( exec, path );
    //   break;
    // }
    default:
      assert( 0 && "Implement Me" );
  }

  if (newItem != NULL)
  {
    delete m_modelRoot;
    m_modelRoot = newItem;
    emit replaceModelRoot( m_modelRoot );
  }
}


////
// Our Slots:
void VEEditorOwner::onValueChanged( int index, const char* name )
{
  if (m_modelRoot == NULL)
    return;

  try
  {
    if (name == NULL)
    {
      // TODO: Update all children
      //int nChildren = m_modelRoot->NumChildren();
    }
    else
    {
      BaseModelItem* changingChild = m_modelRoot->GetChild( name );
      if (changingChild != NULL)
      {
        QVariant val = changingChild->GetValue();
        changingChild->emitModelValueChanged( val );
      }
    }
  }
  catch (FabricCore::Exception e)
  {
    m_owner.log( e.getDesc_cstr() );
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
    if ((*itr)->GetInOut() != FabricCore::DFGPortType_In)
    {
      QVariant val = (*itr)->GetValue();
      (*itr)->emitModelValueChanged( val );
    }
  }
}

void VEEditorOwner::onArgInserted( int index, const char* name, const char* type )
{
  if (m_modelRoot == NULL)
    return;

  if (m_modelRoot->argInserted( index, name, type ))
  {
    emit modelItemInserted( m_modelRoot, index, name );
  }

  onStructureChanged();
}

void VEEditorOwner::onArgTypeChanged( int index, const char* name, const char* newType )
{
  if (m_modelRoot == NULL)
    return;

  if (m_modelRoot->argTypeChanged( index, name, newType ))
  {
    BaseModelItem* changingChild = m_modelRoot->GetChild( name );
    emit modelItemTypeChange( changingChild, newType );
  }

  onStructureChanged();
}

void VEEditorOwner::onArgRemoved( int index, const char* name )
{
  if (m_modelRoot == NULL)
    return;

  BaseModelItem* removedChild = m_modelRoot->GetChild( name );
  emit modelItemRemoved( removedChild );
  m_modelRoot->argRemoved( index, name );

  onStructureChanged();
}

void VEEditorOwner::onArgsReordered( const FTL::JSONArray* newOrder )
{
  if (newOrder == NULL || m_modelRoot == NULL)
    return;

  // The array will specify the new order of our base arrays children
  // We will need to keep track of 
  QList<int> newIntOrder;
#if QT_VERSION >= 0x040800
  newIntOrder.reserve( newOrder->size() );
#endif
  for (size_t i = 0; i < newOrder->size(); i++)
    newIntOrder.push_back( newOrder->get( i )->getSInt32Value() );

  //m_modelRoot->reorderChildren( newIntOrder );
  emit modelItemChildrenReordered( m_modelRoot, newIntOrder );

  onStructureChanged();
}



void VEEditorOwner::onExecPortRenamed( const char* oldName, const char* newName )
{
  // Find the appropriate execPort
  if (m_modelRoot != NULL)
  {
    BaseModelItem* changingChild = m_modelRoot->argRenamed( oldName, newName );
    if ( changingChild != NULL )
    {
      emit modelItemRenamed( changingChild );
    }
  }
}


void VEEditorOwner::onNodeRemoved( FTL::CStrRef execPath, FTL::CStrRef nodeName )
{
  if (m_modelRoot != NULL)
  {
    if (m_modelRoot->matchesPath( execPath, nodeName ))
    {
      emit modelItemRemoved( m_modelRoot );
      onSidePanelInspectRequested();
    }
  }
}

void VEEditorOwner::onNodeRenamed( FTL::CStrRef execPath, FTL::CStrRef oldNodeName, FTL::CStrRef newNodeName )
{
  if (m_modelRoot != NULL)
  {
    if (m_modelRoot->matchesPath( execPath, oldNodeName ))
    {
      m_modelRoot->OnItemRenamed( newNodeName.c_str() );
      emit modelItemRenamed( m_modelRoot );
    }
  }
}

void VEEditorOwner::onPortsConnected( FTL::CStrRef srcPort, FTL::CStrRef dstPort )
{
  if (m_modelRoot != NULL)
  {
    FTL::CStrRef::Split split = dstPort.rsplit( '.' );

    if (m_modelRoot->matchesPath( split.first, "" ))
    {
      BaseModelItem* pChild = m_modelRoot->GetChild( split.second.c_str() );
      if (pChild != NULL)
        emit modelItemTypeChange( pChild, "" );
    }
  }
}

void VEEditorOwner::onPortsDisconnected( FTL::CStrRef srcPort, FTL::CStrRef dstPort )
{
  if (m_modelRoot != NULL)
  {
    FTL::CStrRef::Split split = dstPort.rsplit( '.' );

    if (m_modelRoot->matchesPath( split.first, "" ))
    {
      BaseModelItem* pChild = m_modelRoot->GetChild( split.second.c_str() );
      if (pChild != NULL)
        emit modelItemTypeChange( pChild, "" );
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
      m_owner.log( e.getDesc_cstr() );
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
    FabricCore::Client client; // = binding.getClient();

    if (exec.isExecPortResolvedType( m_timelinePortIndex, "SInt32" ))
      binding.setArgValue(
        m_timelinePortIndex,
        FabricCore::RTVal::ConstructSInt32( client, frame ),
        false
        );
    else if (exec.isExecPortResolvedType( m_timelinePortIndex, "UInt32" ))
      binding.setArgValue(
        m_timelinePortIndex,
        FabricCore::RTVal::ConstructUInt32( client, frame ),
        false
        );
    else if (exec.isExecPortResolvedType( m_timelinePortIndex, "Float32" ))
      binding.setArgValue(
        m_timelinePortIndex,
        FabricCore::RTVal::ConstructFloat32( client, frame ),
        false
        );
    else if (exec.isExecPortResolvedType( m_timelinePortIndex, "Float64" ))
      binding.setArgValue(
        m_timelinePortIndex,
        FabricCore::RTVal::ConstructFloat64( client, frame ),
        false
        );
  }
  catch (FabricCore::Exception e)
  {
    m_owner.log( e.getDesc_cstr() );
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
