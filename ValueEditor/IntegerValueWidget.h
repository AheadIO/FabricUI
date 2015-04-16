// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_ValueEditor_IntegerValueWidget__
#define __UI_ValueEditor_IntegerValueWidget__

#include <QtGui/QSlider>
#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>

#include "ValueWidget.h"

namespace FabricUI
{

  namespace ValueEditor
  {
    class IntegerValueWidget : public ValueWidget
    {
      Q_OBJECT

    public:

      IntegerValueWidget(QString label, QWidget * parent);
      virtual ~IntegerValueWidget();

      int getMinimum() const;
      int getMaximum() const;
      void setRange(int minimum, int maximum);
      virtual void setValue(FabricCore::RTVal v);
      virtual void setEnabled(bool state);

      static TreeView::TreeEditorWidget * creator(QWidget * parent, TreeView::WidgetTreeItem * item);
      static bool canDisplay(TreeView::WidgetTreeItem * item);

    public slots:

      void onValueChangedInLineEdit();
      void onValueChangedInSlider();
      void onValueChangedInComboBox(int index);
      void onBeginInteraction();
      void onEndInteraction();

    private:

      QLineEdit * m_lineEdit;
      QSlider * m_slider;
      QComboBox * m_comboBox;
      std::string m_typeName;
      bool m_changingValue;
    };

  };

};

#endif // __UI_ValueEditor_IntegerValueWidget__
