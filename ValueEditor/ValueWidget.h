// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_ValueEditor_ValueWidget__
#define __UI_ValueEditor_ValueWidget__

#include <QtGui/QLabel> 
#include <QtGui/QHBoxLayout> 

#include <TreeView/TreeEditorWidget.h>
#include <TreeView/WidgetTreeItem.h>

#include "ValueItem.h"

#include <FabricCore.h>

namespace FabricUI
{

  namespace ValueEditor
  {
    class ValueWidget : public TreeView::TreeEditorWidget
    {
      Q_OBJECT

    public:

      ValueWidget(QString label, QWidget * parent, bool labelRightAligned = false);
      virtual ~ValueWidget();

      virtual FabricCore::RTVal value() const;
      virtual void setValue(FabricCore::RTVal v);
      virtual ValueItem * valueItem();
      virtual void setEnabled(bool state) {}

      static TreeView::TreeEditorWidget * creator(QWidget * parent, TreeView::WidgetTreeItem * item);
      static bool canDisplay(TreeView::WidgetTreeItem * item);

    signals:

      void beginInteraction(ValueItem * item);
      void valueChanged(ValueItem * item);
      void endInteraction(ValueItem * item);

    protected:

      FabricCore::RTVal m_value;
      
    private:

      QLabel * m_label;
    };

  };

};

#endif // __UI_ValueEditor_ValueWidget__
