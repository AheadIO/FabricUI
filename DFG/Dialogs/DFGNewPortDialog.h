// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGNewPortDialog__
#define __UI_DFG_DFGNewPortDialog__

#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>
#include "DFGBaseDialog.h"

namespace FabricUI
{
  namespace DFG
  {

    class DFGNewPortDialog : public DFGBaseDialog
    {
      Q_OBJECT

    public:

      DFGNewPortDialog(QWidget * parent, bool showPortType, const DFGConfig & dfgConfig = DFGConfig());
      virtual ~DFGNewPortDialog();

      QString portType() const;
      QString title() const;
      QString dataType() const;
      QString extension() const;

    private:

      QComboBox * m_portTypeCombo;
      QLineEdit * m_titleEdit;
      QLineEdit * m_dataTypeEdit;
      QLineEdit * m_extensionEdit;

    };

  };

};

#endif // __UI_DFG_DFGNewPortDialog__
