#pragma once

#include <QtCore/QSharedPointer>
#include <QtGui/QWidget>
#include <FTL/ArrayRef.h>
#include <FTL/CStrRef.h>

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
    class RootModelItem;
  }

  namespace ValueEditor 
  { 

    class ValueEditorBridgeOwner
    {
    public:
      virtual void log(const char* txt) const =0;
      virtual FabricUI::DFG::DFGWidget * getDfgWidget()=0;
    };

    class VEEditorOwner : public QObject 
    {
      Q_OBJECT

    public:
      VEEditorOwner(ValueEditorBridgeOwner& owner);
      ~VEEditorOwner();

      QWidget* getWidget() const;

      void initConnections();

    protected slots:

      void onSidePanelInspectRequested();
      void onNodeInspectRequested(FabricUI::GraphView::Node *node);

      void onBindingArgValueChanged( unsigned index, FTL::CStrRef name );
      void onOutputsChanged(); // Call after each evalation

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

      void onExecNodePortRenamed(
        FTL::CStrRef nodeName,
        unsigned portIndex,
        FTL::CStrRef oldNodePortName,
        FTL::CStrRef newNodePortName
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

      void onStructureChanged();

      void onFrameChanged(int frame);

      void onGraphSet(FabricUI::GraphView::Graph * graph);

    private:
      FabricUI::DFG::DFGWidget * getDfgWidget();
      FabricUI::DFG::DFGController * getUIController();
      FabricUI::DFG::DFGController * getDFGController();

    signals:
      void replaceModelRoot( BaseModelItem* model);
      void modelItemInserted( BaseModelItem* parent, int index, const char* childName);
      void modelItemTypeChange( BaseModelItem* changingItem, const char* newType);
      void modelItemRemoved( BaseModelItem* removedItem);
      void modelItemChildrenReordered( BaseModelItem* parent, const QList<int>& newOrder );
      void modelItemRenamed( BaseModelItem* renamedItem );

    private:

      int m_timelinePortIndex;

      ValueEditorBridgeOwner& m_owner;
      VETreeWidget* m_dfgValueEditor;
      FabricUI::GraphView::Graph * m_setGraph;

      FabricUI::ModelItems::RootModelItem* m_modelRoot;
      QSharedPointer<QObject> m_notifier;
      QSharedPointer<QObject> m_subNotifier;
    };
}
}
