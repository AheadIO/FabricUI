// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGExecHeaderWidget__
#define __UI_DFG_DFGExecHeaderWidget__

#include <FabricCore.h>

#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

#include <FTL/StrRef.h>

#include <FabricUI/GraphView/GraphConfig.h>

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
        QString caption,
        const GraphView::GraphConfig & config = GraphView::GraphConfig()
        );
      virtual ~DFGExecHeaderWidget();

      QString caption() const;
      QFont font() const;
      QColor fontColor() const;
      
      void refreshExtDeps( FTL::CStrRef extDeps );

    signals:

      void goUpPressed();

    public slots:

      void refresh();
      void setFont(QFont f);
      void setFontColor(QColor c);
      void onExecChanged();

    protected:

      FabricCore::DFGExec &getExec();

      virtual void paintEvent(QPaintEvent * event);

    protected slots:

      void reqExtEditingFinished();

    private:

      DFGController *m_dfgController;
      QFont m_font;
      QColor m_fontColor;
      QString m_caption;
      QLineEdit *m_reqExtLineEdit;
      QPushButton * m_goUpButton;
      QColor m_backgroundColor;
      QPen m_pen;
      GraphView::GraphConfig m_config;

    };

  };

};

#endif // __UI_DFG_DFGExecHeaderWidget__
