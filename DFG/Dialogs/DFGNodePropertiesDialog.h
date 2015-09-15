// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGNodePropertiesDialog__
#define __UI_DFG_DFGNodePropertiesDialog__

#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QCheckBox>
#include <QtGui/QShowEvent>
#include <QtGui/QCheckBox>
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
      /// Constructor
      DFGNodePropertiesDialog(QWidget * parent, DFGController * controller, const char * nodeName, const DFGConfig & dfgConfig = DFGConfig());
      /// Destructor
      virtual ~DFGNodePropertiesDialog();

      /// Gets the node's title
      QString title() const { return m_titleEdit->text(); }
      /// Sets the node's title
      void setTitle(QString value);
      /// Gets the node's tool tip
      QString toolTip() const { return m_toolTipEdit->toPlainText(); }
      /// Sets the node's tool tip
      void setToolTip(QString value);
      /// Gets the node's documentation url 
      QString docUrl() const { return m_docUrlEdit->text(); }
      /// Sets the node's documentation url 
      void setDocUrl(QString value);

      /// Shows this dialog widgets
      virtual void showEvent(QShowEvent * event);

      // [Julien] 
      /// Allows only alpha-numeric text (here the title) only 
      void alphaNumicStringOnly();
      // [Julien] 
      /// Filter the QLineEdit text (here the title) with the setRegexFilter
      void setRegexFilter(QString regex);

      /// Gets the user selected node's title
      QString getTitle();
      /// Gets the user selected node's tool tip
      QString getToolTip();
      /// Gets the user selected node's url doc
      QString getDocUrl();
      /// Gets the user selected node's body color 
      QColor getNodeColor();
      /// Gets the user selected node's header color 
      QColor getHeaderColor();
      /// Gets the user selected node's text color 
      QColor getTextColor();

    public slots:
      // [Julien] FE-5246 : Creates the node header color property
      // Custom header colors can have contrast mistmatches with the body's color
      // Thus, the option is disable by default 
      /// Creates the node header color property
      void createHeaderColor();

    private:
      /// \internal
      /// Gets the color property header color metadata
      QColor getColorFromExec(const char * key, QColor defaultCol);
      /// \internal
      /// Sets the color property of the ColorPickerWidget widget
      void setColorFromExec(ValueEditor::ColorPickerWidget * widget, const char * json, QColor defaultCol);

      std::string                      m_nodeName;
      DFGController                   *m_controller;

      QColor                           m_nodeDefaultHeaderColor;
      QLineEdit                       *m_titleEdit;
      QPlainTextEdit                  *m_toolTipEdit;
      QLineEdit                       *m_docUrlEdit;
      QCheckBox                       *m_allowHeaderColor;
      ValueEditor::ColorPickerWidget  *m_nodeColor;
      ValueEditor::ColorPickerWidget  *m_headerColor;
      ValueEditor::ColorPickerWidget  *m_textColor;
    };

  };

};

#endif // __UI_DFG_DFGNodePropertiesDialog__
