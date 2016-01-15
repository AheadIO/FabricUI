#pragma once

#include "RootModelItem.h"

#include <FabricCore.h>
#include <FTL/StrRef.h>

//////////////////////////////////////////////////////////////////////////
// The Root-level model item for 
// 

namespace FabricUI
{
namespace DFG {
class DFGUICmdHandler;
}

  namespace ModelItems
  {
    class NodeModelItem : public RootModelItem
    {
    protected:

      DFG::DFGUICmdHandler *m_dfgUICmdHandler;
      FabricCore::DFGBinding m_binding;
      std::string m_execPath;
      FabricCore::DFGExec m_exec;
      std::string m_nodeName;

      QString m_name;
  
    public:
      
      NodeModelItem(
        DFG::DFGUICmdHandler *dfgUICmdHandler,
        FabricCore::DFGBinding binding,
        FTL::StrRef execPath,
        FabricCore::DFGExec exec,
        FTL::StrRef nodeName,
        QString name
        );
      ~NodeModelItem();

      virtual bool matchesPath( const std::string& execPath, const std::string& name ) override;

      size_t NumChildren();

      virtual QString ChildName( int i );

      virtual BaseModelItem* CreateChild( QString name );

      virtual QString GetName();

      virtual ItemMetadata* GetMetadata();

      virtual QVariant GetValue();

      virtual void onViewValueChanged(
        QVariant const& var,
        bool commit
        );


    };
  }
}
