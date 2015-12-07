// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGKLEditorPortTableWidget__
#define __UI_DFG_DFGKLEditorPortTableWidget__

#include <QtGui/QWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QListWidget>
#include <QtGui/QKeyEvent>
#include <QtGui/QAction>
#include <QtGui/QItemDelegate>
#include <QtGui/QLineEdit>

#include "DFGConfig.h"
#include "DFGController.h"

namespace FabricUI
{

  namespace DFG
  {
    enum DFGKLEditorPortTableColumn
    {
      DFGKLEditorPortTableColumn_PortType,
      DFGKLEditorPortTableColumn_Name,
      DFGKLEditorPortTableColumn_Type
    };
    
    /// Class to customize DFGKLEditorPortTableWidgetDelegate element
    /// \note http://doc.qt.io/qt-4.8/qitemdelegate.html
    /// \notehttp://stackoverflow.com/questions/22708623/qtablewidget-only-numbers-permitted
    class DFGKLEditorPortTableWidgetDelegate : public QItemDelegate
    {
      public:
        /// Default constructor
        /// Sets alpha-numerical string
        DFGKLEditorPortTableWidgetDelegate() {
          m_regexFilter = QString("^[a-zA-Z0-9]*$*");
        }

        /// Constructor
        /// Sets regex string
        /// \params regexFilter The regex filter
        DFGKLEditorPortTableWidgetDelegate(QString regexFilter) {
          m_regexFilter = regexFilter;
        }

        /// \internal
        /// Sets the QTableWidget target element with a specifid style and options
        /// It is used to force the DFGKLEditorPortTableWidget text elements to support alpha-numerical string only
        QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
        {
          QLineEdit *lineEdit = new QLineEdit(parent);
          if ( index.column() == DFGKLEditorPortTableColumn_Name )
          {
            // Set validator
            QRegExp regex(m_regexFilter);
            QValidator *validator = new QRegExpValidator(regex, 0);
            lineEdit->setValidator(validator);
          }
          return lineEdit;
        }

      private:
        /// \internal
        /// Regex filter definition
        QString m_regexFilter;
    };


    class DFGKLEditorPortTableWidget : public QTableWidget
    {
      Q_OBJECT

    public:

      struct PortInfo
      {
        FabricCore::DFGPortType portType;
        std::string portName;
        std::string dataType;
        std::string extension;
      };

      DFGKLEditorPortTableWidget(QWidget * parent, DFGController * controller, const DFGConfig & config = DFGConfig(), bool setAlphaNum = true);
      virtual ~DFGKLEditorPortTableWidget();

      unsigned int nbPorts() const;
      PortInfo portInfo(unsigned int index) const;
      FabricCore::DFGPortType portType(unsigned int index) const;
      QString portName(unsigned int index) const;
      QString dataType(unsigned int index) const;
      QString extension(unsigned int index) const;

      void keyPressEvent(QKeyEvent * event);

    public slots:

      void refresh();
      void onCellChanged(int row, int col);
      void onComboBoxChanged(int index);
      void onCustomContextMenuRequested(const QPoint & pos);
      void contextMenuTriggered(QAction * action);

    signals:

      void execPortsChanged();

    protected:

      FabricCore::DFGExec &getExec()
        { return m_controller->getExec(); }

    private:

      int currentRow_safe() const;
      int addPort(FabricCore::DFGPortType portType, QString portName, QString dataType, QString extension = "");

      FabricCore::DFGExec m_exec;
      DFGController * m_controller;
      DFGConfig m_config;
      bool m_signalsEnabled;
    };

  };

};

#endif // __UI_DFG_DFGKLEditorPortTableWidget__
