// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGVariablePathLineEdit__
#define __UI_DFG_DFGVariablePathLineEdit__

#include "DFGAutoCompleteLineEdit.h"
#include <FabricCore.h>

namespace FabricUI
{
  namespace DFG
  {

    class DFGVariablePathLineEdit : public DFGAutoCompleteLineEdit
    {
      Q_OBJECT

    public:

      DFGVariablePathLineEdit(
        QWidget * parent,
        FabricCore::DFGBinding const &binding,
        QString text = ""
        );
    };

  };

};

#endif // __UI_DFG_DFGVariablePathLineEdit__
