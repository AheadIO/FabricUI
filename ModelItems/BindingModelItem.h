#pragma once

#include "NodeModelItem.h"

namespace FabricUI
{

  namespace ModelItems
  {
    //////////////////////////////////////////////////////////////////////////
    // Specialize Exec for special case exterior
    class BindingModelItem : public NodeModelItem
    {
    private:
      FabricCore::DFGBinding m_binding;

    public:
      BindingModelItem( FabricCore::DFGBinding binding );

      BaseModelItem* GetChild( QString childName );
      virtual void argInserted( int index, const char* name, const char* type );
    };
  }
}
