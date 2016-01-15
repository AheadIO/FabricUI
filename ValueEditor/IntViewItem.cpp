//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "IntViewItem.h"
#include "QVariantRTVal.h"

#include <QtGui/QSpinBox.h>
#include <limits.h>

IntViewItem::IntViewItem(
  QString const &name,
  QVariant const &value
  )
  : BaseViewItem(name)
{
  m_spinner = new QSpinBox;
  m_spinner->setMinimum( INT_MIN );
  m_spinner->setMaximum( INT_MAX );
  m_spinner->setKeyboardTracking( false );
  connect(
    m_spinner, SIGNAL( valueChanged( int ) ), 
    this, SLOT( OnSpinnerChanged( int ) )
    );
  connect(
    m_spinner, SIGNAL( editingFinished() ), 
    this, SLOT( OnEditFinished() )
    );
  onModelValueChanged( value );
}

IntViewItem::~IntViewItem()
{
}

QWidget *IntViewItem::getWidget()
{
  return m_spinner;
}

void IntViewItem::onModelValueChanged( QVariant const &v )
{
  m_spinner->setValue( v.value<int>() );
}

void IntViewItem::OnSpinnerChanged( int value )
{
  emit viewValueChanged(
    QVariant::fromValue<int>( value ),
    0
    );
}

void IntViewItem::OnEditFinished()
{
  emit viewValueChanged(
    QVariant::fromValue<int>( m_spinner->value() ),
    1
    );
}

//////////////////////////////////////////////////////////////////////////
// 
BaseViewItem* IntViewItem::CreateItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* /*metaData*/
  )
{
  if ( RTVariant::isType<int>(value)
    || RTVariant::isType<long long>(value)
    || RTVariant::isType<unsigned>(value)
    || RTVariant::isType<unsigned long long>(value) )
  {
    return new IntViewItem( name, value );
  }
  return 0;
}

const int IntViewItem::Priority = 3;
