#include "stdafx.h"
#include "BaseModelItem.h"
#include "DefaultViewItem.h"
#include "BaseViewItemCreator.h"
#include "ViewItemFactory.h"

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
static BaseViewItem* CreateItem(
  QString const &name,
  QVariant const &value,
  FTL::JSONObject* /*metaData*/
  )
{
  return new DefaultViewItem( name, value );
}

EXPOSE_VIEW_ITEM(DefaultViewItem, CreateItem, 0);
