//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include <FabricUI/GraphView/TextContainer.h>

namespace FabricUI {
namespace GraphView {

class PortLabel : public TextContainer
{
public:

  PortLabel(
    QGraphicsWidget * parent,
    QString const &text,
    QColor color,
    QColor hlColor,
    QFont font
    );

protected:

  virtual void mousePressEvent( QGraphicsSceneMouseEvent * event );
  virtual void mouseMoveEvent( QGraphicsSceneMouseEvent *event );
  virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent * event );

private:

  QPointF m_dragStartPosition;
};

} // namespace GraphView
} // namespace FabricUI
