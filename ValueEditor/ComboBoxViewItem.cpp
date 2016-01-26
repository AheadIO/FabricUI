//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "ComboBoxViewItem.h"
#include "QVariantRTVal.h"

#include <FTL/StrSplit.h>
#include <QtGui/QComboBox>
#include "QVariantRTVal.h"

ComboBoxViewItem::ComboBoxViewItem( QString const &name, ItemMetadata* metadata )
  : BaseViewItem(name, metadata)
  , m_comboBox(NULL)
{
  m_comboBox = new QComboBox;
  m_comboBox->setObjectName( "ComboBoxItem" );
  connect( m_comboBox, SIGNAL( currentIndexChanged( const QString& ) ),
           this, SLOT( formatChanged( const QString& ) ) );
  metadataChanged();
}

ComboBoxViewItem::~ComboBoxViewItem()
{
}

void ComboBoxViewItem::metadataChanged()
{
  const char* str = m_metadata.getString( "uiCombo" );
  if (str == NULL)
    return;

  std::string uiComboStr = str;
  if (uiComboStr.size() > 0)
  {
    if (uiComboStr[0] == '(')
      uiComboStr = uiComboStr.substr( 1 );
    if (uiComboStr[uiComboStr.size() - 1] == ')')
      uiComboStr = uiComboStr.substr( 0, uiComboStr.size() - 1 );

    QStringList parts = QString( uiComboStr.c_str() ).split( ',' );
    // Push options to UI combobox
    m_comboBox->clear();
    for (int i = 0; i < parts.size(); i++)
    {
      QString itemStr = parts[i].trimmed();
      if (itemStr.startsWith( "\"" ))
        itemStr.remove( 0, 1 );
      if (itemStr.endsWith( "\"" ) )
        itemStr.chop( 1 );

      m_comboBox->addItem( itemStr );
    }
  }
}

QWidget *ComboBoxViewItem::getWidget()
{
  return m_comboBox;
}

void ComboBoxViewItem::onModelValueChanged( QVariant const &v )
{
  m_comboBox->setCurrentIndex( v.value<int>() );
}

void ComboBoxViewItem::entrySelected(int index)
{
  emit viewValueChanged(
    QVariant::fromValue<int>( index )
    );
}

//////////////////////////////////////////////////////////////////////////
// 
BaseViewItem* ComboBoxViewItem::CreateItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metaData
  )
{
  if ( metaData != NULL &&
       metaData->has("uiCombo") &&
       RTVariant::canConvert( value, QVariant::Int ) )
  {
    return new ComboBoxViewItem( name, metaData );
  }
  return 0;
}

const int ComboBoxViewItem::Priority = 5;
