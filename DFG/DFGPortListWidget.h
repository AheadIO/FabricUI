// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGPortListWidget__
#define __UI_DFG_DFGPortListWidget__

#include <DFGWrapper/Host.h>
#include <DFGWrapper/Executable.h>

#include <QtGui/QWidget>
#include <QtGui/QListWidget>

#include "DFGConfig.h"
#include "DFGController.h"

namespace FabricUI
{

  namespace DFG
  {

    class DFGPortListWidget : public QWidget
    {
      Q_OBJECT

    public:

      DFGPortListWidget(QWidget * parent, DFGController * controller, const DFGConfig & config = DFGConfig());
      virtual ~DFGPortListWidget();

      void setExec(FabricServices::DFGWrapper::ExecutablePtr exec);
      QString selectedItem() const;

    private:

      FabricServices::DFGWrapper::ExecutablePtr m_exec;
      DFGController * m_controller;
      QListWidget * m_list;
      DFGConfig m_config;
    };

  };

};

#endif // __UI_DFG_DFGPortListWidget__
