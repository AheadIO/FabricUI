// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_ValueEditor_BooleanValueWidget__
#define __UI_ValueEditor_BooleanValueWidget__

#include <QtGui/QCheckBox>

#include "ValueWidget.h"

namespace FabricUI
{

  namespace ValueEditor
  {
    class BooleanValueWidget : public ValueWidget
    {
      Q_OBJECT

    public:

      BooleanValueWidget(QString label, QWidget * parent);
      virtual ~BooleanValueWidget();

      virtual void setValue(FabricCore::RTVal v);
      virtual void setEnabled(bool state);

      static TreeView::TreeEditorWidget * creator(QWidget * parent, TreeView::WidgetTreeItem * item);
      static bool canDisplay(TreeView::WidgetTreeItem * item);

    public slots:

      void onValueChangedInCheckBox();
      virtual void setFocusToFirstInput() { m_checkBox->setFocus(); }

    private:

      QCheckBox * m_checkBox;
    };

  };

};

#endif // __UI_ValueEditor_BooleanValueWidget__
