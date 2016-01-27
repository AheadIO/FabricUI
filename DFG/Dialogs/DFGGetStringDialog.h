// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGGetStringDialog__
#define __UI_DFG_DFGGetStringDialog__

#include <QtGui/QLineEdit>
#include "DFGBaseDialog.h"

namespace FabricUI
{
  namespace DFG
  {

    class DFGGetStringDialog : public DFGBaseDialog
    {
      Q_OBJECT

    public:
      /// Constructor
      DFGGetStringDialog(QWidget * parent, QString text, const DFGConfig & dfgConfig = DFGConfig(), bool setAlphaNum = false);
      /// Destuctor
      virtual ~DFGGetStringDialog();

      /// Filter the QLineEdit text with the regexFilter
      void alphaNumicStringOnly();
      /// Filter the QLineEdit text with the setRegexFilter
      void setRegexFilter(QString regex);
      /// Gets the text
      QString text() const;

    private:
      /// \internal
      QLineEdit * m_lineEdit;

    };

  };

};

#endif // __UI_DFG_DFGGetStringDialog__
