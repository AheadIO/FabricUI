//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include <FabricUI/ValueEditor/BaseModelItem.h>
#include <FabricUI/ValueEditor/QVariantRTVal.h>
#include <FTL/StrRef.h>

namespace FabricUI {

namespace DFG {
class DFGUICmdHandler;
} // namespace DFG

namespace ModelItems {

class VarPathItemMetadata;

//////////////////////////////////////////////////////////////////////////
// Basic ModelItem for accessing ports
class VarPathModelItem : public BaseModelItem
{
protected:

  DFG::DFGUICmdHandler *m_dfgUICmdHandler;
  FabricCore::DFGBinding m_binding;
  std::string m_execPath;
  FabricCore::DFGExec m_exec;
  std::string m_refName;

  VarPathItemMetadata *m_metadata;

public:

  VarPathModelItem(
    DFG::DFGUICmdHandler *dfgUICmdHandler,
    FabricCore::DFGBinding binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec exec,
    FTL::StrRef refName
    );
  ~VarPathModelItem();

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
  // Others
  /////////////////////////////////////////////////////////////////////////

  virtual QVariant getValue() /*override*/;

  virtual void setValue(
    QVariant value,
    bool commit,
    QVariant valueAtInteractionBegin
    ) /*override*/;

  virtual bool hasDefault() /*override*/;
  virtual void resetToDefault() /*override*/;

  virtual ItemMetadata* getMetadata();

protected:

  virtual void setMetadataImp(
    const char* key,
    const char* value,
    bool canUndo
    ) /*override*/;
};

} // namespace ModelItems
} // namespace FabricUI
