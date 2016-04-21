//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_VALUEEDITOR_ITEMMETADATA_H
#define FABRICUI_VALUEEDITOR_ITEMMETADATA_H

#include <FTL/CStrRef.h>

namespace FTL {
  class JSONObject;
  class JSONArray;
}

namespace FabricUI {
namespace ValueEditor {

/* 
 * This base class is supplied to ViewItems on creation
 * It can supply hints on how to create the appropriate
 * views for data
 */
class ItemMetadata
{
public:

  static FTL::CStrRef const VEPortTypeKey;
  static FTL::CStrRef const VENotInspectableKey;

  virtual ~ItemMetadata() {}

  // Access arbirtary metadata
  virtual const char* getString( const char* key ) const { return 0; }
  virtual int getSInt32( const char* key ) const { return 0; }
  virtual double getFloat64( const char* key )  const { return 0.0; }
  virtual const FTL::JSONObject* getDict( const char* key ) const { return 0; }
  virtual const FTL::JSONArray* getArray( const char* key ) const { return 0; }

  // Returns 'true' if this key is present in our dictionary
  virtual bool has( const char* key ) const { return false; }
};

/*
 * A ViewItem may choose to create children, and if
 * so it may want to modify or replace some metadata to
 * hint how the view should be created.  For example,
 * the ColorViewItem would set a "min" & "max" metadata
 * for its children to specify that they should be
 * between 0 and 1
*/
class ViewItemMetadata : public ItemMetadata
{
  // We store any set metadata here.
  FTL::JSONObject* m_json;
  // Keep a pointer to the parent
  // Any keys we don't match will
  // be passed on to the parent.
  ItemMetadata* m_parent;
public:

  ViewItemMetadata( );
  ViewItemMetadata( ItemMetadata* parent );

  void setParent( ItemMetadata* parent )
    { m_parent = parent; }

  // Store a new value in our local json store.
  void setString( const char* key, const char* value );
  void setSInt32( const char* key, int value );
  void setFloat64( const char* key, double value );
  void setDictionary( const char* key, FTL::JSONObject* value );
  void setArray( const char* key, FTL::JSONArray* value );

  // Retrieve the value either from our local store,
  // or if not present then from the parent.
  virtual const char* getString( const char* key ) const;
  virtual int getSInt32( const char* key ) const;
  virtual double getFloat64( const char* key ) const;
  virtual const FTL::JSONObject* getDict( const char* key ) const;
  virtual const FTL::JSONArray* getArray( const char* key ) const;

  virtual bool has( const char* key ) const;

};

} // namespace FabricUI 
} // namespace ValueEditor 

#endif // FABRICUI_VALUEEDITOR_ITEMMETADATA_H
