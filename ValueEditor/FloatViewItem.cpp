//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "FloatViewItem.h"
#include "QVariantRTVal.h"
#include "VESpinBox.h"

#include <float.h>

FloatViewItem::FloatViewItem(
  QString const &name,
  QVariant const &value
  )
  : BaseViewItem(name)
{
  m_spinBox = new VESpinBox( value.value<double>() );
  connect(
    m_spinBox, SIGNAL(interactionBegin()), 
    this, SIGNAL(interactionBegin())
    );
  connect(
    m_spinBox, SIGNAL(valueChanged(double)), 
    this, SLOT(onSpinBoxValueChanged(double))
    );
  connect(
    m_spinBox, SIGNAL(interactionEnd(bool)), 
    this, SIGNAL(interactionEnd(bool))
    );
}

FloatViewItem::~FloatViewItem()
{
}

QWidget *FloatViewItem::getWidget()
{
  return m_spinBox;
}

void FloatViewItem::onModelValueChanged( QVariant const &v )
{
  m_spinBox->setValue( v.value<double>() );
}

void FloatViewItem::onSpinBoxValueChanged( double value )
{
  emit viewValueChanged(
    QVariant::fromValue<double>( value )
    );
}

//////////////////////////////////////////////////////////////////////////
// 
BaseViewItem* FloatViewItem::CreateItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* /*metaData*/
  )
{
  if (RTVariant::isType<double>(value) || RTVariant::isType<float>(value))
  {
    return new FloatViewItem( name, value );
  }
  return 0;
}

const int FloatViewItem::Priority = 3;
