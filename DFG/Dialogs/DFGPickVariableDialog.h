// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGPickVariableDialog__
#define __UI_DFG_DFGPickVariableDialog__

#include <QtGui/QLineEdit>
#include "DFGBaseDialog.h"
#include "DFGVariablePathComboBox.h"
#include "DFGRegisteredTypeLineEdit.h"
#include "DFGExtensionLineEdit.h"

namespace FabricUI
{
  namespace DFG
  {

    class DFGPickVariableDialog : public DFGBaseDialog
    {
      Q_OBJECT

    public:

      DFGPickVariableDialog(
        QWidget * parent,
        FabricCore::Client & client,
        FabricCore::DFGBinding & binding,
        FTL::CStrRef currentExecPath,
        QString name = "",
        const DFGConfig & dfgConfig = DFGConfig()
        );
      virtual ~DFGPickVariableDialog();

      QString name() const;

    private:

      DFGVariablePathComboBox * m_nameCombo;

    };

  };

};

#endif // __UI_DFG_DFGPickVariableDialog__
