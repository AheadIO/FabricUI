#pragma once

#include "NodeModelItem.h"

namespace FabricUI
{

  namespace ModelItems
  {
    //////////////////////////////////////////////////////////////////////////
    // Access graph variables
    class VarModelItem : public NodeModelItem
    {
    private:
      QString m_name;
      std::string m_cname;

    public:
      VarModelItem( FabricCore::DFGExec& exec, const char* name );
      ~VarModelItem();

      virtual size_t NumChildren() /*override*/;

      virtual BaseModelItem* GetChild( int childIndex ) /*override*/;

      virtual QString GetName() /*override*/;

      virtual int GetInOut() /*override*/;

      virtual QVariant GetValue() /*override*/;

      virtual bool hasDefault() /*override*/;

      virtual void resetToDefault() /*override*/;

      virtual void onViewValueChanged( QVariant const&, bool ) /*override*/;
    };
  }
}
