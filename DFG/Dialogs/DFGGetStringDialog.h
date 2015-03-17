// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

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

      DFGGetStringDialog(QWidget * parent, QString text, const DFGConfig & dfgConfig = DFGConfig());
      virtual ~DFGGetStringDialog();

      QString text() const;

    private:

      QLineEdit * m_lineEdit;

    };

  };

};

#endif // __UI_DFG_DFGGetStringDialog__
