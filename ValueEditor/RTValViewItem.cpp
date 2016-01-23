//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "RTValViewItem.h"
#include "ViewItemFactory.h"
#include "QVariantRTVal.h"

#include <assert.h>
#include <QtGui/QLabel>

RTValViewItem::RTValViewItem( QString name, 
                              const FabricCore::RTVal& value,
                              ItemMetadata* metadata )
  : BaseComplexViewItem( name, metadata )
  , m_val(value)
  , m_widget(new QLabel())
{
  m_widget->setObjectName( "RTValItem" );


  // We cannot leave arbitrary classes open to
  // editing, as we don't know what effect this
  // will have.  Here we white-list the list of
  // items we know are editable
  const char* typeName = m_val.getTypeNameCStr();
  m_isEditableType = (strcmp( typeName, "Mat22" ) == 0 ||
                      strcmp( typeName, "Mat33" ) == 0 ||
                      strcmp( typeName, "Mat44" ) == 0 ||
                      strcmp( typeName, "Vec2" ) == 0 ||
                      strcmp( typeName, "Vec3" ) == 0 ||
                      strcmp( typeName, "Vec4" ) == 0 ||
                      strcmp( typeName, "Quat" ) == 0 ||
                      strcmp( typeName, "Xfo" ) == 0);
  // Do not change state if editable (we inherit our
  // parents editable status)
  if (!m_isEditableType)
    m_metadata.setSInt32( "disabled", 1 );

  UpdateWidget();
}

RTValViewItem::~RTValViewItem()
{
}

bool RTValViewItem::hasChildren() const
{
  return m_isEditableType && BaseComplexViewItem::hasChildren();
}

QWidget *RTValViewItem::getWidget()
{
  return m_widget;
}

void RTValViewItem::onModelValueChanged( QVariant const &value )
{
  RTVariant::toRTVal( value, m_val );

  if ( hasChildren() )
  {
    if ( m_val.isAggregate()
      && ( !m_val.isObject() || !m_val.isNullObject() ) )
    {
      unsigned childCount = m_val.getMemberCount();
      for ( unsigned i = 0; i < childCount; ++i )
      {
        FabricCore::RTVal childVal = m_val.getMemberRef( i );
        routeModelValueChanged( i, toVariant( childVal ) );
      }
    }
  }

  UpdateWidget();
}

void RTValViewItem::onChildViewValueChanged(
  int index,
  QVariant value
  )
{
  if ( m_val.isAggregate()
    && ( !m_val.isObject() || !m_val.isNullObject() ) )
  {
    // We cannot simply create a new RTVal based on the QVariant type, as 
    // we have to set the type exactly the same as the original.  Get the
    // original child value to ensure the new value matches the internal type
    FabricCore::RTVal oldChildVal = m_val.getMemberRef( index );
    RTVariant::toRTVal( value, oldChildVal );
    m_val.setMember( index, oldChildVal );
  }

  emit viewValueChanged( toVariant( m_val ) );
}

void RTValViewItem::doAppendChildViewItems( QList<BaseViewItem*>& items )
{
  if (!m_val.isValid())
    return;

  try
  {
    if ( m_val.isAggregate() )
    {
      if ( !m_val.isObject() || !m_val.isNullObject() )
      {
        // Construct a child for each instance
        ViewItemFactory* factory = ViewItemFactory::GetInstance();
        unsigned childCount = m_val.getMemberCount();
        for ( unsigned i = 0; i < childCount; ++i )
        {
          char const *childName = m_val.getMemberName( i );
          FabricCore::RTVal childVal = m_val.getMemberRef( i );
          BaseViewItem* childItem =
            factory->CreateViewItem(
              childName,
              toVariant( childVal ),
              &m_metadata
              );
          if (childItem != NULL)
          {
            connectChild( (int)i, childItem );
            items.push_back( childItem );
          }
        }
      }
    }
  }
  catch (FabricCore::Exception e)
  {
    const char* error = e.getDesc_cstr();
    printf( "%s", error );
  }
}

void RTValViewItem::UpdateWidget()
{
  QString str;
  str = m_val.getTypeNameCStr();
  //if ( m_val.isObject() && m_val.isNullObject() )
  //{
  //  str += ": null";
  //}
  m_widget->setText( str );
}

//////////////////////////////////////////////////////////
//
BaseViewItem* RTValViewItem::CreateItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metadata
  )
{
  if (value.type() != QVariant::UserType)
    return NULL;
  if (value.userType() != qMetaTypeId<FabricCore::RTVal>())
    return NULL;

  FabricCore::RTVal rtVal = value.value<FabricCore::RTVal>();
  if ( rtVal.isValid() )
  {
    RTValViewItem* pViewItem = new RTValViewItem( QString( name ), rtVal, metadata );
    return pViewItem;
  }
  return NULL;
}

const int RTValViewItem::Priority = 1;
