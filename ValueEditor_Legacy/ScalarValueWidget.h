// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_ValueEditor_ScalarValueWidget__
#define __UI_ValueEditor_ScalarValueWidget__

#include <QtGui/QSlider>

#include "ValueWidget.h"
#include "LineEdit.h"

namespace FabricUI
{

  namespace ValueEditor_Legacy
  {
    class ScalarValueWidget : public ValueWidget
    {
      Q_OBJECT

    public:

      ScalarValueWidget(QString label, QWidget * parent);
      virtual ~ScalarValueWidget();

      float getMinimum() const;
      float getMaximum() const;
      void setRange(float minimum, float maximum);
      virtual void setValue(FabricCore::RTVal v);
      virtual void setEnabled(bool state);

      static TreeView::TreeEditorWidget * creator(QWidget * parent, TreeView::WidgetTreeItem * item);
      static bool canDisplay(TreeView::WidgetTreeItem * item);

    public slots:

      void onValueChangedInLineEdit();
      void onValueChangedInSlider();
      void onBeginInteraction();
      void onEndInteraction();
      virtual void setFocusToFirstInput() { m_lineEdit->setFocus(); }

    private:

      LineEdit * m_lineEdit;
      QSlider * m_slider;
      float m_minimum;
      float m_maximum;
      std::string m_typeName;
      bool m_changingValue;
    };

  };

};

#endif // __UI_ValueEditor_ScalarValueWidget__
