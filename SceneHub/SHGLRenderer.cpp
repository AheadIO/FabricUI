/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include "SHGLRenderer.h"
using namespace FabricCore;
using namespace FabricUI::SceneHub;

 
RTVal SHGLRenderer::getSHGLRenderer() {
  RTVal shGLRendererVal;
  try 
  {
    shGLRendererVal = RTVal::Construct(m_client, "SHGLRenderer", 0, 0);
    shGLRendererVal = shGLRendererVal.callMethod("SHRenderer_Virtual", "get", 0, 0);
  }
  catch(Exception e)
  {
    printf(
      "SHGLRenderer::getSHGLRenderer: caught exception: %s\n",
      e.getDesc_cstr()
      );
  }
  return shGLRendererVal;
}

RTVal SHGLRenderer::getOrAddViewport(uint32_t viewportID) {
  RTVal arg = RTVal::ConstructUInt32(m_client, viewportID);
  return getSHGLRenderer().callMethod("BaseRTRViewport", "getOrAddViewport", 1, &arg);
}

RTVal SHGLRenderer::getOrAddStereoViewport(uint32_t viewportID) {
  RTVal arg = RTVal::ConstructUInt32(m_client, viewportID);
  return getSHGLRenderer().callMethod("BaseRTRViewport", "getOrAddStereoViewport", 1, &arg);
}

void SHGLRenderer::removeViewport(uint32_t viewportID) {
  RTVal arg = RTVal::ConstructUInt32(m_client, viewportID);
  getSHGLRenderer().callMethod("", "removeViewport", 1, &arg);
}

void SHGLRenderer::setOrthographicViewport(uint32_t viewportID, bool orthographic) {
  RTVal args[2] = {
    RTVal::ConstructUInt32(m_client, viewportID),
    RTVal::ConstructBoolean(m_client, orthographic)
  };
  getSHGLRenderer().callMethod("", "setOrthographicViewport", 2, &args[0]);
}

void SHGLRenderer::setPlayback(bool playback) {
  RTVal arg = RTVal::ConstructBoolean(m_client, playback);
  getSHGLRenderer().callMethod("", "setPlayback", 1, &arg);
}

bool SHGLRenderer::isPlayback(bool playback) {
  return getSHGLRenderer().callMethod("Boolean", "isPlayback", 0, 0).getBoolean();
}

void SHGLRenderer::render(uint32_t viewportID, uint32_t width, uint32_t height, uint32_t samples) {
  RTVal args[4] = {
    RTVal::ConstructUInt32(m_client, viewportID),
    RTVal::ConstructBoolean(m_client, width),
    RTVal::ConstructBoolean(m_client, height),
    RTVal::ConstructBoolean(m_client, samples)
  };
  getSHGLRenderer().callMethod("", "render", 4, &args[0]);
}

void SHGLRenderer::render(uint32_t viewportID, uint32_t width, uint32_t height, uint32_t samples, uint32_t drawPhase) {
  RTVal args[5] = {
    RTVal::ConstructUInt32(m_client, viewportID),
    RTVal::ConstructBoolean(m_client, width),
    RTVal::ConstructBoolean(m_client, height),
    RTVal::ConstructBoolean(m_client, samples),
    RTVal::ConstructBoolean(m_client, drawPhase)
  };
  getSHGLRenderer().callMethod("", "render", 5, &args[0]);
}

void SHGLRenderer::getRegisteredTools(QStringList &toolNames, QStringList &toolKeys) {
  RTVal args[2] = {
    RTVal::ConstructVariableArray(m_client, "String"),
    RTVal::ConstructVariableArray(m_client, "Key")
  };
  getSHGLRenderer().callMethod("", "getRegisteredTools", 2, &args[0]);

  for(uint32_t i=0; i<args[0].getArraySize(); ++i)
  {
    RTVal name = args[0].getArrayElement(i);
    RTVal key = args[1].getArrayElement(i);
    toolNames.append(name.getStringCString());
    toolKeys.append(QKeySequence(key.getUInt32()).toString()); 
  }
}

RTVal SHGLRenderer::getSelectionSet() {
  return getSHGLRenderer().callMethod("SWElementHandleSet", "getSelectionSet", 0, 0);
}

bool SHGLRenderer::onEvent(RTVal event) {
  getSHGLRenderer().callMethod("", "onEvent", 1, &event);
  return false;
}

bool SHGLRenderer::selectionChangedFromManips() {
  return getSHGLRenderer().callMethod("", "selectionChangedFromManips", 0, 0).getBoolean();
}

QString SHGLRenderer::getSelectionCategory() {
  return QString(getSHGLRenderer().callMethod("", "selectionChangedFromManips", 0, 0).getStringCString());
}