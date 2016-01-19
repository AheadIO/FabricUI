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

      virtual QString GetName();
      virtual bool canRenameItem();
      virtual void RenameItem( const char* newName );
      virtual void OnItemRenamed( QString newName );

      virtual ItemMetadata* GetMetadata();
      virtual void SetMetadataImp( const char* key,
                                const char* value,
                                bool canUndo )/*override*/;


      virtual int GetInOut();

      virtual QVariant GetValue();

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
