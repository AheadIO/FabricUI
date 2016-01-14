//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "RTValViewItem.h"
#include "ViewItemFactory.h"
#include "QVariantRTVal.h"

#include <assert.h>
#include <FTL/JSONValue.h>
#include <QtGui/QLabel.h>

RTValViewItem::RTValViewItem( QString name, const FabricCore::RTVal& value )
  : BaseComplexViewItem( name )
  , m_val(value)
  , m_widget(new QLabel())
{
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
                      strcmp( typeName, "Quat" ) == 0);
  // Do not change state if editable (we inherit our
  // parents editable status)
  if (!m_isEditableType)
    m_metadata.setSInt32( "disabled", 1 );

  UpdateWidget();
}

RTValViewItem::~RTValViewItem()
{
}

QWidget *RTValViewItem::getWidget()
{
  return m_widget;
}

void RTValViewItem::onModelValueChanged( QVariant const &value )
{
  RTVariant::toRTVal( value, m_val );
  for (int i = 0; i < m_childNames.size(); i++)
  {
    const char* childName = m_childNames[i].data();
    FabricCore::RTVal childVal = m_val.maybeGetMemberRef( childName );
    assert( childVal.isValid() );

    routeModelValueChanged( i, toVariant( childVal ) );
  }

  UpdateWidget();
}

void RTValViewItem::onChildViewValueChanged(
  int index,
  QVariant const &value,
  bool commit
  )
{

  assert( index < m_childNames.size() );
  const char* childName = m_childNames[index].data();

  // We cannot simply create a new RTVal based on the QVariant type, as 
  // we have to set the type exactly the same as the original.  Get the
  // original child value to ensure the new value matches the internal type
  FabricCore::RTVal oldChildVal = m_val.maybeGetMemberRef( childName );
  RTVariant::toRTVal( value, oldChildVal );
  m_val.setMember( childName, oldChildVal );

  emit viewValueChanged( toVariant( m_val ), commit );
}

void RTValViewItem::doAppendChildViewItems( QList<BaseViewItem*>& items )
{
  if (!m_val.isValid())
    return;

  try {

    FabricCore::RTVal desc = m_val.getJSON();
    if (!desc.isValid())
      return;

    const char* cdesc = desc.getStringCString();
    if (strcmp( cdesc, "null" ) == 0)
      return;

    // parse cdesc, Build children from desc.
    FTL::JSONValue* value = FTL::JSONValue::Decode( cdesc );
    FTL::JSONObject* obj = value->cast<FTL::JSONObject>();

    // Construct a child for each instance
    ViewItemFactory* factory = ViewItemFactory::GetInstance();
    for (FTL::JSONObject::const_iterator itr = obj->begin(); itr != obj->end(); itr++)
    {
      const char* childName = itr->first.c_str();
      FabricCore::RTVal childVal = m_val.maybeGetMemberRef( childName );
      if (childVal.isValid())
      {
        BaseViewItem* childItem = factory->CreateViewItem( childName, toVariant( childVal ), &m_metadata );
        if (childItem != NULL)
        {
          size_t index = m_childNames.size();
          m_childNames.push_back( childName );
          connectChild( (int)index, childItem );

          items.push_back( childItem );
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
  if ( m_isEditableType )
  {
    FabricCore::RTVal desc = m_val.getDesc();
    str = m_val.getTypeNameCStr();
    str += ": ";
    str += desc.getStringCString();
  }
  else
    str.sprintf( "<%s>", m_val.getTypeNameCStr() );
  m_widget->setText( str );
}

//////////////////////////////////////////////////////////
//
BaseViewItem* RTValViewItem::CreateItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* /*metaData*/
  )
{
  if (value.type() != QVariant::UserType)
    return NULL;
  if (value.userType() != qMetaTypeId<FabricCore::RTVal>())
    return NULL;

  FabricCore::RTVal rtVal = value.value<FabricCore::RTVal>();
  if (rtVal.isValid() && (rtVal.isObject() || rtVal.isStruct()))
  {
    RTValViewItem* pViewItem = new RTValViewItem( QString( name ), rtVal );
    return pViewItem;
  }
  return NULL;
}

const int RTValViewItem::Priority = 1;
