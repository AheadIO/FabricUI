// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_ValueEditor_Vec4ValueWidget__
#define __UI_ValueEditor_Vec4ValueWidget__

#include <QtGui/QLineEdit>

#include "ValueWidget.h"

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

    private:

      QLineEdit * m_lineEditX;
      QLineEdit * m_lineEditY;
      QLineEdit * m_lineEditZ;
      QLineEdit * m_lineEditT;
    };

  };

};

#endif // __UI_ValueEditor_Vec4ValueWidget__
