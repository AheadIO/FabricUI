// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGExtensionLineEdit__
#define __UI_DFG_DFGExtensionLineEdit__

#include "DFGAutoCompleteLineEdit.h"
#include <FabricCore.h>

namespace FabricUI
{
  namespace DFG
  {

    class DFGExtensionLineEdit : public DFGAutoCompleteLineEdit
    {
      Q_OBJECT

    public:

      DFGExtensionLineEdit(QWidget * parent, FabricCore::Client & client, QString text = "");
    };

  };

};

#endif // __UI_DFG_DFGExtensionLineEdit__
