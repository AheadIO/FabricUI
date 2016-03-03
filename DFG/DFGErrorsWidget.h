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

class DFGController;

class DFGErrorsWidget : public QWidget
{
  Q_OBJECT

public:

  DFGErrorsWidget(
    DFGController *dfgController,
    QWidget *parent = 0
    );
  ~DFGErrorsWidget();

  void focusNone();
  void focusBinding();
  void focusExec();

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

  enum Focus
  {
    Focus_None,
    Focus_Binding,
    Focus_Exec
  };

  DFGController *m_dfgController;
  Focus m_focus;
  FTL::OwnedPtr<FTL::JSONArray> m_errors;
  QTableWidget *m_tableWidget;
  QIcon m_errorIcon;
  QIcon m_warningIcon;
};

} // namespace DFG
} // namespace FabricUI
