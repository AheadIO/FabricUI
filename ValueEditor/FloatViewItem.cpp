#include "stdafx.h"
#include "FloatViewItem.h"
#include "BaseViewItemCreator.h"

FloatViewItem::FloatViewItem(
  QString const &name,
  QVariant const &value
  )
  : BaseViewItem(name)
{
  m_spinner = new QDoubleSpinBox;
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
    QVariant::fromValue<double>( value ),
    0
    );
}

void FloatViewItem::OnEditFinished()
{
  emit viewValueChanged(
    QVariant::fromValue<double>( m_spinner->value() ),
    1
    );
}

//////////////////////////////////////////////////////////////////////////
// 
static FloatViewItem* CreateItem(
  QString const &name,
  QVariant const &value,
  FTL::JSONObject* /*metaData*/
  )
{
  if (RTVariant::isType<double>(value) || RTVariant::isType<float>(value))
  {
    return new FloatViewItem( name, value );
  }
  return 0;
}

EXPOSE_VIEW_ITEM( FloatViewItem, CreateItem, 3 );

// Include MOC'ed file here, in order
// to support PCH on windows.
#include "moc_FloatViewItem.cpp"