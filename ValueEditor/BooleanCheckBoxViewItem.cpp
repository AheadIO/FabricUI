//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "BooleanCheckBoxViewItem.h"
#include "ItemMetadata.h"
#include "QVariantRTVal.h"

#include <QtCore/QVariant>
#include <QtGui/QCheckBox>

BooleanCheckBoxViewItem::BooleanCheckBoxViewItem(
  QString const &name,
  QVariant const &value
  )
  : BaseViewItem( name )
{
  m_checkBox = new QCheckBox;
  connect(
    m_checkBox, SIGNAL( stateChanged( int ) ),
    this, SLOT( onStateChanged( int ) )
    );

  onModelValueChanged( value );
}

BooleanCheckBoxViewItem::~BooleanCheckBoxViewItem()
{
}

QWidget *BooleanCheckBoxViewItem::getWidget()
{
  return m_checkBox;
}

void BooleanCheckBoxViewItem::onModelValueChanged( QVariant const &v )
{
  m_checkBox->setChecked( v.value<bool>() );
}

void BooleanCheckBoxViewItem::updateMetadata( ItemMetadata* metaData ) 
{
}

void BooleanCheckBoxViewItem::onStateChanged( int value )
{
  emit viewValueChanged(
    QVariant::fromValue( bool( value ) ),
    0
    );
}

//////////////////////////////////////////////////////////////////////////
// 
BaseViewItem *BooleanCheckBoxViewItem::CreateItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metaData
  )
{
  if (RTVariant::isType<bool>(value))
  {
    BooleanCheckBoxViewItem* item =
      new BooleanCheckBoxViewItem( name, value );
    item->updateMetadata( metaData );
    return item;
  }
  return NULL;
}

const int BooleanCheckBoxViewItem::Priority = 3;
