#pragma once

#include <QtGui\QWidget>
#include <FTL\CStrRef.h>

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

      void onValueChanged( int index, const char* name );
      void onOutputsChanged(); // Call after each evalation
      // Arg-specific updates for when arg only changes.
      void onArgInserted(int index, const char* name, const char* type);
      void onArgTypeChanged(int index, const char* name, const char* newType);
      void onArgRemoved(int index, const char* name);
      void onArgsReordered( const FTL::JSONArray* newOrder );

      void onExecPortRenamed( const char* oldName, const char* newName );

      void onNodeRemoved( FTL::CStrRef execPath,
                          FTL::CStrRef nodeName );

      void onNodeRenamed( FTL::CStrRef execPath,
                          FTL::CStrRef oldNodeName,
                          FTL::CStrRef newNodeName );
      void onPortsConnected( FTL::CStrRef srcPort,
                             FTL::CStrRef dstPort );
      void onPortsDisconnected( FTL::CStrRef srcPort,
                             FTL::CStrRef dstPort );
      //
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
    };
}
}
