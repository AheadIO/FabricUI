// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_ValueEditor_Vec4ValueWidget__
#define __UI_ValueEditor_Vec4ValueWidget__

#include "ValueWidget.h"
#include "LineEdit.h"

namespace FabricUI
{

  namespace ValueEditor
  {
    class Vec4ValueWidget : public ValueWidget
    {
      Q_OBJECT

    public:

      Vec4ValueWidget(QString label, QWidget * parent);
      virtual ~Vec4ValueWidget();

      virtual void setValue(FabricCore::RTVal v);
      virtual void setEnabled(bool state);

      static TreeView::TreeEditorWidget * creator(QWidget * parent, TreeView::WidgetTreeItem * item);
      static bool canDisplay(TreeView::WidgetTreeItem * item);

    public slots:

      void onValueChangedInLineEdit();
      void onBeginInteraction();
      void onEndInteraction();
      virtual void setFocusToFirstInput() { m_lineEditX->setFocus(); }

    private:

      LineEdit * m_lineEditX;
      LineEdit * m_lineEditY;
      LineEdit * m_lineEditZ;
      LineEdit * m_lineEditT;
    };

  };

};

#endif // __UI_ValueEditor_Vec4ValueWidget__
