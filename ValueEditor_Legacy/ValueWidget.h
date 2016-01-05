// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_ValueEditor_ValueWidget__
#define __UI_ValueEditor_ValueWidget__

#include <QtGui/QLabel> 
#include <QtGui/QHBoxLayout> 

#include <FabricUI/TreeView/TreeEditorWidget.h>
#include <FabricUI/TreeView/WidgetTreeItem.h>

#include "ValueItem.h"

#include <FabricCore.h>

namespace FabricUI
{

  namespace ValueEditor_Legacy
  {
    class ValueEditorWidget;

    class ValueWidget : public TreeView::TreeEditorWidget
    {
      Q_OBJECT

    public:

      ValueWidget(QString label, QWidget * parent, bool requiresEditability = false);
      virtual ~ValueWidget();

      virtual FabricCore::RTVal value() const;
      virtual void setValue(FabricCore::RTVal v);
      virtual ValueItem * valueItem();
      virtual void setEnabled(bool state) {}

      ValueEditorWidget * mainEditorWidget();

      static TreeView::TreeEditorWidget * creator(QWidget * parent, TreeView::WidgetTreeItem * item);
      static bool canDisplay(TreeView::WidgetTreeItem * item);

    public slots:
      virtual void onValueItemDelta( ValueItem *valueItem );
      virtual void setFocusToFirstInput() {}

    signals:

      void beginInteraction(ValueItem * item);
      void endInteraction(ValueItem * item);

    protected:

      FabricCore::RTVal m_value;
      
    private:

      QLabel * m_label;
    };

  };

};

#endif // __UI_ValueEditor_ValueWidget__
