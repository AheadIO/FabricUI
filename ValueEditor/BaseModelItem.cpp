//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "BaseModelItem.h"
#include <FabricCore.h>

BaseModelItem::BaseModelItem()
  : m_interactionBracketCount( 0 )
  , m_modelValueChangedBracketCount( 0 )
  , m_settingMetadata( false ) 
{
}


BaseModelItem::~BaseModelItem()
{
}


int BaseModelItem::getNumChildren()
{
  return 0;
}

BaseModelItem* BaseModelItem::getChild( FTL::CStrRef childName, bool doCreate )
{
  int res = getChildIndex( childName );
  if (res >= 0)
    return getChild( res, doCreate );
  return NULL;
}

BaseModelItem* BaseModelItem::getChild( int index, bool doCreate )
{
  return NULL;
}

FTL::CStrRef BaseModelItem::getChildName( int i )
{
  BaseModelItem* pChild = getChild( i );
  if ( pChild != NULL )
    return pChild->getName();
  return FTL::CStrRef();
}

int BaseModelItem::getChildIndex( FTL::CStrRef childName )
{
  int numChildren = getNumChildren();
  for ( int i = 0; i < numChildren; i++ )
  {
    if (getChildName( i ) == childName)
      return i;
  }
  return -1;
}

ItemMetadata* BaseModelItem::GetMetadata()
{
  return NULL;
}


void BaseModelItem::SetMetadata( const char* key, const char* val, bool canUndo )
{
  m_settingMetadata = true;
  SetMetadataImp( key, val, canUndo );
  m_settingMetadata = false;
}

bool BaseModelItem::SettingMetadata()
{
  return m_settingMetadata;
}

int BaseModelItem::GetInOut()
{
  return FabricCore::DFGPortType_In;
}

void BaseModelItem::onInteractionBegin()
{
  if ( m_interactionBracketCount++ == 0 )
    m_valueAtInteractionBegin = GetValue();
}

void BaseModelItem::onViewValueChanged(
  QVariant value
  )
{
  if ( m_modelValueChangedBracketCount == 0 )
  {
    SetValue(
      value,
      m_interactionBracketCount == 0,
      m_valueAtInteractionBegin
      );
  }
}

void BaseModelItem::onInteractionEnd( bool accept )
{
  if ( --m_interactionBracketCount == 0 )
  {
    if ( accept )
    {
      SetValue(
        GetValue(),
        true, // commit
        m_valueAtInteractionBegin
        );
    }
    else
    {
      SetValue(
        m_valueAtInteractionBegin,
        false,
        m_valueAtInteractionBegin
        );
    }

    m_valueAtInteractionBegin = QVariant();
  }
}
