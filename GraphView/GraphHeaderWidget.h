// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_GraphHeaderWidget__
#define __UI_GraphView_GraphHeaderWidget__

#include <QtGui/QWidget>
#include <QtGui/QPushButton>

#include "GraphConfig.h"

namespace FabricUI
{
  namespace GraphView
  {

    class GraphHeaderWidget : public QWidget
    {
      Q_OBJECT

    public:

      GraphHeaderWidget(QWidget * parent, QString caption, const GraphConfig & config = GraphConfig());
      virtual ~GraphHeaderWidget();

      QString caption() const;
      QFont font() const;
      QColor fontColor() const;

      void setExec(
        FabricCore::DFGBinding const &binding,
        FTL::StrRef execPath,
        FabricCore::DFGExec const &exec
        );

    signals:

      void goUpPressed();

    public slots:

      void setCaption(QString text);
      void setFont(QFont f);
      void setFontColor(QColor c);

    protected:

      virtual void paintEvent(QPaintEvent * event);

      void updateReqExtLineEdit();

    protected slots:

      void reqExtEditingFinished();

    private:

      QFont m_font;
      QColor m_fontColor;
      QString m_caption;
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
