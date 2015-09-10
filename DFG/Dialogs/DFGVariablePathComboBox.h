// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGVariablePathComboBox__
#define __UI_DFG_DFGVariablePathComboBox__

#include <QtGui/QComboBox>
#include <FabricCore.h>
#include <FTL/CStrRef.h>

namespace FabricUI
{
  namespace DFG
  {

    class DFGVariablePathComboBox : public QComboBox
    {
      Q_OBJECT

    public:

      DFGVariablePathComboBox(
        QWidget * parent,
        FabricCore::DFGBinding const &binding,
        FTL::CStrRef currentExecPath,
        QString text = ""
        );
    };

  };

};

#endif // __UI_DFG_DFGVariablePathComboBox__
