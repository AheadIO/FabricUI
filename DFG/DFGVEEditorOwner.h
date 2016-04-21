//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_DFG_DFGVEEDITOROWNER_H
#define FABRICUI_DFG_DFGVEEDITOROWNER_H

#include <FabricUI/ValueEditor/VEEditorOwner.h>
#include <FabricUI/DFG/DFGNotifier.h>

class BaseModelItem;
class VETreeWidget;

namespace FTL {
  class JSONArray;
}

namespace FabricUI {

  namespace DFG {
    class DFGController;
    class DFGWidget;
  }
  namespace GraphView {
    class Node;
    class Graph;
  }
  namespace ModelItems {
    class BindingModelItem;
    class NodeModelItem;
    class RootModelItem;
  }

  namespace DFG 
  { 

    class DFGVEEditorOwner : public ValueEditor::VEEditorOwner
    {
      Q_OBJECT

    public:

      DFGVEEditorOwner( DFGWidget * dfgWidget );
      ~DFGVEEditorOwner();

      virtual void initConnections();

    public slots :
      virtual void onOutputsChanged(); // Call after each evaluation

    protected slots:

      void onControllerBindingChanged(
        FabricCore::DFGBinding const &binding
        );

      virtual void onSidePanelInspectRequested();
      void onNodeInspectRequested(FabricUI::GraphView::Node *node);

      void onBindingArgValueChanged( unsigned index, FTL::CStrRef name );

      void onBindingArgInserted(
        unsigned index,
        FTL::CStrRef name,
        FTL::CStrRef type
        );

      void onBindingArgRenamed(
        unsigned argIndex,
        FTL::CStrRef oldArgName,
        FTL::CStrRef newArgName
        );

      void onBindingArgRemoved(
        unsigned index,
        FTL::CStrRef name
        );

      void onBindingArgTypeChanged(
        unsigned index,
        FTL::CStrRef name,
        FTL::CStrRef newType
        );

      void onBindingArgsReordered(
        FTL::ArrayRef<unsigned> newOrder
        );

      void onExecNodePortInserted(
        FTL::CStrRef nodeName,
        unsigned portIndex,
        FTL::CStrRef portName
        );

      void onExecNodePortRenamed(
        FTL::CStrRef nodeName,
        unsigned portIndex,
        FTL::CStrRef oldNodePortName,
        FTL::CStrRef newNodePortName
        );

      void onExecNodePortRemoved(
        FTL::CStrRef nodeName,
        unsigned portIndex,
        FTL::CStrRef portName
        );

      void onExecNodePortsReordered(
        FTL::CStrRef nodeName,
        FTL::ArrayRef<unsigned> newOrder
        );

      void onExecPortMetadataChanged(
        FTL::CStrRef portName,
        FTL::CStrRef key,
        FTL::CStrRef value
        );

      void onExecNodeRemoved(
        FTL::CStrRef nodeName
        );

      void onExecNodeRenamed(
        FTL::CStrRef oldNodeName,
        FTL::CStrRef newNodeName
        );

      void onExecPortsConnectedOrDisconnected(
        FTL::CStrRef srcPortPath,
        FTL::CStrRef dstPortPath
        );

      void onExecPortDefaultValuesChanged(
        FTL::CStrRef portName
        );

      void onExecNodePortDefaultValuesChanged(
        FTL::CStrRef nodeName,
        FTL::CStrRef portName
        );

      void onExecNodePortResolvedTypeChanged(
        FTL::CStrRef nodeName,
        FTL::CStrRef portName,
        FTL::CStrRef newResolvedTypeName
        );

      void onExecRefVarPathChanged(
        FTL::CStrRef refName,
        FTL::CStrRef newVarPath
        );

      virtual void onStructureChanged();
      virtual void onFrameChanged(int frame);

      virtual void onGraphSet(FabricUI::GraphView::Graph * graph);

      FabricUI::DFG::DFGWidget * getDfgWidget();
      FabricUI::DFG::DFGController * getDFGController();

    private:

      void setModelRoot(
        FabricUI::DFG::DFGController *dfgController,
        FabricUI::ModelItems::BindingModelItem *bindingModelItem
        );
      void setModelRoot(
        FabricCore::DFGExec exec,
        FTL::CStrRef nodeName,
        FabricUI::ModelItems::NodeModelItem *nodeModelItem
        );

      int m_timelinePortIndex;

      DFG::DFGWidget * m_dfgWidget;
      FabricUI::GraphView::Graph * m_setGraph;
      QSharedPointer<DFG::DFGNotifier> m_notifier;
      QSharedPointer<DFG::DFGNotifier> m_subNotifier;
    };
}
}

#endif // FABRICUI_DFG_DFGVEEDITOROWNER_H
