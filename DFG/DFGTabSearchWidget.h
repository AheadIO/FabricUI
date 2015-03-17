// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGTabSearchWidget__
#define __UI_DFG_DFGTabSearchWidget__

#include <DFGWrapper/Host.h>
#include "DFGConfig.h"

#include <QtGui/QWidget>
#include <QtGui/QFocusEvent>

namespace FabricUI
{

  namespace DFG
  {
    class DFGWidget;

    class DFGTabSearchWidget : public QWidget 
    {
      Q_OBJECT

    public:

      DFGTabSearchWidget(DFGWidget * parent, const DFGConfig & config = DFGConfig());
      virtual ~DFGTabSearchWidget();

      void showForSearch(QPoint pos);
      void showForSearch();

      virtual void mousePressEvent(QMouseEvent * event);
      virtual void mouseMoveEvent(QMouseEvent * event);
      virtual void keyPressEvent(QKeyEvent * event);
      virtual void paintEvent(QPaintEvent * event);

    protected:

      virtual void focusOutEvent(QFocusEvent * event);
      virtual bool focusNextPrevChild(bool next);
      virtual void updateSearch();
      virtual void updatePixmap();
      virtual int margin() const;

    private:

      QString resultLabel(unsigned int index) const;
      int indexFromPos(QPoint pos);
      int widthFromResults() const;
      int heightFromResults() const;

      DFGConfig m_config;
      DFGWidget * m_parent;
      int m_currentIndex;
      QString m_search;
      QStringList m_results;
      QPoint m_pos;
      QFontMetrics * m_metrics;
      QPixmap m_pixmap;
    };

  };

};

#endif // __UI_DFG_DFGTabSearchWidget__
