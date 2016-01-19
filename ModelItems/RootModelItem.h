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

      virtual BaseModelItem* CreateChild( QString name ) = 0;

      virtual bool matchesPath( 
        FTL::StrRef execPath, 
        FTL::StrRef nodeName
        ) = 0;


      ChildVec::iterator GetChildItrBegin() { return m_children.begin(); }
      ChildVec::iterator GetChildItrEnd() { return m_children.end(); }

      virtual BaseModelItem* GetChild( QString childName, bool doCreate = true ) /*override*/;
      virtual BaseModelItem* GetChild( int index, bool doCreate = true ) /*override*/;

      virtual int ChildIndex( QString name ) /*override*/;
      
      virtual bool hasDefault() /*override*/;

      virtual void resetToDefault() /*override*/;

      //////////////////////////////////////////////////////////////////////////
      bool argInserted( int index, const char* name, const char* type );

      bool argTypeChanged( int index, const char* name, const char* newType );

      void argRemoved( int index, const char* name );

      BaseModelItem* argRenamed( const char* oldName, const char* newName );
    };
  }
}
