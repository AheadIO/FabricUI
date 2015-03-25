// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_ValueEditor_ScalarValueWidget__
#define __UI_ValueEditor_ScalarValueWidget__

#include <QtGui/QSlider>
#include <QtGui/QLineEdit>

#include "ValueWidget.h"

namespace FabricUI
{

  namespace ValueEditor
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

    private:

      QLineEdit * m_lineEdit;
      QSlider * m_slider;
      float m_minimum;
      float m_maximum;
      std::string m_typeName;
      bool m_changingValue;
    };

  };

};

#endif // __UI_ValueEditor_ScalarValueWidget__
