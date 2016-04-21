//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_VALUEEDITOR_COMBOBOX_H
#define FABRICUI_VALUEEDITOR_COMBOBOX_H

#include <QtGui/QComboBox>
#include <QtGui/QWheelEvent>


namespace FabricUI {
namespace ValueEditor {

class ComboBox : public QComboBox {
  Q_OBJECT

public:

  ComboBox( QWidget *parent = 0 );

  virtual void wheelEvent( QWheelEvent *event ) /*override*/;
};

} // namespace FabricUI 
} // namespace ValueEditor 

#endif // FABRICUI_VALUEEDITOR_COMBOBOX_H
