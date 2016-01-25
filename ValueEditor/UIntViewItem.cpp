//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "UIntViewItem.h"
#include "QVariantRTVal.h"
#include "VEIntSpinBox.h"

#include <limits.h>

UIntViewItem::UIntViewItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metadata
  )
  : BaseViewItem( name, metadata )
{
  m_spinner = new VEIntSpinBox;
  m_spinner->setObjectName( "UIntItem" );
  m_spinner->setMinimum( 0 );
  m_spinner->setMaximum( INT_MAX );
  m_spinner->setKeyboardTracking( false );

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

  //connect(
  //  m_spinner, SIGNAL( valueChanged( int ) ), 
  //  this, SLOT( OnSpinnerChanged( int ) )
  //  );
  //connect(
  //  m_spinner, SIGNAL( editingFinished() ), 
  //  this, SLOT( OnEditFinished() )
  //  );
  onModelValueChanged( value );
}

UIntViewItem::~UIntViewItem()
{
}

QWidget *UIntViewItem::getWidget()
{
  return m_spinner;
}

void UIntViewItem::onModelValueChanged( QVariant const &v )
{
  m_spinner->setValue( int( v.value<unsigned>() ) );
}

void UIntViewItem::OnSpinnerChanged( int value )
{
  emit viewValueChanged(
    QVariant::fromValue<unsigned>( unsigned( value ) )
    );
}

void UIntViewItem::OnEditFinished()
{
  emit viewValueChanged(
    QVariant::fromValue<unsigned>( unsigned( m_spinner->value() ) )
    );
}

//////////////////////////////////////////////////////////////////////////
// 
BaseViewItem* UIntViewItem::CreateItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metadata
  )
{
  if ( RTVariant::isType<unsigned>(value)
    || RTVariant::isType<unsigned long long>(value) )
  {
    return new UIntViewItem( name, value, metadata );
  }
  return 0;
}

const int UIntViewItem::Priority = 3;
