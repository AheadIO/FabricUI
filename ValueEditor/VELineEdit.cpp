//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
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
  QLineEdit::focusInEvent( event );
}
