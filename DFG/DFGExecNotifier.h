//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include <FabricCore.h>
#include <FabricUI/DFG/DFGNotifier.h>
#include <FTL/ArrayRef.h>
#include <FTL/StrRef.h>
#include <QtCore/QSharedPointer>
#if defined(FTL_PLATFORM_WINDOWS)
# include <unordered_map>
#else
# include <tr1/unordered_map>
#endif

namespace FTL {
class JSONObject;
}

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

  void titleChanged(
    FTL::CStrRef title
    );

  void editWouldSplitFromPresetMayHaveChanged();

  void funcCodeChanged(
    FTL::CStrRef code
    );

  void didAttachPreset(
    FTL::CStrRef presetFilePath
    );

  void willDetachPreset(
    FTL::CStrRef presetFilePath
    );

  void extDepsChanged(
    FTL::CStrRef extDeps
    );

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

  void portDefaultValuesChanged(
    FTL::CStrRef portName
    );

  void portsReordered(
    FTL::ArrayRef<unsigned> newOrder
    );

  void portTypeChanged(
    FTL::CStrRef portName,
    FTL::CStrRef newExecPortType
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

  void instExecTitleChanged(
    FTL::CStrRef instName,
    FTL::CStrRef execTitle
    );

  void instExecEditWouldSplitFromPresetMayHaveChanged(
    FTL::CStrRef instName
    );

  void instExecDidAttachPreset(
    FTL::CStrRef instName,
    FTL::CStrRef presetFilePath
    );

  void refVarPathChanged(
    FTL::CStrRef refName,
    FTL::CStrRef newVarPath
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

  void nodePortTypeChanged(
    FTL::CStrRef nodeName,
    FTL::CStrRef portName,
    FTL::CStrRef newNodePortType
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

  void nodePortsReordered(
    FTL::CStrRef nodeName,
    FTL::ArrayRef<unsigned> newOrder
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

  typedef void (DFGExecNotifier::*Handler)(
    FTL::JSONObject const *jsonObject
    );

  void handler_execEditWouldSplitFromPresetMayHaveChanged( FTL::JSONObject const *jsonObject );
  void handler_execMetadataChanged( FTL::JSONObject const *jsonObject );
  void handler_execPortDefaultValuesChanged( FTL::JSONObject const *jsonObject );
  void handler_execPortInserted( FTL::JSONObject const *jsonObject );
  void handler_execPortMetadataChanged( FTL::JSONObject const *jsonObject );
  void handler_execPortRemoved( FTL::JSONObject const *jsonObject );
  void handler_execPortRenamed( FTL::JSONObject const *jsonObject );
  void handler_execPortResolvedTypeChanged( FTL::JSONObject const *jsonObject );
  void handler_execPortsReordered( FTL::JSONObject const *jsonObject );
  void handler_execPortTypeSpecChanged( FTL::JSONObject const *jsonObject );
  void handler_execTitleChanged( FTL::JSONObject const *jsonObject );
  void handler_instExecEditWouldSplitFromPresetMayHaveChanged( FTL::JSONObject const *jsonObject );
  void handler_instExecTitleChanged( FTL::JSONObject const *jsonObject );
  void handler_nodeInserted( FTL::JSONObject const *jsonObject );
  void handler_nodeMetadataChanged( FTL::JSONObject const *jsonObject );
  void handler_nodePortDefaultValuesChanged( FTL::JSONObject const *jsonObject );
  void handler_nodePortInserted( FTL::JSONObject const *jsonObject );
  void handler_nodePortMetadataChanged( FTL::JSONObject const *jsonObject );
  void handler_nodePortRemoved( FTL::JSONObject const *jsonObject );
  void handler_nodePortRenamed( FTL::JSONObject const *jsonObject );
  void handler_nodePortResolvedTypeChanged( FTL::JSONObject const *jsonObject );
  void handler_nodePortsReordered( FTL::JSONObject const *jsonObject );
  void handler_nodeRemoved( FTL::JSONObject const *jsonObject );
  void handler_nodeRenamed( FTL::JSONObject const *jsonObject );
  void handler_portsConnected( FTL::JSONObject const *jsonObject );
  void handler_portsDisconnected( FTL::JSONObject const *jsonObject );
  void handler_refVarPathChanged( FTL::JSONObject const *jsonObject );
  void handler_funcCodeChanged( FTL::JSONObject const *jsonObject );
  void handler_nodePortTypeChanged( FTL::JSONObject const *jsonObject );
  void handler_execPortTypeChanged( FTL::JSONObject const *jsonObject );
  void handler_instExecDidAttachPreset( FTL::JSONObject const *jsonObject );
  void handler_execDidAttachPreset( FTL::JSONObject const *jsonObject );
  void handler_execWillDetachPreset( FTL::JSONObject const *jsonObject );
  void handler_extDepsChanged( FTL::JSONObject const *jsonObject );

  FabricCore::DFGView m_view;

#if defined(FTL_PLATFORM_WINDOWS)
  typedef std::unordered_map<
#else
  typedef std::tr1::unordered_map<
#endif
    FTL::StrRef,
    Handler,
    FTL::StrRef::Hash,
    FTL::StrRef::Equals
    > HandlerMap;

  static HandlerMap const &GetHandlerMap();
};

} // namespace DFG
} // namespace FabricUI
