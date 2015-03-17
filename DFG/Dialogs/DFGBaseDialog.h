// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGBaseDialog__
#define __UI_DFG_DFGBaseDialog__

#include <QtGui/QDialog>
#include "../DFGConfig.h"

namespace FabricUI
{
  namespace DFG
  {

    class DFGBaseDialog : public QDialog 
    {
      Q_OBJECT

    public:

      DFGBaseDialog(QWidget * parent, bool useGridLayout = false, const DFGConfig & dfgConfig = DFGConfig());
      virtual ~DFGBaseDialog();


      virtual void showEvent(QShowEvent * event);

    protected:

      virtual QWidget * inputsWidget();
      virtual QWidget * buttonsWidget();

      virtual void addInput(QWidget * widget, QString label = "");

      bool m_usesGridLayout;
      QWidget * m_inputsWidget;
      QWidget * m_buttonsWidget;
      DFGConfig m_dfgConfig;

      std::vector<QWidget *> m_inputs;
    };

  };

};

#endif // __UI_DFG_DFGBaseDialog__
