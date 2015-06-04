// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGKLEditorPortTableWidget__
#define __UI_DFG_DFGKLEditorPortTableWidget__

#include <QtGui/QWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QListWidget>
#include <QtGui/QKeyEvent>
#include <QtGui/QAction>

#include "DFGConfig.h"
#include "DFGController.h"

namespace FabricUI
{

  namespace DFG
  {

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

      DFGKLEditorPortTableWidget(QWidget * parent, DFGController * controller, const DFGConfig & config = DFGConfig());
      virtual ~DFGKLEditorPortTableWidget();

      void setExec(FabricCore::DFGExec exec);

      unsigned int nbPorts() const;
      PortInfo portInfo(unsigned int index) const;
      FabricCore::DFGPortType portType(unsigned int index) const;
      QString portName(unsigned int index) const;
      QString dataType(unsigned int index) const;
      QString extension(unsigned int index) const;

      void keyPressEvent(QKeyEvent * event);

    public slots:

      void onCellChanged(int row, int col);
      void onComboBoxChanged(int index);
      void onCustomContextMenuRequested(const QPoint & pos);
      void contextMenuTriggered(QAction * action);

    signals:

      void execPortsChanged();

    private:

      int addPort(FabricCore::DFGPortType portType, QString portName, QString dataType, QString extension = "");

      FabricCore::DFGExec m_exec;
      DFGController * m_controller;
      DFGConfig m_config;
      bool m_signalsEnabled;
    };

  };

};

#endif // __UI_DFG_DFGKLEditorPortTableWidget__
