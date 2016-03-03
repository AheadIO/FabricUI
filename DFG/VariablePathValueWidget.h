// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_VariablePathValueWidget__
#define __UI_DFG_VariablePathValueWidget__

#include <QtGui/QLineEdit>

#include <FabricUI/ValueEditor_Legacy/ValueWidget.h>
#include "Dialogs/DFGVariablePathLineEdit.h"

namespace FabricUI
{

  namespace DFG
  {
    class VariablePathValueWidget : public ValueEditor_Legacy::ValueWidget
    {
      Q_OBJECT

    public:

      VariablePathValueWidget(QString label, QWidget * parent);
      virtual ~VariablePathValueWidget();

      virtual void setValue(FabricCore::RTVal v);
      virtual void setEnabled(bool state);

      static TreeView::TreeEditorWidget * creator(QWidget * parent, TreeView::WidgetTreeItem * item);
      static bool canDisplay(TreeView::WidgetTreeItem * item);
      virtual void setFocusToFirstInput() { m_lineEdit->setFocus(); }

    public slots:

      void onValueChangedInLineEdit();

    private:

      DFGVariablePathLineEdit * m_lineEdit;
      bool m_changingValue;
    };

  };

};

#endif // __UI_DFG_VariablePathValueWidget__
