// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGExecHeaderWidget__
#define __UI_DFG_DFGExecHeaderWidget__

#include <FabricCore.h>

#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

#include <FTL/StrRef.h>

#include <FabricUI/GraphView/GraphConfig.h>

class QLabel;

namespace FabricUI
{
  namespace DFG
  {
    class DFGController;

    class DFGExecHeaderWidget : public QWidget
    {
      Q_OBJECT

    public:

      DFGExecHeaderWidget(
        QWidget * parent,
        DFGController *dfgController,
        const GraphView::GraphConfig & config = GraphView::GraphConfig()
        );
      virtual ~DFGExecHeaderWidget();
      
      void refreshTitle( FTL::CStrRef title );
      void refreshExtDeps( FTL::CStrRef extDeps );

    signals:

      void goUpPressed();

    public slots:

      void refresh();
      void onExecChanged();

    protected:

      FTL::CStrRef getExecPath();
      FabricCore::DFGExec &getExec();

      virtual void paintEvent(QPaintEvent * event);

    protected slots:

      void reqExtEditingFinished();
      void reqExtReturnPressed();
      void onSplitFromPresetClicked();

    private:

      DFGController *m_dfgController;
      QLabel *m_execPathLabel;
      QLabel *m_presetNameLabel;
      QLabel *m_reqExtLabel;
      QLineEdit *m_reqExtLineEdit;
      QPushButton * m_goUpButton;
      QColor m_backgroundColor;
      QPen m_pen;
      QWidget *m_presetSplitWidget;

    };

  };

};

#endif // __UI_DFG_DFGExecHeaderWidget__
