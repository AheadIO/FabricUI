//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_MODELITEMS_BINDINGMODELITEM_H
#define FABRICUI_MODELITEMS_BINDINGMODELITEM_H

#include <FabricUI/ModelItems/RootModelItem.h>

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

      virtual bool isBinding() const /*override*/ { return true; }

      FabricUI::ValueEditor::BaseModelItem *createChild( FTL::CStrRef name ) /*override*/;

      virtual int getNumChildren() /*override*/;

      virtual FTL::CStrRef getChildName( int i ) /*override*/;

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
                                bool canUndo )/*override*/;
      /////////////////////////////////////////////////////////////////////////
      // Other
      /////////////////////////////////////////////////////////////////////////

      void argRemoved( unsigned index, FTL::CStrRef name );

    protected:

      virtual void setValue(
        QVariant var,
        bool commit,
        QVariant valueAtInteractionBegin
        ) /*override*/;
    };
  }
}

#endif // FABRICUI_MODELITEMS_BINDINGMODELITEM_H
