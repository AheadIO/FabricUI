//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include <FabricCore.h>
#include <FTL/OwnedPtr.h>
#include <QtGui/QWidget>

class QTableWidget;

namespace FTL {
class JSONArray;
} // namespace FTL

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

  FTL::OwnedPtr<FTL::JSONArray> m_errors;
  QTableWidget *m_errorsWidget;
};

} // namespace DFG
} // namespace FabricUI
