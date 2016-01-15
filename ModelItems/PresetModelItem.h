#pragma once

#include "RootModelItem.h"

//////////////////////////////////////////////////////////////////////////
// The Root-level model item for 
// 

namespace FabricUI
{

  namespace ModelItems
  {
    class PresetModelItem : public RootModelItem
    {
    protected:

      FabricCore::DFGExec m_exec;
  
    public:
      PresetModelItem( FabricCore::DFGExec exec );
      ~PresetModelItem();

      virtual bool matchesPath( const std::string& execPath, 
                                const std::string& name ) /*override*/;

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
