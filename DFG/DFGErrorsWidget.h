//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include <FabricCore.h>
#include <FTL/OwnedPtr.h>
#include <FTL/StrRef.h>
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

  void focusBinding( FabricCore::DFGBinding binding );

  void focusExec(
    FabricCore::DFGBinding binding,
    FTL::StrRef execPath,
    FabricCore::DFGExec exec
    );

  bool haveErrors();

signals:
  
  void execSelected(
    FTL::CStrRef execPath,
    int line,
    int column
    );
  
  void nodeSelected(
    FTL::CStrRef execPath,
    FTL::CStrRef nodeName,
    int line,
    int column
    );

public slots:
  
  void onErrorsMayHaveChanged();

private slots:

  void onCellClicked( int row, int col );

private:

  FabricCore::DFGBinding m_binding;
  std::string m_execPath;
  FabricCore::DFGExec m_exec;
  FTL::OwnedPtr<FTL::JSONArray> m_errors;
  QTableWidget *m_errorsWidget;
};

} // namespace DFG
} // namespace FabricUI
