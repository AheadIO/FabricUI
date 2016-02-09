//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "ComboBox.h"

ComboBox::ComboBox( QWidget *parent )
{
  setFocusPolicy( Qt::StrongFocus );
}

void ComboBox::wheelEvent( QWheelEvent *event )
{
  if (!hasFocus())
  {
    event->ignore();
    return;
  }

  wheelEvent( event );
}
