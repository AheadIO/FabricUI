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


size_t BaseModelItem::NumChildren()
{
  return 0;
}

BaseModelItem* BaseModelItem::GetChild( QString childName, bool doCreate )
{
  int res = ChildIndex( childName );
  if (res >= 0)
    return GetChild( res, doCreate );
  return NULL;
}

BaseModelItem* BaseModelItem::GetChild( int index, bool doCreate )
{
  return NULL;
}

QString BaseModelItem::ChildName( int i )
{
  BaseModelItem* pChild = GetChild( i );
  if (pChild != NULL)
    return pChild->GetName();
  return QString();
}

int BaseModelItem::ChildIndex( QString childName )
{
  for (size_t i = 0; i < NumChildren(); i++)
  {
    if (ChildName( i ) == childName)
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

void BaseModelItem::OnItemRenamed( QString newName )
{
}
