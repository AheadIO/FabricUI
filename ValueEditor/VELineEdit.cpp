//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "VELineEdit.h"

#include <QtGui/QFocusEvent>

void VELineEdit::focusInEvent( QFocusEvent *event )
{
  if ( event->reason() != Qt::PopupFocusReason )
    m_last = text();
  QLineEdit::focusInEvent( event );
}

void VELineEdit::focusOutEvent( QFocusEvent *event )
{
  if ( event->reason() != Qt::PopupFocusReason )
    checkText();
  QLineEdit::focusOutEvent( event );
}

void VELineEdit::keyPressEvent( QKeyEvent *event )
{
  if ( event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter )
  {
    // [FE-6025]
    clearFocus();
    event->accept();
    return;
  }

  QLineEdit::keyPressEvent( event );
}
