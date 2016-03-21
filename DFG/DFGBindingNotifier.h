//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include <FabricCore.h>
#include <FTL/ArrayRef.h>
#include <FTL/StrRef.h>
#include <FabricUI/DFG/DFGNotifier.h>
#include <QtCore/QSharedPointer>

namespace FabricUI {
namespace DFG {

class DFGBindingNotifier : public DFGNotifier
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

  virtual ~DFGBindingNotifier();

signals:
  
  void topoDirty();
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

  void loadDiagInserted(
    unsigned diagIndex
    );
  void loadDiagRemoved(
    unsigned diagIndex
    );

private:

  DFGBindingNotifier( FabricCore::DFGBinding binding )
    : m_binding( binding )
  {
    m_binding.registerNotificationCallback( &Callback, this );
  }

  virtual void handle( FTL::CStrRef jsonStr ) /*override*/;

  FabricCore::DFGBinding m_binding;
};

} // namespace DFG
} // namespace FabricUI
