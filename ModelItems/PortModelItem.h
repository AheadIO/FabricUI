#pragma once

#include <FabricUI/ValueEditor/BaseModelItem.h>
#include <FabricUI/ValueEditor/QVariantRTVal.h>
#include <FTL/StrRef.h>

namespace FabricUI
{

namespace DFG {
class DFGUICmdHandler;
}

  namespace ModelItems
  {
    //////////////////////////////////////////////////////////////////////////
    // Basic ModelItem for accessing ports
    class PortModelItem : public BaseModelItem
    {
    protected:

      DFG::DFGUICmdHandler *m_dfgUICmdHandler;
      FabricCore::DFGBinding m_binding;
      std::string m_execPath;
      FabricCore::DFGExec m_exec;
      std::string m_portPath;

      QString m_name;

      ItemMetadata* m_metadata;

    public:
      PortModelItem(
        DFG::DFGUICmdHandler *dfgUICmdHandler,
        FabricCore::DFGBinding binding,
        FTL::StrRef execPath,
        FabricCore::DFGExec exec,
        FTL::StrRef nodeName,
        FTL::StrRef portName,
        QString name
        );
      ~PortModelItem();

      // Every port has exactly 1 child - its RTValue
      virtual size_t NumChildren();

      virtual BaseModelItem* GetChild( int i );

      virtual QString GetName();

      virtual ItemMetadata* GetMetadata();

      virtual int GetInOut();

      virtual QVariant GetValue();

      virtual bool hasDefault() /*override*/;

      virtual void resetToDefault() /*override*/;

    protected:

      virtual void onViewValueChangedImpl( QVariant const&, bool ) /*override*/;

    };
  }
}
