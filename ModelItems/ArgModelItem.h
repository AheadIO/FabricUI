#pragma once

#include "PortModelItem.h"

//////////////////////////////////////////////////////////////////////////
// The Root-level model item for 
namespace FabricUI
{

  namespace ModelItems
  {
    //////////////////////////////////////////////////////////////////////////
    // Specialization for accessing ports that are also args
    class ArgModelItem : public PortModelItem
    {
    private:
      FabricCore::DFGBinding m_binding;
      
    public:

      ArgModelItem( const FabricCore::DFGBinding& binding, QString portName );

      virtual QVariant GetValue();

      virtual void onViewValueChanged( QVariant const&, bool ) /*override*/;

      virtual ItemMetadata* GetMetadata() /*override*/;
    };
  }
}
