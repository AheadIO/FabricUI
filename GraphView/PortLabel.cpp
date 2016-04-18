//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include <FabricUI/GraphView/Graph.h>
#include <FabricUI/GraphView/Port.h>
#include <FabricUI/GraphView/PortLabel.h>

#include <QtGui/QApplication>
#include <QtGui/QGraphicsSceneMouseEvent>

namespace FabricUI {
namespace GraphView {

PortLabel::PortLabel(
  QGraphicsWidget * parent,
  QString const &text,
  QColor color,
  QColor hlColor,
  QFont font
  )
  : TextContainer(
    parent,
    text,
    color,
    hlColor,
    font
    )
{
}

void PortLabel::mousePressEvent( QGraphicsSceneMouseEvent *event )
{
  if ( !!(event->buttons() & Qt::LeftButton) )
  {
    Port *port = static_cast<Port *>( parentItem() );
    if ( port->graph()->isEditable() )
    {
      m_dragStartPosition = event->pos();
      event->accept();
      return;
    }
  }
  
  TextContainer::mousePressEvent( event );
}

void PortLabel::mouseMoveEvent( QGraphicsSceneMouseEvent *event )
{
  if ( !!(event->buttons() & Qt::LeftButton) )
  {
    if ( (event->pos() - m_dragStartPosition).manhattanLength()
       >= QApplication::startDragDistance() )
    {
      Port *port = static_cast<Port *>( parentItem() );
      if ( port->graph()->isEditable() )
      {
        event->accept();

        QDrag *drag = new QDrag( event->widget() );

        Port::MimeData *mimeData = new Port::MimeData( port );
        drag->setMimeData( mimeData );

        Qt::DropAction dropAction = drag->exec( Qt::MoveAction );
        (void)dropAction;

        return;
      }
    }
  }

  TextContainer::mouseMoveEvent( event );
}

void PortLabel::mouseReleaseEvent( QGraphicsSceneMouseEvent *event )
{
  if ( !!(event->buttons() & Qt::LeftButton) )
  {
    Port *port = static_cast<Port *>( parentItem() );
    if ( port->graph()->isEditable() )
    {
      event->accept();
      return;
    }
  }
  
  TextContainer::mouseReleaseEvent( event );  
}

} // namespace GraphView
} // namespace FabricUI
