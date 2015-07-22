// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_ValueEditor_ValueEditorWidget__
#define __UI_ValueEditor_ValueEditorWidget__

#include <QtGui/QLabel> 
#include <QtGui/QWidget>

#include <FabricUI/TreeView/TreeViewWidget.h>
#include <FabricUI/TreeView/TreeModel.h>
#include <FabricUI/TreeView/TreeEditorFactory.h>

#include <FabricCore.h>

#include "ValueItem.h"
#include "EditorConfig.h"

namespace FabricUI
{

  namespace ValueEditor
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

      virtual ValueItem * addValue(QString path, FabricCore::RTVal value, QString label = "", bool enabled = true, bool parentToRoot = false);
      virtual ValueItem * addValue(QString path, ValueItem * item, bool enabled = true);
      virtual bool removeValue(QString path);
      virtual FabricCore::RTVal getValue(QString path);
      virtual void clear();

    signals:

      void valueItemDelta( ValueItem *valueItem );

      void valueItemInteractionEnter( ValueItem *valueItem );
      void valueItemInteractionDelta( ValueItem *valueItem );
      void valueItemInteractionLeave( ValueItem *valueItem );

    protected:
    
      EditorConfig m_config;
      FabricCore::Client m_client;
      TreeView::TreeViewWidget * m_treeView;
      TreeView::TreeModel * m_treeModel;
      TreeView::TreeEditorFactory * m_factory;

    };

  };

};

#endif // __UI_ValueEditor_ValueEditorWidget__
