//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "DFGErrorsWidget.h"

#include <FTL/JSONDec.h>
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
  QStringList headers;
  headers.append( "Location" );
  headers.append( "Description" );
  m_errorsWidget->setHorizontalHeaderLabels( headers );
  QHeaderView *horizontalHeader = m_errorsWidget->horizontalHeader();
  horizontalHeader->setMovable( false );
  horizontalHeader->setClickable( false );
  horizontalHeader->setResizeMode( QHeaderView::ResizeToContents );
  m_errorsWidget->verticalHeader()->hide();
  m_errorsWidget->setColumnCount( 2 );
  m_errorsWidget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
  m_errorsWidget->setEditTriggers( QAbstractItemView::NoEditTriggers );

  QVBoxLayout *layout = new QVBoxLayout;
  layout->setSpacing( 0 );
  layout->setContentsMargins( 0, 0, 0, 0 );
  layout->addWidget( m_errorsWidget );
  setLayout( layout );
  setVisible( false );
  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
}

DFGErrorsWidget::~DFGErrorsWidget()
{
}

void DFGErrorsWidget::onErrorsMayHaveChanged(
  FabricCore::DFGExec const &rootExec
  )
{
  bool hasErrors = rootExec.hasRecursiveConnectedErrors();
  if ( hasErrors )
  {
    FabricCore::String errorsJSON =
      rootExec.getErrors( true /* recursive */ );
    FTL::StrRef errorsJSONStr( errorsJSON.getCStr(), errorsJSON.getLength() );
    FTL::JSONStrWithLoc errorsJSONStrWithLoc( errorsJSONStr );
    FTL::JSONArrayDec errorsJSONArrayDec( errorsJSONStrWithLoc );
    FTL::JSONEnt errorJSONEnt;
    m_errorsWidget->setRowCount( 0 );
    while ( errorsJSONArrayDec.getNext( errorJSONEnt ) )
    {
      std::string errorStr;
      errorJSONEnt.stringAppendTo( errorStr );
      QString errorQString =
        QString::fromUtf8( errorStr.data(), errorStr.size() );
      int row = m_errorsWidget->rowCount();
      m_errorsWidget->insertRow( row );
      QTableWidgetItem *tableWidgetItem =
        new QTableWidgetItem( errorQString );
      m_errorsWidget->setItem( row, 0, tableWidgetItem );
    }
  }
  setVisible( hasErrors );
}

} // namespace DFG
} // namespace FabricUI
