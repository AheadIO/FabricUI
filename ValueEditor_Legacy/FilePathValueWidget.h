// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_ValueEditor_FilePathValueWidget__
#define __UI_ValueEditor_FilePathValueWidget__

#include <QtGui/QPushButton>

#include "ValueWidget.h"
#include "LineEdit.h"

namespace FabricUI
{

  namespace ValueEditor_Legacy
  {
    class FilePathValueWidget : public ValueWidget
    {
      Q_OBJECT

    public:

      FilePathValueWidget(QString label, QWidget * parent);
      virtual ~FilePathValueWidget();

      virtual void setValue(FabricCore::RTVal v);
      virtual void setEnabled(bool state);

      static TreeView::TreeEditorWidget * creator(QWidget * parent, TreeView::WidgetTreeItem * item);
      static bool canDisplay(TreeView::WidgetTreeItem * item);

    public slots:

      void onValueChangedInLineEdit();
      void onBrowseClicked();
      virtual void setFocusToFirstInput() { m_lineEdit->setFocus(); }

    private:

      LineEdit * m_lineEdit;
      QPushButton * m_browseButton;
    };

  };

};

#endif // __UI_ValueEditor_FilePathValueWidget__
