#include "QtToKLEvent.h"

#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>

#include <map>
#include <iostream>

inline FabricCore::RTVal QtToKLMousePosition(QPoint pos, FabricCore::Client const& client, FabricCore::RTVal viewport, bool swapAxis) {
  FabricCore::RTVal klViewportDim = viewport.callMethod("Vec2", "getDimensions", 0, 0);
  FabricCore::RTVal klpos = FabricCore::RTVal::Construct(client, "Vec2", 0, 0);
  klpos.setMember("x", FabricCore::RTVal::ConstructFloat32(client, pos.x()));
  // We must inverse the y coordinate to match Qt/RTR viewport system of coordonates
  if(swapAxis)
    klpos.setMember("y", FabricCore::RTVal::ConstructFloat32(client, klViewportDim.maybeGetMember("y").getFloat32() - pos.y()));
  else
    klpos.setMember("y", FabricCore::RTVal::ConstructFloat32(client, pos.y()));
  return klpos;
}

FabricCore::RTVal QtToKLEvent(QEvent *event, FabricCore::Client const& client, FabricCore::RTVal viewport, bool swapAxis)
{
  // Now we translate the Qt events to FabricEngine events..
  FabricCore::RTVal klevent;

  if(event->type() == QEvent::Enter)
  {
    klevent = FabricCore::RTVal::Create(client, "MouseEvent", 0, 0);
  }
  
  else if(event->type() == QEvent::Leave)
  {
    klevent = FabricCore::RTVal::Create(client, "MouseEvent", 0, 0);
  }

  else if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) 
  {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
    klevent = FabricCore::RTVal::Create(client, "KeyEvent", 0, 0);
    klevent.setMember("key", FabricCore::RTVal::ConstructUInt32(client, keyEvent->key()));
    klevent.setMember("count", FabricCore::RTVal::ConstructUInt32(client, keyEvent->count()));
    klevent.setMember("isAutoRepeat", FabricCore::RTVal::ConstructBoolean(client, keyEvent->isAutoRepeat()));
  } 

  else if(event->type() == QEvent::MouseMove || 
          event->type() == QEvent::MouseButtonDblClick || 
          event->type() == QEvent::MouseButtonPress || 
          event->type() == QEvent::MouseButtonRelease) 
  {
    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
    klevent = FabricCore::RTVal::Create(client, "MouseEvent", 0, 0);
    FabricCore::RTVal klpos = QtToKLMousePosition(mouseEvent->pos(), client, viewport, swapAxis);
    klevent.setMember("button", FabricCore::RTVal::ConstructUInt32(client, mouseEvent->button()));
    klevent.setMember("buttons", FabricCore::RTVal::ConstructUInt32(client, mouseEvent->buttons()));
    klevent.setMember("pos", klpos);
  } 

  else if (event->type() == QEvent::Wheel) 
  {
    QWheelEvent *mouseWheelEvent = static_cast<QWheelEvent *>(event);
    klevent = FabricCore::RTVal::Create(client, "MouseWheelEvent", 0, 0);
    FabricCore::RTVal klpos = QtToKLMousePosition(mouseWheelEvent->pos(), client, viewport, swapAxis);
    klevent.setMember("buttons", FabricCore::RTVal::ConstructUInt32(client, mouseWheelEvent->buttons()));
    klevent.setMember("delta", FabricCore::RTVal::ConstructSInt32(client, mouseWheelEvent->delta()));
    klevent.setMember("pos", klpos);
  }

  if(klevent.isValid())
  {
    int eventType = int(event->type());
    // FABRIC_TRY_RETURN("ManipulationTool::onEvent", false,
    klevent.setMember("eventType", FabricCore::RTVal::ConstructUInt32(client, eventType));

    QInputEvent *inputEvent = static_cast<QInputEvent *>(event);
    klevent.setMember("modifiers", FabricCore::RTVal::ConstructUInt32(client, inputEvent->modifiers()));

    //////////////////////////
    // Setup the viewport
    klevent.setMember("viewport", viewport);

    //////////////////////////
    // Setup the Host
    // We cannot set an interface value via RTVals.
    FabricCore::RTVal host = FabricCore::RTVal::Create(client, "Host", 0, 0);
    host.setMember("hostName", FabricCore::RTVal::ConstructString(client, "Canvas"));
    klevent.setMember("host", host);
  }
  return klevent;
}
