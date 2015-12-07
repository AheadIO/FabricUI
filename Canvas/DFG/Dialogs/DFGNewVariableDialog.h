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

      /// Constructor
      DFGNewVariableDialog(
        QWidget * parent,
        FabricCore::Client & client,
        FabricCore::DFGBinding & binding,
        FTL::CStrRef currentExecPath,
        bool setAlphaNum = true,
        QString name = "",
        QString dataType = "",
        QString extension = "",
        const DFGConfig & dfgConfig = DFGConfig()
        );
      /// Destuctor
      virtual ~DFGNewVariableDialog();
      
      /// Filter the QLineEdit text with the regexFilter (here the name)
      void alphaNumicStringOnly();
      /// Filter the QLineEdit text with the setRegexFilter (here the name)
      void setRegexFilter(QString regex);

      /// Gets the name of the variable
      QString name() const;
      /// Gets the type of the variable
      QString dataType() const;
      /// Gets the extension of the variable
      QString extension() const;

    private:
      /// \internal
      DFGVariablePathLineEdit * m_nameEdit;
      /// \internal
      DFGRegisteredTypeLineEdit * m_dataTypeEdit;
      /// \internal
      DFGExtensionLineEdit * m_extensionEdit;

    };

  };

};

#endif // __UI_DFG_DFGNewVariableDialog__
