// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGGetTextDialog__
#define __UI_DFG_DFGGetTextDialog__

#include <QtGui/QTextEdit>
#include "DFGBaseDialog.h"

namespace FabricUI
{
  namespace DFG
  {

    class DFGGetTextDialog : public DFGBaseDialog
    {
      Q_OBJECT

    public:

      DFGGetTextDialog(QWidget * parent, QString text, const DFGConfig & dfgConfig = DFGConfig());
      virtual ~DFGGetTextDialog();

      QString text() const;

    private:

      QTextEdit * m_textEdit;

    };

  };

};

#endif // __UI_DFG_DFGGetTextDialog__
