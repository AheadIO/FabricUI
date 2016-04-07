//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "IntSlider.h"

using namespace FabricUI::ValueEditor;

IntSlider::IntSlider( QWidget *parent )
{
  setFocusPolicy(Qt::StrongFocus);

  // Default to horizontal orientation
  setOrientation( Qt::Horizontal );
}

void IntSlider::wheelEvent( QWheelEvent *event )
{
  // [FE-5997] inspired by http://stackoverflow.com/questions/5821802/qspinbox-inside-a-qscrollarea-how-to-prevent-spin-box-from-stealing-focus-when
  if (!hasFocus())
  {
    event->ignore();
    return;
  }

  wheelEvent( event );
}
