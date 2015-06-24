#include "QtToKLEvent.h"
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>

#include <map>
#include <iostream>

FabricCore::RTVal QtToKLEvent(QEvent *event, FabricCore::Client * client, FabricCore::RTVal viewport)
{

  // Now we translate the Qt events to FabricEngine events..
  FabricCore::RTVal klevent;

  if(event->type() == QEvent::Enter)
  {
    klevent = FabricCore::RTVal::Create(*client, "MouseEvent", 0, 0);
  }
  else if(event->type() == QEvent::Leave)
  {
    klevent = FabricCore::RTVal::Create(*client, "MouseEvent", 0, 0);
  }
  else if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) 
  {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

    klevent = FabricCore::RTVal::Create(*client, "KeyEvent", 0, 0);
    klevent.setMember("key", FabricCore::RTVal::ConstructUInt32(*client, keyEvent->key()));
    klevent.setMember("count", FabricCore::RTVal::ConstructUInt32(*client, keyEvent->count()));
    klevent.setMember("isAutoRepeat", FabricCore::RTVal::ConstructBoolean(*client, keyEvent->isAutoRepeat()));
  } 
  else if ( event->type() == QEvent::MouseMove || 
            event->type() == QEvent::MouseButtonDblClick || 
            event->type() == QEvent::MouseButtonPress || 
            event->type() == QEvent::MouseButtonRelease
          ) 
  {
    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

    klevent = FabricCore::RTVal::Create(*client, "MouseEvent", 0, 0);

    FabricCore::RTVal klpos = FabricCore::RTVal::Construct(*client, "Vec2", 0, 0);
    klpos.setMember("x", FabricCore::RTVal::ConstructFloat32(*client, mouseEvent->pos().x()));
    klpos.setMember("y", FabricCore::RTVal::ConstructFloat32(*client, mouseEvent->pos().y()));

    klevent.setMember("button", FabricCore::RTVal::ConstructUInt32(*client, mouseEvent->button()));
    klevent.setMember("buttons", FabricCore::RTVal::ConstructUInt32(*client, mouseEvent->buttons()));
    klevent.setMember("pos", klpos);
  } 
  else if (event->type() == QEvent::Wheel) 
  {
    QWheelEvent *mouseWheelEvent = static_cast<QWheelEvent *>(event);

    klevent = FabricCore::RTVal::Create(*client, "MouseWheelEvent", 0, 0);

    FabricCore::RTVal klpos = FabricCore::RTVal::Construct(*client, "Vec2", 0, 0);
    klpos.setMember("x", FabricCore::RTVal::ConstructFloat32(*client, mouseWheelEvent->pos().x()));
    klpos.setMember("y", FabricCore::RTVal::ConstructFloat32(*client, mouseWheelEvent->pos().y()));

    klevent.setMember("buttons", FabricCore::RTVal::ConstructUInt32(*client, mouseWheelEvent->buttons()));
    klevent.setMember("delta", FabricCore::RTVal::ConstructSInt32(*client, mouseWheelEvent->delta()));
    klevent.setMember("pos", klpos);
  }

  if(klevent.isValid())
  {
    int eventType = int(event->type());

    klevent.setMember("eventType", FabricCore::RTVal::ConstructUInt32(*client, eventType));

    QInputEvent *inputEvent = static_cast<QInputEvent *>(event);
    klevent.setMember("modifiers", FabricCore::RTVal::ConstructUInt32(*client, inputEvent->modifiers()));

    //////////////////////////
    // Setup the viewport
    klevent.setMember("viewport", viewport);

    //////////////////////////
    // Setup the Host
    // We cannot set an interface value via RTVals.
    FabricCore::RTVal host = FabricCore::RTVal::Create(*client, "Host", 0, 0);
    host.setMember("hostName", FabricCore::RTVal::ConstructString(*client, "Splice Standalone"));
    klevent.setMember("host", host);
  }
  return klevent;
}
