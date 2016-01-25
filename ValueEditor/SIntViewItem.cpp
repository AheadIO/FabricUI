//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "SIntViewItem.h"
#include "QVariantRTVal.h"

#include <QtGui/QSpinBox>
#include <limits.h>
#include "VEIntSpinBox.h"

SIntViewItem::SIntViewItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metadata
  )
  : BaseViewItem( name, metadata )
{
  m_spinner = new VEIntSpinBox();
  m_spinner->setObjectName( "SIntItem" );
  onModelValueChanged( value );

  connect(
    m_spinner, SIGNAL( interactionBegin() ),
    this, SIGNAL( interactionBegin() )
    );
  connect(
    m_spinner, SIGNAL( valueChanged( int ) ),
    this, SLOT( OnSpinnerChanged( int ) )
    );
  connect(
    m_spinner, SIGNAL( interactionEnd( bool ) ),
    this, SIGNAL( interactionEnd( bool ) )
    );

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
  ItemMetadata* metadata
  )
{
  if ( RTVariant::isType<int>(value)
    || RTVariant::isType<long long>(value) )
  {
    return new SIntViewItem( name, value, metadata );
  }
  return 0;
}

const int SIntViewItem::Priority = 3;
