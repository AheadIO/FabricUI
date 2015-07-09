// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_GraphHeaderWidget__
#define __UI_GraphView_GraphHeaderWidget__

#include <FabricCore.h>

#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

#include <FTL/StrRef.h>

#include "GraphConfig.h"

namespace FabricUI
{
  namespace GraphView
  {

    class GraphHeaderWidget : public QWidget
    {
      Q_OBJECT

    public:

      GraphHeaderWidget(
        QWidget * parent,
        QString caption,
        FabricCore::DFGBinding const &binding,
        FTL::StrRef execPath,
        FabricCore::DFGExec const &exec,
        const GraphConfig & config = GraphConfig()
        );
      virtual ~GraphHeaderWidget();

      QString caption() const;
      QFont font() const;
      QColor fontColor() const;

      void setExec(
        FabricCore::DFGBinding const &binding,
        FTL::StrRef execPath,
        FabricCore::DFGExec const &exec
        );

      void setExecExtDeps( FTL::CStrRef extDeps );

    signals:

      void goUpPressed();

    public slots:

      void setCaption(QString text);
      void setFont(QFont f);
      void setFontColor(QColor c);

    protected:

      virtual void paintEvent(QPaintEvent * event);

    protected slots:

      void reqExtEditingFinished();

    private:

      QFont m_font;
      QColor m_fontColor;
      QString m_caption;
      QLineEdit *m_reqExtLineEdit;
      QPushButton * m_goUpButton;
      QColor m_backgroundColor;
      QPen m_pen;
      GraphConfig m_config;

      FabricCore::DFGBinding m_binding;
      std::string m_execPath;
      FabricCore::DFGExec m_exec;
    };

  };

};

#endif // __UI_GraphView_GraphHeaderWidget__
