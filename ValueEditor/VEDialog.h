//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

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
