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
      DFGNodePropertiesDialog(
        QWidget * parent, 
        DFGController * controller, 
        const char * nodeName, 
        const DFGConfig & dfgConfig = DFGConfig()
        );
      /// Destructor
      virtual ~DFGNodePropertiesDialog();

      bool isPreset() const
        { return m_isPreset; }

      /// Gets the node's tool tip
      QString toolTip() const
      {
        if ( m_isPreset )
        {
          QLabel *toolTipLabel =
            static_cast<QLabel *>( m_toolTipWidget );
          return toolTipLabel->text();
        }
        else
        {
          QPlainTextEdit *toolTipEdit =
            static_cast<QPlainTextEdit *>( m_toolTipWidget );
          return toolTipEdit->toPlainText();
        }
      }

      /// Sets the node's tool tip
      void setToolTip(QString value);

      /// Gets the node's documentation url 
      QString docUrl() const
      {
        if ( m_isPreset )
        {
          QLabel *docUrlLabel =
            static_cast<QLabel *>( m_docUrlWidget );
          return docUrlLabel->text();
        }
        else
        {
          QLineEdit *docUrlEdit =
            static_cast<QLineEdit *>( m_docUrlWidget );
          return docUrlEdit->text();
        }
      }

      /// Sets the node's documentation url 
      void setDocUrl(QString value);

      /// Shows this dialog widgets
      virtual void showEvent(QShowEvent * event);

      /// Gets the user selected node's script name
      QString getScriptName();

      /// Gets the user selected node's body color 
      QColor getNodeColor();

      /// Gets the user selected node's text color 
      QColor getTextColor();

      /// Gets the user selected node's header color 
      bool getHeaderColor(QColor &color);

      void updateNodeName( FTL::StrRef newNodeName )
        { m_nodeName = newNodeName; }

    protected:

      /// Sets the node's title
      void setTitle(QString value);

    protected slots:

      void onNodeColorPushButtonClicked();
      void onTextColorPushButtonClicked();
      void onAllowHeaderColorCheckBoxClicked();
      void onHeaderColorPushButtonClicked();

    private:

      /// \internal
      /// Gets the color property header color metadata
      QColor getColorFromExec( const char * key, QColor defaultCol );

      /// \internal
      /// Sets the color property of the ColorPickerWidget widget
      void setColorFromExec(
        ValueEditor::ColorPickerWidget * widget,
        const char * json,
        QColor defaultCol
        );

      std::string                      m_nodeName;
      DFGController                   *m_controller;
      bool                             m_isPreset;

      QColor                           m_nodeDefaultHeaderColor;

      QWidget                         *m_titleWidget;
      QLabel                          *m_nameLabel;
      QWidget                         *m_toolTipWidget;
      QWidget                         *m_docUrlWidget;
      QPushButton                     *m_nodeColorPushButton;
      QPushButton                     *m_textColorPushButton;
      QCheckBox                       *m_allowHeaderColorCheckBox;
      QPushButton                     *m_headerColorPushButton;
    };

  };

};

#endif // __UI_DFG_DFGNodePropertiesDialog__
