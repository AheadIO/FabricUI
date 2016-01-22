//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include <FabricCore.h>
#include <FTL/ArrayRef.h>
#include <FTL/StrRef.h>
#include <QtCore/QObject.h>
#include <QtCore/QSharedPointer.h>

namespace FabricUI {
namespace ModelItems {

class DFGBindingNotifier : public QObject
{
  Q_OBJECT

public:

  static QSharedPointer<DFGBindingNotifier>
  Create( FabricCore::DFGBinding binding )
  {
    return QSharedPointer<DFGBindingNotifier>(
      new DFGBindingNotifier( binding )
      );
  }

  ~DFGBindingNotifier();

signals:
  
  void dirty();

  void argInserted(
    unsigned index,
    FTL::CStrRef name,
    FTL::CStrRef typeName
    );

  void argRenamed(
    unsigned index,
    FTL::CStrRef oldName,
    FTL::CStrRef newName
    );

  void argTypeChanged(
    unsigned index,
    FTL::CStrRef name,
    FTL::CStrRef newTypeName
    );

  void argValueChanged(
    unsigned index,
    FTL::CStrRef name
    );
  
  void argRemoved(
    unsigned index,
    FTL::CStrRef name
    );
  
  void argsReordered(
    FTL::ArrayRef<unsigned> newOrder
    );

  void varInserted(
    FTL::CStrRef varName,
    FTL::CStrRef varPath,
    FTL::CStrRef typeName,
    FTL::CStrRef extDep
    );

  void varRemoved(
    FTL::CStrRef varName,
    FTL::CStrRef varPath
    );

private:

  DFGBindingNotifier( FabricCore::DFGBinding binding );

  void bindingNotificationCallback( FTL::CStrRef jsonStr );

  static void BindingNotificationCallback(
    void * userData,
    char const *jsonCString,
    uint32_t jsonLength
    );

  FabricCore::DFGBinding m_binding;
};

} // namespace FabricUI
} // namespace ModelItems
