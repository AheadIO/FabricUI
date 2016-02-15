//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGErrorsWidget.h>
#include <FabricUI/DFG/DFGUICmdHandler.h>
#include <FabricUI/Util/LoadPixmap.h>

#include <FTL/JSONValue.h>
#include <QtGui/QLabel>
#include <QtGui/QHeaderView>
#include <QtGui/QMenu>
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>

namespace FabricUI {
namespace DFG {

DFGErrorsWidget::DFGErrorsWidget(
  QWidget *parent
  )
  : QWidget( parent )
  , m_cmdHandler( 0 )
  , m_tableWidget( new QTableWidget )
  , m_errorIcon( QIcon( LoadPixmap( "DFGError.png" ) ) )
  , m_warningIcon( QIcon( LoadPixmap( "DFGWarning.png" ) ) )
{
  m_tableWidget->setColumnCount( 2 );
  QHeaderView *horizontalHeader = m_tableWidget->horizontalHeader();
  horizontalHeader->setMovable( false );
  horizontalHeader->setClickable( false );
  horizontalHeader->setResizeMode( QHeaderView::ResizeToContents );
  horizontalHeader->setStretchLastSection( true );
  m_tableWidget->setHorizontalHeaderLabels(
    QStringList() << "Location" << "Description"
    );
  m_tableWidget->verticalHeader()->hide();
  m_tableWidget->setSizePolicy(
    QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding )
    );
  m_tableWidget->setEditTriggers( QAbstractItemView::NoEditTriggers );
  m_tableWidget->setSelectionBehavior( QAbstractItemView::SelectRows );
  m_tableWidget->setShowGrid( false );
  
  connect(
    m_tableWidget, SIGNAL(cellDoubleClicked(int, int)),
    this, SLOT(visitRow(int, int))
    );

  QVBoxLayout *layout = new QVBoxLayout;
  layout->setContentsMargins( 0, 0, 0, 0 );
  layout->setSpacing( 0 );
  layout->addWidget( m_tableWidget );
  setLayout( layout );

  setObjectName( "DFGErrorsWidget" );
  setVisible( false );
  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
 
  setContextMenuPolicy( Qt::CustomContextMenu );
  QObject::connect(
    this, SIGNAL(customContextMenuRequested(const QPoint &)),
    this, SLOT(onCustomContextMenuRequested(const QPoint &))
    );
}

DFGErrorsWidget::~DFGErrorsWidget()
{
}

void DFGErrorsWidget::focusNone()
{
  if ( m_bindingNotifier )
  {
    disconnect(
      m_bindingNotifier.data(), SIGNAL(loadDiagInserted(unsigned)),
      this, SLOT(onLoadDiagInserted(unsigned))
      );
    disconnect(
      m_bindingNotifier.data(), SIGNAL(loadDiagRemoved(unsigned)),
      this, SLOT(onLoadDiagRemoved(unsigned))
      );
  }

  m_cmdHandler = 0;
  m_bindingNotifier.clear();
  m_binding = FabricCore::DFGBinding();
  m_execPath.clear();
  m_exec = FabricCore::DFGExec();
}

void DFGErrorsWidget::focusBinding(
  DFGUICmdHandler *cmdHandler,
  QSharedPointer<DFGBindingNotifier> bindingNotifier,
  FabricCore::DFGBinding binding
  )
{
  if ( m_bindingNotifier )
  {
    disconnect(
      m_bindingNotifier.data(), SIGNAL(loadDiagInserted(unsigned)),
      this, SLOT(onLoadDiagInserted(unsigned))
      );
    disconnect(
      m_bindingNotifier.data(), SIGNAL(loadDiagRemoved(unsigned)),
      this, SLOT(onLoadDiagRemoved(unsigned))
      );
  }

  m_cmdHandler = cmdHandler;
  m_bindingNotifier = bindingNotifier;
  m_binding = binding;
  m_execPath.clear();
  m_exec = FabricCore::DFGExec();

  if ( m_bindingNotifier )
  {
    connect(
      m_bindingNotifier.data(), SIGNAL(loadDiagInserted(unsigned)),
      this, SLOT(onLoadDiagInserted(unsigned))
      );
    connect(
      m_bindingNotifier.data(), SIGNAL(loadDiagRemoved(unsigned)),
      this, SLOT(onLoadDiagRemoved(unsigned))
      );
  }

  onErrorsMayHaveChanged();
}

void DFGErrorsWidget::focusExec(
  DFGUICmdHandler *cmdHandler,
  QSharedPointer<DFGBindingNotifier> bindingNotifier,
  FabricCore::DFGBinding binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec exec
  )
{
  if ( m_bindingNotifier )
  {
    disconnect(
      m_bindingNotifier.data(), SIGNAL(loadDiagInserted(unsigned)),
      this, SLOT(onLoadDiagInserted(unsigned))
      );
    disconnect(
      m_bindingNotifier.data(), SIGNAL(loadDiagRemoved(unsigned)),
      this, SLOT(onLoadDiagRemoved(unsigned))
      );
  }

  m_cmdHandler = cmdHandler;
  m_bindingNotifier = bindingNotifier;
  m_binding = binding;
  m_execPath = execPath;
  m_exec = exec;

  if ( m_bindingNotifier )
  {
    connect(
      m_bindingNotifier.data(), SIGNAL(loadDiagInserted(unsigned)),
      this, SLOT(onLoadDiagInserted(unsigned))
      );
    connect(
      m_bindingNotifier.data(), SIGNAL(loadDiagRemoved(unsigned)),
      this, SLOT(onLoadDiagRemoved(unsigned))
      );
  }

  onErrorsMayHaveChanged();
}

void DFGErrorsWidget::onErrorsMayHaveChanged()
{
  bool bindingHasRecursiveConnectedErrors =
    m_binding.hasRecursiveConnectedErrors(); // updates errors by side effect
  int firstErrorRow = 0;
  if ( m_exec.isValid() )
  {
    FabricCore::String errorsJSON =
      m_exec.getErrors( true /* recursive */ );
    FTL::StrRef errorsJSONStr( errorsJSON.getCStr(), errorsJSON.getLength() );
    FTL::JSONStrWithLoc errorsJSONStrWithLoc( errorsJSONStr );
    m_errors = 
      FTL::JSONValue::Decode( errorsJSONStrWithLoc )->cast<FTL::JSONArray>();
  }
  else
  {
    FabricCore::String loadDiagsJSON = m_binding.getLoadDiags();
    FTL::StrRef loadDiagsJSONStr( loadDiagsJSON.getCStr(), loadDiagsJSON.getLength() );
    FTL::JSONStrWithLoc loadDiagsJSONStrWithLoc( loadDiagsJSONStr );
    m_errors = 
      FTL::JSONValue::Decode( loadDiagsJSONStrWithLoc )->cast<FTL::JSONArray>();
    firstErrorRow = m_errors->size();

    if ( bindingHasRecursiveConnectedErrors )
    {
      FabricCore::String errorsJSON =
        m_binding.getErrors( true /* recursive */ );
      FTL::StrRef errorsJSONStr( errorsJSON.getCStr(), errorsJSON.getLength() );
      FTL::JSONStrWithLoc errorsJSONStrWithLoc( errorsJSONStr );
      FTL::OwnedPtr<FTL::JSONArray> errors(
        FTL::JSONValue::Decode( errorsJSONStrWithLoc )->cast<FTL::JSONArray>()
        );
      m_errors->extend_take( errors );
    }
  }

  m_tableWidget->clearSelection();
  if ( m_errors && !m_errors->empty() )
  {
    int rowCount = m_errors->size();
    m_tableWidget->setRowCount( rowCount );
    for ( int row = 0; row < rowCount; ++row )
    {
      FTL::JSONObject const *error = m_errors->getObject( row );

      FTL::CStrRef execPath = error->getStringOrEmpty( FTL_STR("execPath") );
      FTL::CStrRef nodeName = error->getStringOrEmpty( FTL_STR("nodeName") );
      int32_t line = error->getSInt32Or( FTL_STR("line"), -1 );
      int32_t column = error->getSInt32Or( FTL_STR("column"), -1 );
      QString location;
      if ( !execPath.empty() )
        location += QString::fromUtf8( execPath.data(), execPath.size() );
      if ( !execPath.empty() && !nodeName.empty() )
        location += '.';
      if ( !nodeName.empty() )
        location += QString::fromUtf8( nodeName.data(), nodeName.size() );
      if ( location.isEmpty() )
        location += "<root>";
      if ( line != -1 )
      {
        location += ':';
        location += QString::number( line );
        if ( column != -1 )
        {
          location += ':';
          location += QString::number( column );
        }
      }
      m_tableWidget->setItem( row, 0, new QTableWidgetItem( location ) );

      FTL::CStrRef desc = error->getString( FTL_STR("desc") );
      m_tableWidget->setItem(
        row, 1, new QTableWidgetItem(
          row < firstErrorRow? m_warningIcon: m_errorIcon,
          QString::fromUtf8( desc.data(), desc.size() )
          )
        );
    }
    show();
  }
  else
  {
    m_tableWidget->setRowCount( 0 );
    hide();
  }
}

void DFGErrorsWidget::visitRow( int row, int col )
{
  FTL::JSONObject const *error = m_errors->getObject( row );

  FTL::CStrRef baseExecPath = error->getString( FTL_STR("execPath") );
  FTL::CStrRef nodeName = error->getStringOrEmpty( FTL_STR("nodeName") );
  int32_t line = error->getSInt32Or( FTL_STR("line"), -1 );
  int32_t column = error->getSInt32Or( FTL_STR("column"), -1 );

  std::string execPath;
  if ( !m_execPath.empty() )
    execPath += m_execPath;
  if ( !m_execPath.empty() && !baseExecPath.empty() )
    execPath += '.';
  if ( !baseExecPath.empty() )
    execPath += baseExecPath;

  if ( !nodeName.empty() )
    emit nodeSelected( execPath, nodeName, line, column );
  else
    emit execSelected( execPath, line, column );
}

bool DFGErrorsWidget::haveErrors()
{
  return m_errors && !m_errors->empty();
}

void DFGErrorsWidget::onDismissSelected()
{
  QList<int> diagIndices;
  QList<QTableWidgetSelectionRange> ranges = m_tableWidget->selectedRanges();
  for ( int i = 0; i < ranges.size(); ++i )
  {
    QTableWidgetSelectionRange const &range = ranges[i];
    for ( int row = range.topRow(); row <= range.bottomRow(); ++row )
    {
      FTL::JSONObject *rowJSONObject =
        m_errors->get( row )->cast<FTL::JSONObject>();
      int diagIndex = rowJSONObject->getSInt32Or( FTL_STR("diagIndex"), -1 );
      diagIndices.append( diagIndex );
    }
  }
  m_cmdHandler->dfgDoDismissLoadDiags(
    m_binding,
    diagIndices
    );
}

void DFGErrorsWidget::onCustomContextMenuRequested( QPoint const &pos )
{
  QMenu menu;

  QAction *dismissAction = new QAction( "Dismiss Selected", &menu );
  connect(
    dismissAction, SIGNAL(triggered()),
    this, SLOT(onDismissSelected())
    );
  menu.addAction( dismissAction );

  bool haveDiagIndex = false;
  QList<QTableWidgetSelectionRange> ranges = m_tableWidget->selectedRanges();
  for ( int i = 0; i < ranges.size(); ++i )
  {
    QTableWidgetSelectionRange const &range = ranges[i];
    for ( int row = range.topRow(); row <= range.bottomRow(); ++row )
    {
      FTL::JSONObject *rowJSONObject =
        m_errors->get( row )->cast<FTL::JSONObject>();
      int diagIndex = rowJSONObject->getSInt32Or( FTL_STR("diagIndex"), -1 );
      if ( diagIndex != -1 )
      {
        haveDiagIndex = true;
        break;
      }
    }
  }
  dismissAction->setEnabled( haveDiagIndex );

  menu.exec( mapToGlobal( pos ) );
}

void DFGErrorsWidget::onLoadDiagInserted( unsigned diagIndex )
{
  onErrorsMayHaveChanged();
}

void DFGErrorsWidget::onLoadDiagRemoved( unsigned diagIndex )
{
  onErrorsMayHaveChanged();
}

} // namespace DFG
} // namespace FabricUI
