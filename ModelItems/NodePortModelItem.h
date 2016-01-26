//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

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

    class NodePortItemMetadata;

    //////////////////////////////////////////////////////////////////////////
    // Basic ModelItem for accessing ports
    class NodePortModelItem : public BaseModelItem
    {
    protected:

      DFG::DFGUICmdHandler *m_dfgUICmdHandler;
      FabricCore::DFGBinding m_binding;
      std::string m_execPath;
      FabricCore::DFGExec m_exec;
      std::string m_nodeName;
      std::string m_portName;
    
      std::string m_portPath;

      NodePortItemMetadata *m_metadata;

    public:

      NodePortModelItem(
        DFG::DFGUICmdHandler *dfgUICmdHandler,
        FabricCore::DFGBinding binding,
        FTL::StrRef execPath,
        FabricCore::DFGExec exec,
        FTL::StrRef nodeName,
        FTL::StrRef portName
        );
      ~NodePortModelItem();

      FabricCore::DFGExec getExec()
        { return m_exec; }
      FTL::CStrRef getNodeName()
        { return m_nodeName; }
      FTL::CStrRef getPortName()
        { return m_portName; }
      FTL::CStrRef getPortPath()
        { return m_portPath; }

      void onNodeRenamed(
        FTL::CStrRef oldNodeName,
        FTL::CStrRef newNodeName
        );

      /////////////////////////////////////////////////////////////////////////
      // Metadata
      /////////////////////////////////////////////////////////////////////////

      virtual ItemMetadata* getMetadata();
      virtual void setMetadataImp( const char* key,
                                const char* value,
                                bool canUndo )/*override*/;


      /////////////////////////////////////////////////////////////////////////
      // Value
      /////////////////////////////////////////////////////////////////////////

      virtual QVariant getValue();

    protected:

      void updatePortPath();

      virtual void setValue(
        QVariant var,
        bool commit,
        QVariant valueAtInteractionBegin
        ) /*override*/;


    };
  }
}
