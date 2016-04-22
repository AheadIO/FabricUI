//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_VALUEEDITOR_VEDIALOG_H
#define FABRICUI_VALUEEDITOR_VEDIALOG_H

#include <QtGui/QDialog>
#include <QtGui/QShowEvent>

namespace FabricUI {
namespace ValueEditor {

class VEDialog : public QDialog
{
  Q_OBJECT

public:

  VEDialog( QWidget * parent = 0, Qt::WindowFlags f = 0 );

protected:

  void showEvent( QShowEvent * event );
};

} // namespace FabricUI 
} // namespace ValueEditor 

#endif // FABRICUI_VALUEEDITOR_VEDIALOG_H
