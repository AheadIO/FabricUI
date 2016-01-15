//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "BaseModelItem.h"
#include <FabricCore.h>

BaseModelItem::BaseModelItem()
  : m_interactionBracketCount( 0 )
  , m_modelValueChangedBracketCount( 0 )
{
}


BaseModelItem::~BaseModelItem()
{
}


BaseModelItem* BaseModelItem::GetChild( QString childName )
{
  int res = ChildIndex( childName );
  if (res >= 0)
    return GetChild( res );
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

void BaseModelItem::onInteractionEnd()
{
  if ( --m_interactionBracketCount == 0 )
  {
    SetValue(
      GetValue(),
      true, // commit
      m_valueAtInteractionBegin
      );
  }
}
