//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "ComboBox.h"
using namespace FabricUI::ValueEditor;

ComboBox::ComboBox( QWidget *parent )
{
  setFocusPolicy( Qt::StrongFocus );
}

void ComboBox::wheelEvent( QWheelEvent *event )
{
  // [FE-5997] inspired by http://stackoverflow.com/questions/5821802/qspinbox-inside-a-qscrollarea-how-to-prevent-spin-box-from-stealing-focus-when
  if (!hasFocus())
  {
    event->ignore();
    return;
  }

  QComboBox::wheelEvent( event );
}
