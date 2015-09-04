// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_ValueEditor_Vec3ValueWidget__
#define __UI_ValueEditor_Vec3ValueWidget__

#include <QtGui/QLineEdit>

#include "ValueWidget.h"

namespace FabricUI
{

  namespace ValueEditor
  {
    class Vec3ValueWidget : public ValueWidget
    {
      Q_OBJECT

    public:

      Vec3ValueWidget(QString label, QWidget * parent);
      virtual ~Vec3ValueWidget();

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

      QLineEdit * m_lineEditX;
      QLineEdit * m_lineEditY;
      QLineEdit * m_lineEditZ;
    };

  };

};

#endif // __UI_ValueEditor_Vec3ValueWidget__
