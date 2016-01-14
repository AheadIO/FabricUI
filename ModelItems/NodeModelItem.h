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
    class NodeModelItem : public BaseModelItem
    {
    protected:

      FabricCore::DFGExec m_exec;
      std::string m_path;
      ChildVec m_children;

    public:
      NodeModelItem( FabricCore::DFGExec exec, const char* path );
      ~NodeModelItem();

      size_t NumChildren();

      ChildVec::iterator GetChildItrBegin() { return m_children.begin(); }
      ChildVec::iterator GetChildItrEnd() { return m_children.end(); }

      virtual BaseModelItem* GetChild( QString childName );
      virtual BaseModelItem* GetChild( int index );

      virtual int ChildIndex( QString name );
      virtual QString ChildName( int i );

      virtual QString GetName();;

      virtual ItemMetadata* GetMetadata();

      virtual QVariant GetValue();;

      virtual void argInserted( int index, const char* name, const char* type );

      void argTypeChanged( int index, const char* name, const char* newType );

      void argRemoved( int index, const char* name );

      virtual void onViewValueChanged(
        QVariant const& var,
        bool commit
        );

      virtual bool hasDefault() /*override*/;

      virtual void resetToDefault() /*override*/;

    };
  }
}
