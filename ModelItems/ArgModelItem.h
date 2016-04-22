//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_MODELITEMS_ARGMODELITEM_H
#define FABRICUI_MODELITEMS_ARGMODELITEM_H

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
    class ArgModelItem : public FabricUI::ValueEditor::BaseModelItem
    {
    private:

      DFG::DFGUICmdHandler *m_dfgUICmdHandler;
      FabricCore::DFGBinding m_binding;
      std::string m_argName;

      FabricCore::DFGExec m_rootExec;
      FabricUI::ValueEditor::ItemMetadata *m_metadata;

    public:

      ArgModelItem(
        DFG::DFGUICmdHandler *dfgUICmdHandler,
        FabricCore::DFGBinding binding,
        FTL::StrRef argName
        );
      ~ArgModelItem();

      FabricCore::DFGExec getRootExec()
        { return m_rootExec; }
      FTL::CStrRef getArgName()
        { return m_argName; }

      /////////////////////////////////////////////////////////////////////////
      // Name
      /////////////////////////////////////////////////////////////////////////

      virtual FTL::CStrRef getName() /*override*/;

      virtual bool canRename() /*override*/;

      virtual void rename( FTL::CStrRef newName ) /*override*/;

      virtual void onRenamed(
        FTL::CStrRef oldName,
        FTL::CStrRef newName
        ) /*override*/;

      /////////////////////////////////////////////////////////////////////////
      // Value
      /////////////////////////////////////////////////////////////////////////
      virtual QVariant getValue() /*override*/;


      /////////////////////////////////////////////////////////////////////////
      // Metadata
      /////////////////////////////////////////////////////////////////////////

      virtual FabricUI::ValueEditor::ItemMetadata* getMetadata() /*override*/;

      virtual void setMetadataImp( const char* key, 
                                const char* value,
                                bool canUndo)/*override*/;

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

#endif // FABRICUI_MODELITEMS_ARGMODELITEM_H
