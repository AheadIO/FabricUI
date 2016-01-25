//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include <FabricUI/ValueEditor/BaseModelItem.h>
#include <FabricCore.h>
#include <FTL/StrRef.h>

//////////////////////////////////////////////////////////////////////////
// The Root-level model item for 
namespace FabricUI
{
namespace DFG {
class DFGUICmdHandler;
}

  namespace ModelItems
  {
    //////////////////////////////////////////////////////////////////////////
    // Specialization for accessing ports that are also args
    class ArgModelItem : public BaseModelItem
    {
    private:

      DFG::DFGUICmdHandler *m_dfgUICmdHandler;
      FabricCore::DFGBinding m_binding;
      std::string m_argName;

      FabricCore::DFGExec m_rootExec;
      ItemMetadata *m_metadata;

    public:

      ArgModelItem(
        DFG::DFGUICmdHandler *dfgUICmdHandler,
        FabricCore::DFGBinding binding,
        FTL::StrRef argName
        );

      FabricCore::DFGExec getRootExec()
        { return m_rootExec; }
      FTL::CStrRef getArgName()
        { return m_argName; }

      virtual FTL::CStrRef getName()/*override*/;
      virtual void rename( const char* name )/*override*/;

      virtual BaseModelItem *onExecPortRenamed(
        FTL::CStrRef execPath,
        FTL::CStrRef oldExecPortName,
        FTL::CStrRef newExecPortName
        ) /*override*/;

      virtual QVariant getValue()/*override*/;

      virtual ItemMetadata* getMetadata() /*override*/;

      virtual void setMetadataImp( const char* key, 
                                const char* value,
                                bool canUndo)/*override*/;

      virtual int getInOut() /*override*/;

      virtual bool hasDefault() /*override*/;

      virtual void resetToDefault() /*override*/;

    protected:

      virtual void setValue(
        QVariant var,
        bool commit,
        QVariant valueAtInteractionBegin
        ) /*override*/;
    };
  }
}
