#pragma once

#include <FabricUI/ValueEditor/BaseModelItem.h>
#include <FabricUI/ValueEditor/QVariantRTVal.h>

#include <vector>

//////////////////////////////////////////////////////////////////////////
// The Root-level model item for 
// 

namespace FabricUI
{

  namespace ModelItems
  {
    typedef std::vector<BaseModelItem*> ChildVec;
    class RootModelItem : public BaseModelItem
    {
    protected:

      ChildVec m_children;

    public:
      RootModelItem( );
      ~RootModelItem();

      virtual BaseModelItem* CreateChild( QString name ) = 0;

      ChildVec::iterator GetChildItrBegin() { return m_children.begin(); }
      ChildVec::iterator GetChildItrEnd() { return m_children.end(); }

      virtual BaseModelItem* GetChild( QString childName ) /*override*/;
      virtual BaseModelItem* GetChild( int index ) /*override*/;

      virtual int ChildIndex( QString name ) /*override*/;
      
      virtual bool hasDefault() /*override*/;

      virtual void resetToDefault() /*override*/;

      //////////////////////////////////////////////////////////////////////////
      virtual void argInserted( int index, const char* name, const char* type );

      void argTypeChanged( int index, const char* name, const char* newType );

      void argRemoved( int index, const char* name );
    };
  }
}
