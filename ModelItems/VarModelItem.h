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
    public:

      VarModelItem(
        DFG::DFGUICmdHandler *dfgUICmdHandler,
        FabricCore::DFGBinding binding,
        FTL::StrRef execPath,
        FabricCore::DFGExec exec,
        FTL::StrRef nodeName,
        QString name
        );
      ~VarModelItem();

      virtual size_t NumChildren() /*override*/;
      BaseModelItem* GetChild( int childIndex ) /*override*/;

      virtual int GetInOut() /*override*/;

      virtual QVariant GetValue() /*override*/;

      virtual bool hasDefault() /*override*/;

      virtual void resetToDefault() /*override*/;

    protected:

      virtual void SetValue(
        QVariant var,
        bool commit,
        QVariant valueAtInteractionBegin
        ) /*override*/;
    };
  }
}
