#pragma once

#include "RootModelItem.h"

//////////////////////////////////////////////////////////////////////////
// The Root-level model item for 
// 

namespace FabricUI
{

  namespace ModelItems
  {
    class NodeModelItem : public RootModelItem
    {
    protected:

      FabricCore::DFGExec m_exec;
      std::string m_path;
  
    public:
      NodeModelItem( FabricCore::DFGExec exec, const char* path );
      ~NodeModelItem();

      size_t NumChildren();

      virtual QString ChildName( int i );

      virtual BaseModelItem* CreateChild( QString name );

      virtual QString GetName();

      virtual ItemMetadata* GetMetadata();

      virtual QVariant GetValue();

      virtual void onViewValueChanged(
        QVariant const& var,
        bool commit
        );
    };
  }
}
