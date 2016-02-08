// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGCollapsableWidget__
#define __UI_DFG_DFGCollapsableWidget__

#include <QtGui/QWidget>
#include "DFGCollapsableHeaderWidget.h"
#include "../DFGConfig.h"

namespace FabricUI
{
  namespace DFG
  {

    class DFGCollapsableWidget : public QWidget
    {
      Q_OBJECT

    public:

      DFGCollapsableWidget(QWidget * parent, QString title, bool useGridLayout = false, const DFGConfig & dfgConfig = DFGConfig());
      virtual ~DFGCollapsableWidget();

      QString title() const;
      void addWidget(QWidget * widget, QString label = "");
      void removeWidget(QWidget * widget);
      DFGCollapsableHeaderWidget * header() { return m_header; }

      bool collapsed() const { return m_collapsed; }
      void setCollapsed(bool state);

      bool collapsable() const { return m_collapsable; }
      void setCollapsable(bool state);

    signals:

      void collapsed();
      void expanded();
      void toggled();

    private slots:

      void onToggled();

    private:

      bool m_collapsed;
      bool m_collapsable;
      DFGConfig m_dfgConfig;
      DFGCollapsableHeaderWidget * m_header;
      QWidget * m_content;
      bool m_usesGridLayout;

    };

  };

};

#endif // __UI_DFG_DFGCollapsableWidget__
