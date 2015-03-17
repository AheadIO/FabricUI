// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGBindValueDialog__
#define __UI_DFG_DFGBindValueDialog__

#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>
#include "DFGBaseDialog.h"

namespace FabricUI
{
  namespace DFG
  {

    class DFGBindValueDialog : public DFGBaseDialog
    {
      Q_OBJECT

    public:

      DFGBindValueDialog(QWidget * parent, QString dataType = "Scalar", const DFGConfig & dfgConfig = DFGConfig());
      virtual ~DFGBindValueDialog();

      QString dataType() const;
      QString extension() const;

    private:

      QLineEdit * m_dataTypeEdit;
      QLineEdit * m_extensionEdit;

    };

  };

};

#endif // __UI_DFG_DFGBindValueDialog__
