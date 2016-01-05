// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_ValueEditor_ValueEditorWidget__
#define __UI_ValueEditor_ValueEditorWidget__

#include <QtGui/QLabel> 
#include <QtGui/QWidget>
#include <QtGui/QKeyEvent>

#include <FabricUI/TreeView/TreeViewWidget.h>
#include <FabricUI/TreeView/TreeModel.h>
#include <FabricUI/TreeView/TreeEditorFactory.h>

#include <FabricCore.h>

#include "ValueItem.h"
#include "EditorConfig.h"

namespace FabricUI
{

  namespace ValueEditor_Legacy
  {
    class ValueEditorWidget : public QWidget
    {
      Q_OBJECT

    public:

      ValueEditorWidget(
        FabricCore::Client client,
        const EditorConfig & config = EditorConfig()
        );
      virtual ~ValueEditorWidget();
      EditorConfig & config();

      TreeView::TreeViewWidget * treeView() { return m_treeView; }

      virtual ValueItem * addValue(
        FTL::StrRef path,
        FabricCore::RTVal value,
        FTL::StrRef label = FTL::StrRef(),
        bool enabled = true,
        bool parentToRoot = false
        );
      virtual ValueItem * addValue(
        FTL::StrRef path,
        ValueItem * item,
        bool enabled = true
        );
      virtual bool removeValue(
        FTL::StrRef path
        );
      virtual FabricCore::RTVal getValue(
        FTL::StrRef path
        );
      
      virtual void clear();

      virtual void keyPressEvent(QKeyEvent * event);

    signals:

      void valueItemDelta( ValueItem *valueItem );

      void valueItemInteractionEnter( ValueItem *valueItem );
      void valueItemInteractionDelta( ValueItem *valueItem );
      void valueItemInteractionLeave( ValueItem *valueItem );

    protected slots:

      void onValueItemDelta( ValueItem *valueItem );

    protected:
    
      EditorConfig m_config;
      FabricCore::Client m_client;
      TreeView::TreeViewWidget * m_treeView;
      TreeView::TreeModel * m_treeModel;
      TreeView::TreeEditorFactory * m_factory;

      bool m_updatingOutputs;
    };

  };

};

#endif // __UI_ValueEditor_ValueEditorWidget__
