// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

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
      /// Constructor
      DFGPickVariableDialog(
        QWidget * parent,
        FabricCore::Client & client,
        FabricCore::DFGBinding & binding,
        FTL::CStrRef currentExecPath,
        bool setAlphaNum = false,
        QString name = "",
        const DFGConfig & dfgConfig = DFGConfig()
        );

      /// Destuctor
      virtual ~DFGPickVariableDialog();

      /// Filter the QLineEdit text with the regexFilter (here the name)
      void alphaNumicStringOnly();
      /// Filter the QLineEdit text with the setRegexFilter (here the name)
      void setRegexFilter(QString regex);

      /// Gets the name of the variable
      QString name() const;

    private:

      DFGVariablePathComboBox * m_nameCombo;

    };

  };

};

#endif // __UI_DFG_DFGPickVariableDialog__
