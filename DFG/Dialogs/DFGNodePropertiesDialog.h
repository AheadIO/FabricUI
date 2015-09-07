// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGNodePropertiesDialog__
#define __UI_DFG_DFGNodePropertiesDialog__

#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QCheckBox>
#include <QtGui/QShowEvent>

#include "DFGBaseDialog.h"
#include "DFGRegisteredTypeLineEdit.h"
#include "DFGExtensionLineEdit.h"
#include <FabricUI/ValueEditor/ColorValueWidget.h>

namespace FabricUI
{
  namespace DFG
  {

    class DFGNodePropertiesDialog : public DFGBaseDialog
    {
      Q_OBJECT

    public:

      DFGNodePropertiesDialog(QWidget * parent, DFGController * controller, const char * nodeName, const DFGConfig & dfgConfig = DFGConfig());
      virtual ~DFGNodePropertiesDialog();

      QString title() const { return m_titleEdit->text(); }
      void setTitle(QString value);
      QString toolTip() const { return m_toolTipEdit->toPlainText(); }
      void setToolTip(QString value);
      QString docUrl() const { return m_docUrlEdit->text(); }
      void setDocUrl(QString value);

      virtual void showEvent(QShowEvent * event);

      QString getTitle();
      QString getToolTip();
      QString getDocUrl();
      QColor getNodeColor();
      QColor getHeaderColor();
      QColor getTextColor();

    private:

      void setColorFromExec(ValueEditor::ColorPickerWidget * widget, const char * json, QColor defaultCol);

      std::string                      m_nodeName;
      DFGController                   *m_controller;

      QLineEdit                       *m_titleEdit;
      QPlainTextEdit                  *m_toolTipEdit;
      QLineEdit                       *m_docUrlEdit;
      ValueEditor::ColorPickerWidget  *m_nodeColor;
      ValueEditor::ColorPickerWidget  *m_headerColor;
      ValueEditor::ColorPickerWidget  *m_textColor;
    };

  };

};

#endif // __UI_DFG_DFGNodePropertiesDialog__
