//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "BaseModelItem.h"
#include "DefaultViewItem.h"
#include "ViewItemFactory.h"

#include <QtCore/QVariant>
#include <QtGui/QLabel>

DefaultViewItem::DefaultViewItem(
  QString const &name,
  QVariant const &value
  )
  : BaseViewItem( name )
{
  m_label = new QLabel;

  onModelValueChanged( value );
}

DefaultViewItem::~DefaultViewItem()
{
}

QWidget *DefaultViewItem::getWidget()
{
  return m_label;
}

void DefaultViewItem::onModelValueChanged( QVariant const &value )
{
  if ( value.canConvert( QVariant::String ) )
  {
    QString string = value.toString();
    m_label->setText( string );
  }
}

//////////////////////////////////////////////////////////////////////////
// Expose the ViewItem to the UI layer
BaseViewItem* DefaultViewItem::CreateItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* /*metaData*/
  )
{
  return new DefaultViewItem( name, value );
}

const int DefaultViewItem::Priority = 0;
