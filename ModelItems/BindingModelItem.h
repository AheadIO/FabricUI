#pragma once

#include "RootModelItem.h"

namespace FabricUI
{

  namespace ModelItems
  {
    //////////////////////////////////////////////////////////////////////////
    // Specialize Exec for special case exterior
    class BindingModelItem : public RootModelItem
    {
    private:
      FabricCore::DFGBinding m_binding;

    public:
      BindingModelItem( FabricCore::DFGBinding binding );
      ~BindingModelItem();

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
