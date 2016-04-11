
#include "QtToKLEvent.h"
#include <map>
#include <iostream>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>

using namespace FabricCore;

inline FabricCore::RTVal QtToKLMousePosition(QPoint pos, Client const& client, RTVal viewport, bool swapAxis) {
  RTVal klViewportDim = viewport.callMethod("Vec2", "getDimensions", 0, 0);
  RTVal klpos = RTVal::Construct(client, "Vec2", 0, 0);
  klpos.setMember("x", RTVal::ConstructFloat32(client, pos.x()));
  // We must inverse the y coordinate to match Qt/RTR viewport system of coordonates
  if(swapAxis)
    klpos.setMember("y", RTVal::ConstructFloat32(client, klViewportDim.maybeGetMember("y").getFloat32() - pos.y()));
  else
    klpos.setMember("y", RTVal::ConstructFloat32(client, pos.y()));
  return klpos;
}

RTVal QtToKLEvent(QEvent *event, Client const& client, RTVal viewport, bool swapAxis) {
  // Now we translate the Qt events to FabricEngine events..
  RTVal klevent;

  if(event->type() == QEvent::Enter)
    klevent = RTVal::Create(client, "MouseEvent", 0, 0);
  
  else if(event->type() == QEvent::Leave)
    klevent = RTVal::Create(client, "MouseEvent", 0, 0);


  else if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) 
  {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
    klevent = RTVal::Create(client, "KeyEvent", 0, 0);
    klevent.setMember("key", RTVal::ConstructUInt32(client, keyEvent->key()));
    klevent.setMember("count", RTVal::ConstructUInt32(client, keyEvent->count()));
    klevent.setMember("isAutoRepeat", RTVal::ConstructBoolean(client, keyEvent->isAutoRepeat()));
  } 

  else if(event->type() == QEvent::MouseMove || 
          event->type() == QEvent::MouseButtonDblClick || 
          event->type() == QEvent::MouseButtonPress || 
          event->type() == QEvent::MouseButtonRelease) 
  {
    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
    klevent = RTVal::Create(client, "MouseEvent", 0, 0);
    RTVal klpos = QtToKLMousePosition(mouseEvent->pos(), client, viewport, swapAxis);
    klevent.setMember("button", RTVal::ConstructUInt32(client, mouseEvent->button()));
    klevent.setMember("buttons", RTVal::ConstructUInt32(client, mouseEvent->buttons()));
    klevent.setMember("pos", klpos);
  } 

  else if (event->type() == QEvent::Wheel) 
  {
    QWheelEvent *mouseWheelEvent = static_cast<QWheelEvent *>(event);
    klevent = RTVal::Create(client, "MouseWheelEvent", 0, 0);
    RTVal klpos = QtToKLMousePosition(mouseWheelEvent->pos(), client, viewport, swapAxis);
    klevent.setMember("buttons", RTVal::ConstructUInt32(client, mouseWheelEvent->buttons()));
    klevent.setMember("delta", RTVal::ConstructSInt32(client, mouseWheelEvent->delta()));
    klevent.setMember("pos", klpos);
  }

  if(klevent.isValid())
  {
    int eventType = int(event->type());
    // FABRIC_TRY_RETURN("ManipulationTool::onEvent", false,
    klevent.setMember("eventType", RTVal::ConstructUInt32(client, eventType));

    QInputEvent *inputEvent = static_cast<QInputEvent *>(event);
    klevent.setMember("modifiers", RTVal::ConstructUInt32(client, inputEvent->modifiers()));

    //////////////////////////
    // Setup the viewport
    klevent.setMember("viewport", viewport);

    //////////////////////////
    // Setup the Host
    // We cannot set an interface value via RTVals.
    RTVal host = RTVal::Create(client, "Host", 0, 0);
    host.setMember("hostName", RTVal::ConstructString(client, "Canvas"));
    klevent.setMember("host", host);
  }
  return klevent;
}
