/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include "SHGLRenderer.h"
#include <FabricUI/Viewports/QtToKLEvent.h>

using namespace FabricCore;
using namespace FabricUI::SceneHub;


SHGLRenderer::SHGLRenderer(FabricCore::Client client) : m_client(client) {
  try 
  {
    RTVal dummyGLRendererVal = RTVal::Construct( m_client, "SHGLRenderer", 0, 0 );
    m_shGLRendererVal = dummyGLRendererVal.callMethod( "SHRenderer_Virtual", "create", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::SHGLRenderer: exception: %s\n", e.getDesc_cstr());
  }
}

void SHGLRenderer::update() {
  try 
  {
    RTVal dummyGLRendererVal = RTVal::Construct( m_client, "SHGLRenderer", 0, 0 );
    m_shGLRendererVal = dummyGLRendererVal.callMethod( "SHRenderer_Virtual", "getOrCreate", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::update: exception: %s\n", e.getDesc_cstr());
  }
}

void SHGLRenderer::getDrawStats(
  uint32_t viewportID, 
  uint32_t &obj, 
  uint32_t &point, 
  uint32_t &line, 
  uint32_t &triangle) 
{
  try 
  {
    RTVal args[5] = {
      RTVal::ConstructUInt32(m_client, viewportID), 
      RTVal::ConstructUInt32(m_client, 0),
      RTVal::ConstructUInt32(m_client, 0),
      RTVal::ConstructUInt32(m_client, 0),
      RTVal::ConstructUInt32(m_client, 0)
    };
    m_shGLRendererVal.callMethod("Boolean", "getDrawStats", 5, &args[0]);
    obj = args[1].getUInt32();
    point = args[2].getUInt32();
    line = args[3].getUInt32();
    triangle = args[4].getUInt32();
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::getOrAddViewport: exception: %s\n", e.getDesc_cstr());
  }
}

RTVal SHGLRenderer::getOrAddViewport(uint32_t viewportID) {
  RTVal viewportVal;
  try 
  {
    RTVal arg = RTVal::ConstructUInt32(m_client, viewportID);
    viewportVal = m_shGLRendererVal.callMethod("BaseRTRViewport", "getOrAddViewport", 1, &arg);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::getOrAddViewport: exception: %s\n", e.getDesc_cstr());
  }
  return viewportVal;
}

RTVal SHGLRenderer::getOrAddStereoViewport(uint32_t viewportID) {
  RTVal viewportVal;
  try 
  {
    RTVal arg = RTVal::ConstructUInt32(m_client, viewportID);
    viewportVal = m_shGLRendererVal.callMethod("BaseRTRViewport", "getOrAddStereoViewport", 1, &arg);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::getOrAddStereoViewport: exception: %s\n", e.getDesc_cstr());
  }
  return viewportVal;
}

void SHGLRenderer::removeViewport(uint32_t viewportID) {
  try 
  {
    RTVal glRenderer = m_shGLRendererVal;
    if( glRenderer.isValid() && !glRenderer.isNullObject() ) {
      RTVal arg = RTVal::ConstructUInt32( m_client, viewportID );
      m_shGLRendererVal.callMethod( "BaseRTRViewport", "removeViewport", 1, &arg );
    }
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::removeViewport: exception: %s\n", e.getDesc_cstr());
  }
}

void SHGLRenderer::setOrthographicViewport(uint32_t viewportID, bool orthographic) {
  try 
  {
    RTVal args[2] = {
      RTVal::ConstructUInt32(m_client, viewportID),
      RTVal::ConstructBoolean(m_client, orthographic)
    };
    m_shGLRendererVal.callMethod("", "setOrthographicViewport", 2, &args[0]);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::setOrthographicViewport: exception: %s\n", e.getDesc_cstr());
  }
}

RTVal SHGLRenderer::getCamera(uint32_t viewportID) {
  RTVal cameraVal;
  try 
  {
    RTVal viewportVal = getOrAddViewport(viewportID);
    cameraVal = viewportVal.callMethod("Camera", "getCamera", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::getOrAddStereoViewport: exception: %s\n", e.getDesc_cstr());
  }
  return cameraVal;
}

void SHGLRenderer::setPlayback(bool playback) {
  try 
  {
    RTVal arg = RTVal::ConstructBoolean(m_client, playback);
    m_shGLRendererVal.callMethod("", "setPlayback", 1, &arg);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::setPlayback: exception: %s\n", e.getDesc_cstr());
  }
}

bool SHGLRenderer::isPlayback(bool playback) {
  try 
  {
    return m_shGLRendererVal.callMethod("Boolean", "isPlayback", 0, 0).getBoolean();
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::isPlayback: exception: %s\n", e.getDesc_cstr());
  }
  return false;
}

FabricCore::RTVal SHGLRenderer::castRay(uint32_t viewportID, float pos[2]) {
  RTVal rayVal;
  try 
  {
    RTVal posVal = RTVal::Construct(m_client, "Vec2", 0, 0);
    posVal.setMember("x", RTVal::ConstructFloat32(m_client, pos[0]));
    posVal.setMember("y", RTVal::ConstructFloat32(m_client, pos[1]));
    RTVal args[2] = {
      RTVal::ConstructUInt32(m_client, viewportID),
      posVal
    };
    rayVal = m_shGLRendererVal.callMethod("Ray", "castRay", 2, &args[0]);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::castRay: exception: %s\n", e.getDesc_cstr());
  }
  return rayVal;
}

void SHGLRenderer::get3DScenePosFrom2DScreenPos(uint32_t viewportID, float pos[2], float *pos3D) {
  try 
  {
    RTVal posVal = RTVal::Construct(m_client, "Vec2", 0, 0);
    posVal.setMember("x", RTVal::ConstructFloat32(m_client, pos[0]));
    posVal.setMember("y", RTVal::ConstructFloat32(m_client, pos[1]));
    RTVal args[2] = {
      RTVal::ConstructUInt32(m_client, viewportID),
      posVal
    };
    RTVal pos3DVal = m_shGLRendererVal.callMethod("Vec3", "get3DScenePosFrom2DScreenPos", 2, &args[0]);
    pos3D[0] = pos3DVal.maybeGetMember("x").getFloat32();
    pos3D[1] = pos3DVal.maybeGetMember("y").getFloat32();
    pos3D[2] = pos3DVal.maybeGetMember("z").getFloat32();
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::get3DScenePosFrom2DScreenPos: exception: %s\n", e.getDesc_cstr());
  }
}

void SHGLRenderer::render(uint32_t viewportID, uint32_t width, uint32_t height, uint32_t samples) {
  try 
  {
    RTVal args[4] = {
      RTVal::ConstructUInt32(m_client, viewportID),
      RTVal::ConstructUInt32(m_client, width),
      RTVal::ConstructUInt32(m_client, height),
      RTVal::ConstructUInt32(m_client, samples)
    };
    m_shGLRendererVal.callMethod("", "render", 4, &args[0]);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::render: exception: %s\n", e.getDesc_cstr());
  }
}

void SHGLRenderer::render(uint32_t viewportID, uint32_t width, uint32_t height, uint32_t samples, uint32_t drawPhase) {
  try 
  {
    RTVal args[5] = {
      RTVal::ConstructUInt32(m_client, viewportID),
      RTVal::ConstructUInt32(m_client, width),
      RTVal::ConstructUInt32(m_client, height),
      RTVal::ConstructUInt32(m_client, samples),
      RTVal::ConstructUInt32(m_client, drawPhase)
    };
    m_shGLRendererVal.callMethod("", "render", 5, &args[0]);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::render: exception: %s\n", e.getDesc_cstr());
  }
}

bool SHGLRenderer::onEvent(uint32_t viewportID, QEvent *event, bool &redrawAllViewports) {
  try 
  {
    RTVal viewportVal = getOrAddViewport(viewportID);
    RTVal klevent = QtToKLEvent(event, m_client, viewportVal, true);
    
    m_shGLRendererVal.callMethod("", "onEvent", 1, &klevent);
    bool result = klevent.callMethod("Boolean", "isAccepted", 0, 0).getBoolean();
    event->setAccepted(result);
    redrawAllViewports = klevent.callMethod("Boolean", "redrawAllViewports", 0, 0).getBoolean();
    return result;
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::onEvent: exception: %s\n", e.getDesc_cstr());
  }
  return false;
}

RTVal SHGLRenderer::getToolDispatcher() {
  RTVal toolDispatcherVal;
  try 
  {
    toolDispatcherVal = m_shGLRendererVal.callMethod("RTRToolDispatcher", "getToolDispatcher", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::getToolDispatcher: exception: %s\n", e.getDesc_cstr());
  }
  return toolDispatcherVal;
}

void SHGLRenderer::getRegisteredTools(QStringList &toolNames, QStringList &toolKeys) {
  try 
  {
    RTVal args[2] = {
      RTVal::ConstructVariableArray(m_client, "String"),
      RTVal::ConstructVariableArray(m_client, "Key")
    };
    m_shGLRendererVal.callMethod("", "getRegisteredTools", 2, &args[0]);

    for(uint32_t i=0; i<args[0].getArraySize(); ++i)
    {
      RTVal name = args[0].getArrayElement(i);
      RTVal key = args[1].getArrayElement(i);
      toolNames.append(name.getStringCString());
      toolKeys.append(QKeySequence(key.getUInt32()).toString()); 
    }
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::getRegisteredTools: exception: %s\n", e.getDesc_cstr());
  }
}

RTVal SHGLRenderer::getSelectionSet() {
  RTVal selectionSetVal;
  try 
  {
    selectionSetVal = m_shGLRendererVal.callMethod("SWElementHandleSet", "getSelectionSet", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::getSelectionSet: exception: %s\n", e.getDesc_cstr());
  }
  return selectionSetVal;
}

bool SHGLRenderer::selectionChangedFromManips() {
  try 
  {
    return m_shGLRendererVal.callMethod("Boolean", "selectionChangedFromManips", 0, 0).getBoolean();
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::selectionChangedFromManips: exception: %s\n", e.getDesc_cstr());
  }
  return false;
}

QString SHGLRenderer::getSelectionCategory() {
  try 
  {
    return QString(m_shGLRendererVal.callMethod("String", "getSelectionCategory", 0, 0).getStringCString());
  }
  catch(Exception e)
  {
    printf("SHGLRenderer::getSelectionCategory: exception: %s\n", e.getDesc_cstr());
  }
  return QString();
}
