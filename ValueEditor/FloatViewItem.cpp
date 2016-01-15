//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "FloatViewItem.h"
#include "QVariantRTVal.h"

#include <QtGui/QSpinBox>
#include <float.h>

FloatViewItem::FloatViewItem(
  QString const &name,
  QVariant const &value
  )
  : BaseViewItem(name)
{
  m_spinner = new QDoubleSpinBox;
  m_spinner->setMinimum( -DBL_MAX );
  m_spinner->setMaximum( +DBL_MAX );
  m_spinner->setKeyboardTracking( false );
  connect(
    m_spinner, SIGNAL( valueChanged( double ) ), 
    this, SLOT( OnSpinnerChanged( double ) )
    );
  connect(
    m_spinner, SIGNAL( editingFinished() ), 
    this, SLOT( OnEditFinished() )
    );
  onModelValueChanged( value );
}

FloatViewItem::~FloatViewItem()
{
}

QWidget *FloatViewItem::getWidget()
{
  return m_spinner;
}

void FloatViewItem::onModelValueChanged( QVariant const &v )
{
  m_spinner->setValue( v.value<double>() );
}

void FloatViewItem::OnSpinnerChanged( double value )
{
  emit viewValueChanged(
    QVariant::fromValue<double>( value )
    );
}

void FloatViewItem::OnEditFinished()
{
  emit viewValueChanged(
    QVariant::fromValue<double>( m_spinner->value() )
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
