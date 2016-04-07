//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "ItemMetadata.h"
#include <FTL/JSONValue.h>

using namespace FabricUI::ValueEditor;

FTL::CStrRef const ItemMetadata::VEPortTypeKey = FTL_STR("vePortType");
FTL::CStrRef const ItemMetadata::VENotInspectableKey = FTL_STR("veNotInspectable");

ViewItemMetadata::ViewItemMetadata( ItemMetadata* parent )
  : m_json(new FTL::JSONObject())
  , m_parent(parent)
{

}

ViewItemMetadata::ViewItemMetadata()
  : m_json( new FTL::JSONObject() )
  , m_parent(NULL)
{

}

void ViewItemMetadata::setString( const char* key, const char* value )
{
  m_json->insert( key, new FTL::JSONString( value ) );
}

void ViewItemMetadata::setSInt32( const char* key, int value )
{
  m_json->insert( key, new FTL::JSONSInt32( value ) );
}

void ViewItemMetadata::setFloat64( const char* key, double value )
{
  m_json->insert( key, new FTL::JSONFloat64( value ) );
}

void ViewItemMetadata::setDictionary( const char* key, FTL::JSONObject* value )
{
  m_json->insert( key, value );
}

void ViewItemMetadata::setArray( const char* key, FTL::JSONArray* value )
{
  m_json->insert( key, value );
}

//////////////////////////////////////////////////////////////////////////

const char* ViewItemMetadata::getString( const char* key ) const
{
  FTL::CStrRef v;
  if (m_json->maybeGetString(key, v))
  {
    return v.c_str();
  }
  if (m_parent != NULL)
    return m_parent->getString( key );
  return NULL;
}

int ViewItemMetadata::getSInt32( const char* key ) const
{
  const FTL::JSONValue* v = m_json->maybeGet( key );
  if (v)
  {
    const FTL::JSONSInt32* asInt = v->cast<FTL::JSONSInt32>();
    if (asInt)
      return asInt->getValue();
  }
  return m_parent->getSInt32( key );
}

double ViewItemMetadata::getFloat64( const char* key ) const
{
  const FTL::JSONValue* v = m_json->maybeGet( key );
  if (v)
  {
    const FTL::JSONFloat64* asInt = v->cast<FTL::JSONFloat64>();
    if (asInt)
      return asInt->getValue();
  }
  return m_parent->getFloat64( key );
}

const FTL::JSONObject* ViewItemMetadata::getDict( const char* key ) const
{
  const FTL::JSONObject* v = m_json->maybeGetObject( key );
  if (v)
  {
    return v;
  }
  return m_parent->getDict( key );
}

const FTL::JSONArray* ViewItemMetadata::getArray( const char* key ) const
{
  const FTL::JSONArray* v = m_json->maybeGetArray( key );
  if (v)
  {
    return v;
  }
  return m_parent->getArray( key );
}

bool ViewItemMetadata::has( const char* key ) const
{
  if (m_json->has( key ))
    return true;
  if (m_parent != NULL)
    return m_parent->has( key );
  return false;
}
