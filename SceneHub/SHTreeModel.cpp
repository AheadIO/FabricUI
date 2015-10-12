/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#include "SHTreeModel.h"

using namespace FabricUI;
using namespace FabricUI::SceneHub;

SHTreeModel::SHTreeModel( FabricCore::Client client, FabricCore::RTVal sceneGraph, QObject *parent )
  : QAbstractItemModel( parent )
  , m_client( client )
  , m_sceneHierarchyChangedBlockCount( 0 )
{
  try {
    m_treeViewDataRTVal =
      FabricCore::RTVal::Create(
      m_client,
      "SGTreeViewData",
      1, &sceneGraph
      );

    // Try to optimize a bit: share RTVals when updating nodes
    // However, there RTVal don't have methods to update content so the benefit is limited for now.
    m_getUpdatedChildDataArgs[0] = FabricCore::RTVal::ConstructUInt32( m_client, 0 );//Size index
    m_getUpdatedChildDataArgs[1] = FabricCore::RTVal::ConstructData( m_client, NULL );//Data externalOwnerID
    m_getUpdatedChildDataArgs[2] = FabricCore::RTVal::ConstructBoolean( m_client, false );//io Boolean invalidate

    m_updateArgs[0] = FabricCore::RTVal::ConstructBoolean( m_client, false );//Boolean includeProperties
    m_updateArgs[1] = FabricCore::RTVal::Construct( m_client, "SGTreeViewObjectDataChanges", 0, 0 );//io SGTreeViewObjectDataChanges changes
  }
  catch ( FabricCore::Exception e )
  {
    printf("SHTreeModel::SHTreeModel: Error: %s\n", e.getDesc_cstr());
  }
}

std::vector< QModelIndex > SHTreeModel::getIndicesFromSGObject( FabricCore::RTVal sgObject ) {
  std::vector< QModelIndex > indices;
  try {
    FabricCore::RTVal dataArray = m_treeViewDataRTVal.callMethod("Data[]", "getExternalOwnerIDs", 1, &sgObject);

    unsigned int count = dataArray.getArraySize();
    for( unsigned int i = 0; i < count; ++i ) {
      SHTreeItem* item = (SHTreeItem*)dataArray.getArrayElement( i ).getData();
      if( item )
        indices.push_back( item->getIndex() );
    }
  }
  catch ( FabricCore::Exception e )
  {
    printf("SHTreeModel::getIndicesFromSGObject: Error: %s\n", e.getDesc_cstr());
  }
  return indices;
}
