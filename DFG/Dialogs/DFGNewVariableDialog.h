// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGNewVariableDialog__
#define __UI_DFG_DFGNewVariableDialog__

#include <QtGui/QLineEdit>
#include "DFGBaseDialog.h"
#include "DFGVariablePathLineEdit.h"
#include "DFGRegisteredTypeLineEdit.h"
#include "DFGExtensionLineEdit.h"

namespace FabricUI
{
  namespace DFG
  {

    class DFGNewVariableDialog : public DFGBaseDialog
    {
      Q_OBJECT

    public:

      DFGNewVariableDialog(
        QWidget * parent,
        FabricCore::Client & client,
        FabricCore::DFGBinding & binding,
        QString name = "",
        QString dataType = "",
        QString extension = "",
        const DFGConfig & dfgConfig = DFGConfig()
        );
      virtual ~DFGNewVariableDialog();

      QString name() const;
      QString dataType() const;
      QString extension() const;

    private:

      DFGVariablePathLineEdit * m_nameEdit;
      DFGRegisteredTypeLineEdit * m_dataTypeEdit;
      DFGExtensionLineEdit * m_extensionEdit;

    };

  };

};

#endif // __UI_DFG_DFGNewVariableDialog__
