//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "DFGErrorsWidget.h"
#include <FabricUI/Util/LoadPixmap.h>

#include <FTL/JSONValue.h>
#include <QtGui/QLabel>
#include <QtGui/QHeaderView>
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>

namespace FabricUI {
namespace DFG {

DFGErrorsWidget::DFGErrorsWidget(
  QWidget *parent
  )
  : QWidget( parent )
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
    m_tableWidget, SIGNAL(cellClicked(int, int)),
    this, SLOT(onCellClicked(int, int))
    );

  QVBoxLayout *layout = new QVBoxLayout;
  layout->setContentsMargins( 0, 0, 0, 0 );
  layout->setSpacing( 0 );
  layout->addWidget( m_tableWidget );
  setLayout( layout );

  setObjectName( "DFGErrorsWidget" );
  setVisible( false );
  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
}

DFGErrorsWidget::~DFGErrorsWidget()
{
}

void DFGErrorsWidget::focusBinding(
  FabricCore::DFGBinding binding
  )
{
  m_binding = binding;
  m_execPath.clear();
  m_exec = FabricCore::DFGExec();
  onErrorsMayHaveChanged();
}

void DFGErrorsWidget::focusExec(
  FabricCore::DFGBinding binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec exec
  )
{
  m_binding = binding;
  m_execPath = execPath;
  m_exec = exec;
  onErrorsMayHaveChanged();
}

void DFGErrorsWidget::onErrorsMayHaveChanged()
{
  bool bindingHasRecursiveConnectedErrors =
    m_binding.hasRecursiveConnectedErrors(); // updates errors by side effect
  if ( m_exec.isValid() )
  {
    FabricCore::String errorsJSON =
      m_exec.getErrors( true /* recursive */ );
    FTL::StrRef errorsJSONStr( errorsJSON.getCStr(), errorsJSON.getLength() );
    FTL::JSONStrWithLoc errorsJSONStrWithLoc( errorsJSONStr );
    m_errors = 
      FTL::JSONValue::Decode( errorsJSONStrWithLoc )->cast<FTL::JSONArray>();
  }
  else if ( bindingHasRecursiveConnectedErrors )
  {
    FabricCore::String errorsJSON =
      m_binding.getErrors( true /* recursive */ );
    FTL::StrRef errorsJSONStr( errorsJSON.getCStr(), errorsJSON.getLength() );
    FTL::JSONStrWithLoc errorsJSONStrWithLoc( errorsJSONStr );
    m_errors = 
      FTL::JSONValue::Decode( errorsJSONStrWithLoc )->cast<FTL::JSONArray>();
  }
  else m_errors = 0;

  m_tableWidget->clearSelection();
  if ( m_errors && !m_errors->empty() )
  {
    int rowCount = m_errors->size();
    m_tableWidget->setRowCount( rowCount );
    for ( int row = 0; row < rowCount; ++row )
    {
      FTL::JSONObject const *error = m_errors->getObject( row );

      FTL::CStrRef execPath = error->getString( FTL_STR("execPath") );
      FTL::CStrRef nodeName = error->getStringOrEmpty( FTL_STR("nodeName") );
      FTL::CStrRef desc = error->getString( FTL_STR("desc") );
      int32_t line = error->getSInt32Or( FTL_STR("line"), -1 );
      int32_t column = error->getSInt32Or( FTL_STR("column"), -1 );

      QString location;
      if ( !execPath.empty() )
        location += QString::fromUtf8( execPath.data(), execPath.size() );
      if ( !execPath.empty() && !nodeName.empty() )
        location += '.';
      if ( !nodeName.empty() )
        location += QString::fromUtf8( nodeName.data(), nodeName.size() );
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
      
      m_tableWidget->setItem(
        row, 1, new QTableWidgetItem(
          m_errorIcon,
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

void DFGErrorsWidget::onCellClicked( int row, int col )
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

} // namespace DFG
} // namespace FabricUI
