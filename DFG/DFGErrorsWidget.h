//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include <FabricUI/DFG/DFGBindingNotifier.h>
#include <FabricCore.h>
#include <FTL/OwnedPtr.h>
#include <FTL/StrRef.h>
#include <QtGui/QIcon>
#include <QtGui/QWidget>

class QTableWidget;

namespace FTL {
class JSONArray;
} // namespace FTL

namespace FabricUI {
namespace DFG {

class DFGUICmdHandler;

class DFGErrorsWidget : public QWidget
{
  Q_OBJECT

public:

  DFGErrorsWidget(
    QWidget *parent = 0
    );
  ~DFGErrorsWidget();

  void focusNone();

  void focusBinding(
    DFGUICmdHandler *cmdHandler,
    QSharedPointer<DFGBindingNotifier> bindingNotifier,
    FabricCore::DFGBinding binding
    );

  void focusExec(
    DFGUICmdHandler *cmdHandler,
    QSharedPointer<DFGBindingNotifier> bindingNotifier,
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
  void onCustomContextMenuRequested( QPoint const &pos );
  void onDismissSelected();
  void onLoadDiagInserted( unsigned diagIndex );
  void onLoadDiagRemoved( unsigned diagIndex );

private slots:

  void visitRow( int row, int col );

private:

  DFGUICmdHandler *m_cmdHandler;
  QSharedPointer<DFGBindingNotifier> m_bindingNotifier;
  FabricCore::DFGBinding m_binding;
  std::string m_execPath;
  FabricCore::DFGExec m_exec;
  FTL::OwnedPtr<FTL::JSONArray> m_errors;
  QTableWidget *m_tableWidget;
  QIcon m_errorIcon;
  QIcon m_warningIcon;
};

} // namespace DFG
} // namespace FabricUI
