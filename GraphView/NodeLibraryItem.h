// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_NodeLibraryItem__
#define __UI_GraphView_NodeLibraryItem__

#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QtGui/QColor>

namespace FabricUI
{

  namespace GraphView
  {

    class NodeLibraryItem : public QLabel
    {
      Q_OBJECT

    public:

      NodeLibraryItem(QWidget * parent, QString path, QString label, QColor color);
      virtual ~NodeLibraryItem() {}

      QString path() const;
      QColor color() const;

      virtual void mousePressEvent(QMouseEvent * event);

    protected:
      virtual void enterEvent(QEvent * event);
      virtual void leaveEvent(QEvent * event);

    private:

      QString m_path;
      QColor m_color;
      bool m_highlighted;
    };

  };

};

#endif // __UI_GraphView_NodeLibraryItem__
