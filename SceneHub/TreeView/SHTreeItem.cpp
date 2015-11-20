/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#include "SHTreeItem.h"
#include "SHTreeModel.h"

using namespace FabricUI;
using namespace FabricUI::SceneHub;

void SHTreeItem::updateChildItemIfNeeded( int row ) {
  if( m_childItems[row].m_updateNeeded ) {
    m_childItems[row].m_updateNeeded = false;

    SHTreeItem *childItem = m_childItems[row].m_child.get();

    m_model->m_getUpdatedChildDataArgs[0] = FabricCore::RTVal::ConstructUInt32( m_client, row );//Size index (TODO: RTVal don't have method to update content for now...)
    m_model->m_getUpdatedChildDataArgs[1].setData( childItem );

    FabricCore::RTVal childDataRTVal =
      m_treeViewObjectDataRTVal.callMethod(
      "SGTreeViewObjectData",
      "getUpdatedChildData",
      3, m_model->m_getUpdatedChildDataArgs
      );

    bool invalidate = m_model->m_getUpdatedChildDataArgs[2].getBoolean();
    childItem->updateNeeded( childDataRTVal, invalidate );
    if( invalidate ) // Target object might have been created
      m_model->emitSceneHierarchyChanged();
  }
}

void SHTreeItem::updateChildItemsIfNeeded() {
  if( m_needsUpdate && m_treeViewObjectDataRTVal.isValid() && !m_treeViewObjectDataRTVal.isNullObject() ) {
    //std::cerr << "updateChildItemsIfNeeded " << (const char*)(desc().data()) << " " << std::endl;

    try {
      //NOTE: normally we should be able to "reuse" the content m_updateArgs[1] but there seems to be a bug (getArraySize isn't refreshing)
      //A bug has been logged for this. To repro the bug, comment out the following line.
      m_model->m_updateArgs[1] = FabricCore::RTVal::Construct( m_client, "SGTreeViewObjectDataChanges", 0, 0 );//io SGTreeViewObjectDataChanges changes
      unsigned int result = m_treeViewObjectDataRTVal.callMethod("UInt32", "update", 2, m_model->m_updateArgs).getUInt32();

      // If result is 0: no scene changes; no need to even recurse into children
      if( result != 0 ) {
        if( result == 2 ) {
          // Layout changed: we need to remove then insert rows
          FabricCore::RTVal removedItems = m_model->m_updateArgs[1].maybeGetMember( "removed" );
          unsigned int removedCount = removedItems.getArraySize();

          for( unsigned int i = 0; i < removedCount; ++i ) {
            unsigned int index = removedItems.getArrayElement( i ).getUInt32();
            assert( index < m_childItems.size() );
            m_model->beginRemoveRows( m_index, index, index );
            m_childItems.erase( m_childItems.begin() + index );
            m_model->endRemoveRows();
          }

          FabricCore::RTVal insertedItems = m_model->m_updateArgs[1].maybeGetMember( "inserted" );
          unsigned int insertedCount = insertedItems.getArraySize();

          if( insertedCount ) {
            FabricCore::RTVal insertedItemNames = m_model->m_updateArgs[1].maybeGetMember( "insertedNames" );

            for( unsigned int i = 0; i < insertedCount; ++i ) {
              unsigned int index = insertedItems.getArrayElement( i ).getUInt32();
              assert( index <= m_childItems.size() );
              m_model->beginInsertRows( m_index, index, index );

              ChildItem childItem;
              childItem.m_child = new SHTreeItem( m_model, this, m_client );

              QString childName = insertedItemNames.getArrayElement( i ).getStringCString();
              childItem.m_child->setName( childName );
              
              m_childItems.insert( m_childItems.begin() + index, childItem );

              m_model->endInsertRows();
            }
          }
          if( insertedCount != 0 || removedCount != 0 ) {
            // Update childs' QModelIndex
            for( unsigned int i = 0; i < m_childItems.size(); ++i )
              m_childItems[i].m_child.get()->setIndex( m_model->createIndex( i, 0, m_childItems[i].m_child.get() ) );

            m_model->emitSceneHierarchyChanged();
          }
        }

        // All children might need to be updated
        unsigned int row = 0;
        for( ChildItemVec::iterator it = m_childItems.begin(); it != m_childItems.end(); ++it, ++row ) {
          it->m_updateNeeded = true;
          SHTreeItem *childItem = it->m_child.get();

          // Update child right away if it was already expanded
          // TODO: don't update if collapsed!
          if( childItem->m_hadInitialUpdate ) {
            updateChildItemIfNeeded( row );
            childItem->updateChildItemsIfNeeded();
          }
        }
      }
    }
    catch ( FabricCore::Exception e )
    {
      printf("Error: %s\n", e.getDesc_cstr());
    }
    m_hadInitialUpdate = true;
  }
  m_needsUpdate = false;
}

void SHTreeItem::loadRecursively() {
  try
  {
    FabricCore::RTVal sgObject = getSGObject();
    sgObject.callMethod( "", "forceHierarchyExpansion", 0, 0 );
  }
  catch( FabricCore::Exception e ) {
    printf( "Error: %s\n", e.getDesc_cstr() );
  }
  m_model->emitSceneHierarchyChanged();
}

