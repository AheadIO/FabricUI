//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include <QtGui/QWidget>
#include <FabricCore.h>

class QTableWidget;

namespace FabricUI {
namespace DFG {

class DFGErrorsWidget : public QWidget
{
  Q_OBJECT

public:

  DFGErrorsWidget(
    QWidget *parent = 0
    );
  ~DFGErrorsWidget();

public slots:
  
  void onErrorsMayHaveChanged(
    FabricCore::DFGExec const &rootExec
    );

private:

  QTableWidget *m_errorsWidget;
};

} // namespace DFG
} // namespace FabricUI
