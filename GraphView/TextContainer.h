// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_TextContainer__
#define __UI_GraphView_TextContainer__

#include <QtGui/QGraphicsWidget>
#include <QtGui/QGraphicsTextItem>
#include <QtGui/QColor>
#include <QtGui/QFont>

namespace FabricUI
{

  namespace GraphView
  {

    class TextContainer : public QGraphicsWidget
    {
    public:

      TextContainer(
        QGraphicsWidget * parent,
        QString const &text,
        QColor color,
        QColor hlColor,
        QFont font
        );
      virtual ~TextContainer() {}

      virtual QString text() const
        { return m_textItem->text(); }
      virtual void setText(QString const &text);
      virtual QColor color() const;
      virtual QColor highlightColor() const;
      virtual void setColor(QColor color, QColor hlColor);
      virtual bool highlighted() const;
      virtual void setHighlighted(bool state = true);
      virtual QFont font() const;
      virtual void setFont(QFont font);

      QGraphicsSimpleTextItem * textItem();

    protected:

      void refresh();

    private:

      QColor m_color;
      QFont m_font;
      QColor m_highlightColor;
      bool m_highlighted;
      QGraphicsSimpleTextItem * m_textItem;
    };

  };

};

#endif // __UI_GraphView_TextContainer__
