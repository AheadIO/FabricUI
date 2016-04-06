//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "NotInspectableViewItem.h"
#include "QVariantRTVal.h"

#include <assert.h>
#include <QtGui/QLabel>

using namespace FabricUI::ValueEditor;

NotInspectableViewItem::NotInspectableViewItem(
  QString name,
  QVariant value,
  ItemMetadata* metadata
  )
  : BaseViewItem( name, metadata )
  , m_label( new QLabel )
{
  m_label->setObjectName( "NotInspectableItem" );
  onModelValueChanged( value );
}

NotInspectableViewItem::~NotInspectableViewItem()
{
}

bool NotInspectableViewItem::hasChildren() const
{
  return false;
}

QWidget *NotInspectableViewItem::getWidget()
{
  return m_label;
}

void NotInspectableViewItem::onModelValueChanged( QVariant const &value )
{
  FabricCore::RTVal rtVal;
  RTVariant::toRTVal( value, rtVal );
  FTL::CStrRef typeName;
  if ( rtVal.isValid() )
    typeName = rtVal.getTypeNameCStr();
  if ( !typeName.empty() )
    m_label->setText( QString::fromUtf8( typeName.data(), typeName.size() ) );
  else
    m_label->setText( QString() );
}

void NotInspectableViewItem::appendChildViewItems( QList<BaseViewItem*>& items )
{
}

//////////////////////////////////////////////////////////
//
BaseViewItem* NotInspectableViewItem::CreateItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata *metadata
  )
{
  if ( metadata )
  {
    FTL::CStrRef veNotInspectable =
      metadata->getString( ItemMetadata::VENotInspectableKey.c_str() );
    if ( !veNotInspectable.empty() )
    {
      NotInspectableViewItem* pViewItem =
        new NotInspectableViewItem(
          QString( name ),
          QVariant( value ),
          metadata
          );
      return pViewItem;
    }
  }
  return 0;
}

const int NotInspectableViewItem::Priority = INT_MAX;
