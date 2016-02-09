//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "DFGErrorsWidget.h"

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
  , m_errorsWidget( new QTableWidget )
{
  QHeaderView *horizontalHeader = m_errorsWidget->horizontalHeader();
  horizontalHeader->setMovable( false );
  horizontalHeader->setClickable( false );
  horizontalHeader->setResizeMode( QHeaderView::ResizeToContents );
  horizontalHeader->setStretchLastSection( true );
  m_errorsWidget->setHorizontalHeaderLabels(
    QStringList() << "Location" << "Description"
    );
  m_errorsWidget->verticalHeader()->hide();
  m_errorsWidget->setColumnCount( 2 );
  m_errorsWidget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
  m_errorsWidget->setEditTriggers( QAbstractItemView::NoEditTriggers );

  QVBoxLayout *layout = new QVBoxLayout;
  layout->setSpacing( 0 );
  layout->setContentsMargins( 0, 0, 0, 0 );
  layout->addWidget( new QLabel( "Diagnostics" ) );
  layout->addWidget( m_errorsWidget );
  setLayout( layout );
  setVisible( false );
  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
}

DFGErrorsWidget::~DFGErrorsWidget()
{
}

void DFGErrorsWidget::onErrorsMayHaveChanged(
  FabricCore::DFGBinding binding
  )
{
  bool hasErrors = binding.hasRecursiveConnectedErrors();
  if ( hasErrors )
  {
    FabricCore::DFGExec rootExec = binding.getExec();
    FabricCore::String errorsJSON =
      rootExec.getErrors( true /* recursive */ );
    FTL::StrRef errorsJSONStr( errorsJSON.getCStr(), errorsJSON.getLength() );
    FTL::JSONStrWithLoc errorsJSONStrWithLoc( errorsJSONStr );
    m_errors = 
      FTL::JSONValue::Decode( errorsJSONStrWithLoc )->cast<FTL::JSONArray>();

    int rowCount = m_errors->size();
    m_errorsWidget->setRowCount( rowCount );
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
      m_errorsWidget->setItem( row, 0, new QTableWidgetItem( location ) );

      m_errorsWidget->setItem(
        row, 1, new QTableWidgetItem(
          QString::fromUtf8( desc.data(), desc.size() )
          )
        );
    }
  }
  setVisible( hasErrors );
}

} // namespace DFG
} // namespace FabricUI
