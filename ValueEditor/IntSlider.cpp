//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "IntSlider.h"

IntSlider::IntSlider( QWidget *parent )
{
  setFocusPolicy(Qt::StrongFocus);

  // Default to horizontal orientation
  setOrientation( Qt::Horizontal );
}

void IntSlider::wheelEvent( QWheelEvent *event )
{
  if (!hasFocus())
  {
    event->ignore();
    return;
  }

  wheelEvent( event );
}
