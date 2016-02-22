// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef _SHDFGCOMBINEDWIDGET_H_
#define _SHDFGCOMBINEDWIDGET_H_
 
#include <FabricUI/DFG/DFGCombinedWidget.h>
#include <FabricUI/ValueEditor/VEEditorOwner.h>
#include <FabricUI/SceneHub/TreeView/SHTreeView.h>

using namespace FabricServices;
 
namespace FabricUI
{
  namespace DFG
  {
    class SHDFGCombinedWidget : public FabricUI::DFG::DFGCombinedWidget
    {

      Q_OBJECT
      
      public:
        SHDFGCombinedWidget(QWidget * parent);

        ~SHDFGCombinedWidget() {};
 
      public slots:
        virtual void onUndo() {};
        virtual void onRedo() {};
        virtual void onHotkeyPressed(Qt::Key key, Qt::KeyboardModifier modifier, QString str) {
          DFGCombinedWidget::onHotkeyPressed(key, modifier, str);
        }
        virtual void onGraphSet(FabricUI::GraphView::Graph * graph) { 
          DFGCombinedWidget::onGraphSet(graph); 
        };
        virtual void onNodeInspectRequested(FabricUI::GraphView::Node * node) { 
          DFGCombinedWidget::onNodeInspectRequested(node); 
        };
        virtual void onNodeEditRequested(FabricUI::GraphView::Node * node) { 
          DFGCombinedWidget::onNodeEditRequested(node); 
        };
        virtual void onAdditionalMenuActionsRequested(QString name, QMenu * menu, bool prefix) { 
          DFGCombinedWidget::onAdditionalMenuActionsRequested(name, menu, prefix); 
        }; 

      private slots:
        void onPortEditDialogCreated(FabricUI::DFG::DFGBaseDialog * dialog) {
          DFGCombinedWidget::onPortEditDialogCreated(dialog); 
        }
        void onPortEditDialogInvoked(FabricUI::DFG::DFGBaseDialog * dialog, FTL::JSONObjectEnc<> * additionalMetaData) {
          DFGCombinedWidget::onPortEditDialogInvoked(dialog, additionalMetaData); 
        }
      
      private :
        /// Initializes the treeView widget.
        virtual void initTreeView();
        /// Initializes the windows docks.
        virtual void initDocks();

        SceneHub::SHTreeView *m_shTreeView;
    };

  }

}

#endif 
