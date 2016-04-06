//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include <FabricCore.h>
#include <FTL/CStrRef.h>
#include <FTL/ArrayRef.h>
#include <QtGui/QWidget>
#include <QtCore/QMetaType>
#include <QtCore/QSharedPointer>
#include <FabricUI/DFG/DFGNotifier.h>


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

  namespace ValueEditor 
  {     
    class BaseModelItem;
    class VETreeWidget;

    class ValueEditorBridgeOwner
    {
      public:
        // Fixes error : QObject::connect: Cannot queue arguments of type 'FTL::CStrRef'
        // (Make sure 'FTL::CStrRef' is registered using qRegisterMetaType().)     
        ValueEditorBridgeOwner() { qRegisterMetaType<FTL::CStrRef>("FTL::CStrRef"); }
        // This must be concrete so we can subclass in PySide
        virtual ~ValueEditorBridgeOwner() {}
        virtual void log(const char* txt) const {}
        virtual FabricUI::DFG::DFGWidget * getDfgWidget() { return 0; }
    };

    class VEEditorOwner : public QObject 
    {
      Q_OBJECT

    public:

      VEEditorOwner( ValueEditorBridgeOwner *owner );
      ~VEEditorOwner();

      QWidget* getWidget() const;

      void initConnections();

    protected slots:

      void onControllerBindingChanged(
        FabricCore::DFGBinding const &binding
        );

      void onSidePanelInspectRequested();
      void onNodeInspectRequested(FabricUI::GraphView::Node *node);

      void onBindingArgValueChanged( unsigned index, FTL::CStrRef name );
      void onOutputsChanged(); // Call after each evaluation

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

      void onStructureChanged();

      void onFrameChanged(int frame);

      void onGraphSet(FabricUI::GraphView::Graph * graph);

    private:
      FabricUI::DFG::DFGWidget * getDfgWidget();
      FabricUI::DFG::DFGController * getUIController();
      FabricUI::DFG::DFGController * getDFGController();

    signals:
      void replaceModelRoot( FabricUI::ValueEditor::BaseModelItem* model);
      void modelItemInserted( FabricUI::ValueEditor::BaseModelItem* parent, int index, const char* childName);
      void modelItemTypeChange( FabricUI::ValueEditor::BaseModelItem* changingItem, const char* newType);
      void modelItemRemoved( FabricUI::ValueEditor::BaseModelItem* removedItem);
      void modelItemChildrenReordered( FabricUI::ValueEditor::BaseModelItem* parent, const QList<int>& newOrder );
      void modelItemRenamed( FabricUI::ValueEditor::BaseModelItem* renamedItem );

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

      ValueEditorBridgeOwner *m_owner;
      VETreeWidget* m_dfgValueEditor;
      FabricUI::GraphView::Graph * m_setGraph;

      FabricUI::ModelItems::RootModelItem* m_modelRoot;
      QSharedPointer<DFG::DFGNotifier> m_notifier;
      QSharedPointer<DFG::DFGNotifier> m_subNotifier;
    };
  }
}
