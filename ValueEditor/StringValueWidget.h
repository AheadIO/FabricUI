// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_ValueEditor_StringValueWidget__
#define __UI_ValueEditor_StringValueWidget__

#include <QtGui/QLineEdit>
#include <QtGui/QComboBox>

#include "ValueWidget.h"

namespace FabricUI
{

  namespace ValueEditor
  {
    class StringValueWidget : public ValueWidget
    {
      Q_OBJECT

    public:

      StringValueWidget(QString label, QWidget * parent);
      virtual ~StringValueWidget();

      virtual void setValue(FabricCore::RTVal v);
      virtual void setEnabled(bool state);

      static TreeView::TreeEditorWidget * creator(QWidget * parent, TreeView::WidgetTreeItem * item);
      static bool canDisplay(TreeView::WidgetTreeItem * item);

    public slots:

      void onValueChangedInLineEdit();
      void onValueChangedInComboBox(int index);
      virtual void setFocusToFirstInput();

    private:

      QLineEdit * m_lineEdit;
      QComboBox * m_comboBox;
      bool m_changingValue;
    };

  };

};

#endif // __UI_ValueEditor_StringValueWidget__
