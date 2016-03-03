// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGRegisteredTypeLineEdit__
#define __UI_DFG_DFGRegisteredTypeLineEdit__

#include "DFGAutoCompleteLineEdit.h"
#include <FabricCore.h>

namespace FabricUI
{
  namespace DFG
  {

    class DFGRegisteredTypeLineEdit : public DFGAutoCompleteLineEdit
    {
      Q_OBJECT

    public:

      DFGRegisteredTypeLineEdit(QWidget * parent, FabricCore::Client & client, QString text = "");
    };

  };

};

#endif // __UI_DFG_DFGRegisteredTypeLineEdit__
