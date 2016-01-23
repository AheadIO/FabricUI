//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include <FabricUI/ValueEditor/BaseModelItem.h>
#include <FabricUI/ValueEditor/QVariantRTVal.h>
#include <FTL/StrRef.h>
#include <vector>

//////////////////////////////////////////////////////////////////////////
// The Root-level model item for 
// 

namespace FabricUI
{

  namespace ModelItems
  {

    // Split last word after '.' and return it, remove from path
    // Given "Node.Node.Port", it will return "Port", 
    // and path will be "Node.Node".
    extern std::string SplitLast( std::string& path );

    // Split first word before '.' and return it, remove from path
    // Given "Node.Node.Port", it will return "Node", 
    // and path will be "Node.Port".
    extern std::string SplitFirst( std::string& path );

    typedef std::vector<BaseModelItem*> ChildVec;
    class RootModelItem : public BaseModelItem
    {
    protected:

      ChildVec m_children;
      
    public:

      RootModelItem( );
      ~RootModelItem();

      virtual bool isNode() const { return false; }
      virtual bool isBinding() const { return false; }

      virtual BaseModelItem* createChild( FTL::CStrRef name ) = 0;

      ChildVec::iterator GetChildItrBegin() { return m_children.begin(); }
      ChildVec::iterator GetChildItrEnd() { return m_children.end(); }

      virtual BaseModelItem *getChild(
        FTL::CStrRef childName,
        bool doCreate = true
        ) /*override*/;
      virtual BaseModelItem *getChild(
        int index,
        bool doCreate = true
        ) /*override*/;

      virtual int getChildIndex( FTL::CStrRef name ) /*override*/;
      
      virtual bool hasDefault() /*override*/;

      virtual void resetToDefault() /*override*/;

      //////////////////////////////////////////////////////////////////////////
      bool argInserted( int index, const char* name, const char* type );

      bool argTypeChanged( int index, const char* name, const char* newType );

      void argRemoved( int index, const char* name );

      BaseModelItem *onPortRenamed(
        unsigned index,
        FTL::CStrRef oldName,
        FTL::CStrRef newName
        );
    };
  }
}
