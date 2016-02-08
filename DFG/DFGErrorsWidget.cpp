//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "DFGErrorsWidget.h"

#include <QtGui/QPlainTextEdit>
#include <QtGui/QVBoxLayout>

class QPlainTextEdit;

namespace FabricUI {
namespace DFG {

DFGErrorsWidget::DFGErrorsWidget(
  QWidget *parent
  )
  : QWidget( parent )
  , m_errorsText( new QPlainTextEdit )
{
  QVBoxLayout *layout = new QVBoxLayout;
  layout->setSpacing( 0 );
  layout->setContentsMargins( 0, 0, 0, 0 );
  layout->addWidget( m_errorsText );
  setLayout( layout );
  setVisible( false );
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
    QString errors = QString::fromUtf8( errorsJSON.getCStr() );
    m_errorsText->setPlainText( errors );
  }
  setVisible( hasErrors );
}

} // namespace DFG
} // namespace FabricUI
