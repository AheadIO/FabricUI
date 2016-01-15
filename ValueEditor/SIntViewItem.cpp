//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "SIntViewItem.h"
#include "QVariantRTVal.h"

#include <QtGui/QSpinBox>
#include <limits.h>

SIntViewItem::SIntViewItem(
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

SIntViewItem::~SIntViewItem()
{
}

QWidget *SIntViewItem::getWidget()
{
  return m_spinner;
}

void SIntViewItem::onModelValueChanged( QVariant const &v )
{
  m_spinner->setValue( v.value<int>() );
}

void SIntViewItem::OnSpinnerChanged( int value )
{
  emit viewValueChanged(
    QVariant::fromValue<int>( value )
    );
}

void SIntViewItem::OnEditFinished()
{
  emit viewValueChanged(
    QVariant::fromValue<int>( m_spinner->value() )
    );
}

//////////////////////////////////////////////////////////////////////////
// 
BaseViewItem* SIntViewItem::CreateItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* /*metaData*/
  )
{
  if ( RTVariant::isType<int>(value)
    || RTVariant::isType<long long>(value) )
  {
    return new SIntViewItem( name, value );
  }
  return 0;
}

const int SIntViewItem::Priority = 3;
