//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include <FabricCore.h>
#include <FabricUI/DFG/DFGNotifier.h>
#include <FTL/StrRef.h>
#include <QtCore/QSharedPointer.h>

namespace FabricUI {
namespace DFG {

class DFGExecNotifier : public DFGNotifier
{
  Q_OBJECT

public:

  static QSharedPointer<DFGExecNotifier>
  Create( FabricCore::DFGExec exec )
  {
    return QSharedPointer<DFGExecNotifier>(
      new DFGExecNotifier( exec )
      );
  }

  ~DFGExecNotifier()
    { m_view.invalidate(); }

signals:

  // The executable itself
  
  void metadataChanged(
    FTL::CStrRef key,
    FTL::CStrRef value
    );

  void editWouldSplitFromPresetMayHaveChanged();

  // The executable's ports

  void portInserted(
    unsigned portIndex,
    FTL::CStrRef portName
    );

  void portRenamed(
    unsigned portIndex,
    FTL::CStrRef oldPortName,
    FTL::CStrRef newPortName
    );

  void portRemoved(
    unsigned portIndex,
    FTL::CStrRef portName
    );

  void portMetadataChanged(
    FTL::CStrRef portName,
    FTL::CStrRef key,
    FTL::CStrRef value
    );

  void portTypeSpecChanged(
    FTL::CStrRef portName,
    FTL::CStrRef newTypeSpec
    );

  void portResolvedTypeChanged(
    FTL::CStrRef portName,
    FTL::CStrRef newResolvedTypeName
    );

  // The executable's nodes

  void nodeInserted(
    FTL::CStrRef nodeName
    );

  void nodeRenamed(
    FTL::CStrRef oldNodeName,
    FTL::CStrRef newNodeName
    );

  void nodeRemoved(
    FTL::CStrRef nodeName
    );

  void nodeMetadataChanged(
    FTL::CStrRef nodeName,
    FTL::CStrRef key,
    FTL::CStrRef value
    );

  void instExecEditWouldSplitFromPresetMayHaveChanged(
    FTL::CStrRef instName
    );

  // The executable's nodes' ports

  void nodePortInserted(
    FTL::CStrRef nodeName,
    unsigned portIndex,
    FTL::CStrRef portName
    );

  void nodePortRenamed(
    FTL::CStrRef nodeName,
    unsigned portIndex,
    FTL::CStrRef oldPortName,
    FTL::CStrRef newPortName
    );

  void nodePortRemoved(
    FTL::CStrRef nodeName,
    unsigned portIndex,
    FTL::CStrRef portName
    );

  void nodePortMetadataChanged(
    FTL::CStrRef nodeName,
    FTL::CStrRef portName,
    FTL::CStrRef key,
    FTL::CStrRef value
    );

  void nodePortResolvedTypeChanged(
    FTL::CStrRef nodeName,
    FTL::CStrRef portName,
    FTL::CStrRef newResolvedTypeName
    );

  void nodePortDefaultValuesChanged(
    FTL::CStrRef nodeName,
    FTL::CStrRef portName
    );

  // Conections

  void portsConnected(
    FTL::CStrRef srcPortPath,
    FTL::CStrRef dstPortPath
    );

  void portsDisconnected(
    FTL::CStrRef srcPortPath,
    FTL::CStrRef dstPortPath
    );

private:

  DFGExecNotifier( FabricCore::DFGExec exec )
    : m_view( exec.createView( &Callback, this ) ) {}

  virtual void handle( FTL::CStrRef jsonStr ) /*override*/;

  FabricCore::DFGView m_view;
};

} // namespace DFG
} // namespace FabricUI
