// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGVariablePathLineEdit__
#define __UI_DFG_DFGVariablePathLineEdit__

#include "DFGAutoCompleteLineEdit.h"
#include <FabricCore.h>
#include <FTL/CStrRef.h>

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
        FTL::CStrRef currentExecPath,
        QString text = ""
        );
    };

  };

};

#endif // __UI_DFG_DFGVariablePathLineEdit__
