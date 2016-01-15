#pragma once

#include "RootModelItem.h"

namespace FabricUI
{
namespace DFG {
class DFGUICmdHandler;
}

  namespace ModelItems
  {
    //////////////////////////////////////////////////////////////////////////
    // Specialize Exec for special case exterior
    class BindingModelItem : public RootModelItem
    {
    private:

      DFG::DFGUICmdHandler *m_dfgUICmdHandler;
      FabricCore::DFGBinding m_binding;

      FabricCore::DFGExec m_rootExec;

    public:

      BindingModelItem(
        DFG::DFGUICmdHandler *dfgUICmdHandler,
        FabricCore::DFGBinding binding
        );
      ~BindingModelItem();

      virtual bool matchesPath( 
        const std::string& execPath,
        const std::string& name ) /*override*/;

      BaseModelItem* CreateChild( QString name ) /*override*/;

      virtual size_t NumChildren() /*override*/;

      virtual QString ChildName( int i ) /*override*/;

      virtual QString GetName() /*override*/;

      virtual QVariant GetValue() /*override*/;

      virtual ItemMetadata* GetMetadata() /*override*/;

      virtual void onViewValueChanged( QVariant const&, bool ) /*override*/;

    };
  }
}
