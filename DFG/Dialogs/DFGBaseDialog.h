// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGBaseDialog__
#define __UI_DFG_DFGBaseDialog__

#include <QtGui/QDialog>
#include "../DFGConfig.h"
#include "DFGCollapsableWidget.h"

namespace FabricUI
{
  namespace DFG
  {

    class DFGWidget;
    class DFGController;

    class DFGBaseDialog : public QDialog 
    {
      Q_OBJECT

    public:

      DFGBaseDialog(QWidget * parent, bool useGridLayout = false, const DFGConfig & dfgConfig = DFGConfig());
      virtual ~DFGBaseDialog();

      virtual DFGWidget * getDFGWidget();
      virtual DFGController * getDFGController();

      virtual void showEvent(QShowEvent * event);
      virtual void addInput(QWidget * widget, QString label = "", QString section = "");
      virtual unsigned int inputCount() const;
      virtual QWidget * input(unsigned int index);
      virtual QWidget * input(QString label);
      virtual void removeSection(QWidget * widget);
      virtual void setSectionCollapsed(QString section, bool state = true);
      virtual void setSectionCollapsable(QString section, bool state = true);

    private slots:

      void onSectionToggled();

    protected:

      virtual QWidget * inputsWidget();
      virtual QWidget * buttonsWidget();


      bool m_usesGridLayout;
      QWidget * m_inputsWidget;
      QWidget * m_buttonsWidget;
      DFGConfig m_dfgConfig;

      std::vector<DFGCollapsableWidget*> m_sections;
      std::vector<QWidget *> m_inputs;
      std::map<std::string, unsigned int> m_labelToIndex;
    };

  };

};

#include "../DFGWidget.h"
#include "../DFGController.h"

#endif // __UI_DFG_DFGBaseDialog__
