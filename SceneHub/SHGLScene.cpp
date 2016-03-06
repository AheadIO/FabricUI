/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include "SHGLScene.h"
#include <FabricUI/Viewports/QtToKLEvent.h>

using namespace FabricCore;
using namespace FabricUI::SceneHub;


SHGLScene::SHGLScene(Client *client, QString sceneName) 
  : m_client(client) 
{
  try 
  {
    m_shGLScene = RTVal::Create(*client, sceneName.toUtf8().constData(), 0, 0);
    m_shGLScene.callMethod("", "initializeSceneAndRTR", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLScene::SHGLScene: exception: %s\n", e.getDesc_cstr());
  }
}

SHGLScene::SHGLScene(FabricCore::Client *client, FabricCore::RTVal shGLScene) 
  : m_client(client)
{
  m_shGLScene = shGLScene;
}


bool SHGLScene::getSHGLScene(RTVal &shGLScene) {
  shGLScene = m_shGLScene;
  return true;
}

RTVal SHGLScene::getSG() {
  RTVal shGLScene, sceneVal;
  if(getSHGLScene(shGLScene))
  {
    try 
    {
      sceneVal = shGLScene.callMethod("SceneGraph", "getSG", 0, 0);
    }
    catch(Exception e)
    {
      printf("SHGLScene::getSG: exception: %s\n", e.getDesc_cstr());
    }
  }
  return sceneVal;
}

RTVal SHGLScene::getSceneRoot() {
  RTVal shGLScene, sceneRootVal;
  if(getSHGLScene(shGLScene))
  {
    try 
    {
      sceneRootVal = shGLScene.callMethod("SGObject", "getSceneRoot", 0, 0);
    }
    catch(Exception e)
    {
      printf("SHGLScene::getSceneRoot: exception: %s\n", e.getDesc_cstr());
    }
  }
  return sceneRootVal;
}

RTVal SHGLScene::getMaterialLibraryRoot() {
  RTVal shGLScene, libRootVal;
  if(getSHGLScene(shGLScene))
  {
    try 
    {
      libRootVal = shGLScene.callMethod("SGObject", "getMaterialLibraryRoot", 0, 0);
    }
    catch(Exception e)
    {
      printf("SHGLScene::getMaterialLibraryRoot: exception: %s\n", e.getDesc_cstr());
    }
  }
  return libRootVal;
}

RTVal SHGLScene::getImageLibraryRoot() {
  RTVal shGLScene, libRootVal;
  if(getSHGLScene(shGLScene))
  {
    try 
    {
      libRootVal = shGLScene.callMethod("SGObject", "getImageLibraryRoot", 0, 0);
    }
    catch(Exception e)
    {
      printf("SHGLScene::getImageLibraryRoot: exception: %s\n", e.getDesc_cstr());
    }
  }
  return libRootVal;
}

RTVal SHGLScene::getAssetLibraryRoot() {
  RTVal shGLScene, libRootVal;
  if(getSHGLScene(shGLScene))
  {
    try 
    {
      libRootVal = shGLScene.callMethod("SGObject", "getAssetLibraryRoot", 0, 0);
    }
    catch(Exception e)
    {
      printf("SHGLScene::getAssetLibraryRoot: exception: %s\n", e.getDesc_cstr());
    }
  }
  return libRootVal;
}

void SHGLScene::getSceneStats(uint32_t &mesh, uint32_t &triangle, uint32_t &light) {
  RTVal shGLScene;
  if(getSHGLScene(shGLScene))
  {
    try 
    {
      RTVal args[3] = {
        RTVal::ConstructUInt32(getClient(), 0),
        RTVal::ConstructUInt32(getClient(), 0),
        RTVal::ConstructUInt32(getClient(), 0)
      };
      shGLScene.callMethod("", "getSceneStats", 3, &args[0]);
      mesh = args[0].getUInt32();
      triangle = args[1].getUInt32();
      light = args[2].getUInt32();
    }
    catch(Exception e)
    {
      printf("SHGLScene::getSceneStats: exception: %s\n", e.getDesc_cstr());
    }
  }
}

void SHGLScene::prepareSceneForRender() {
  RTVal shGLScene;
  if(getSHGLScene(shGLScene))
  {
    try 
    {
      shGLScene.callMethod("", "prepareSceneForRender", 0, 0);
    }
    catch(Exception e)
    {
      printf("SHGLScene::prepareSceneForRender: exception: %s\n", e.getDesc_cstr());
    }
  }
}

bool SHGLScene::playbackByDefault() {
  RTVal shGLScene;
  if(getSHGLScene(shGLScene))
  {
    try 
    {
      return shGLScene.callMethod("Boolean", "playbackByDefault", 0, 0).getBoolean();
    }
    catch(Exception e)
    {
      printf("SHGLScene::playbackByDefault: exception: %s\n", e.getDesc_cstr());
    }
  }
  return false;
}

void SHGLScene::getInitialTimelineState(
  bool &enable, 
  int &startFrame, 
  int &endFrame, 
  float &fps) 
{
  RTVal shGLScene;
  if(getSHGLScene(shGLScene))
  {
    try 
    {
      RTVal args[4] = {
        RTVal::ConstructBoolean(getClient(), 0),
        RTVal::ConstructSInt32(getClient(), 0),
        RTVal::ConstructSInt32(getClient(), 0),
        RTVal::ConstructFloat32(getClient(), 0)
      };
      shGLScene.callMethod("", "getInitialTimelineState", 4, &args[0]);
      enable = args[0].getBoolean();
      startFrame = args[1].getSInt32();
      endFrame = args[2].getSInt32();
      fps = args[3].getFloat32();
    }
    catch(Exception e)
    {
      printf("SHGLScene::getInitialTimelineState: exception: %s\n", e.getDesc_cstr());
    }
  }
}

void SHGLScene::setFrame(int frame) {
  RTVal shGLScene;
  if(getSHGLScene(shGLScene))
  {
    try 
    {
      RTVal arg = RTVal::ConstructSInt32(getClient(), frame);    
      shGLScene.callMethod("", "setFrame", 1, &arg);
    }
    catch(Exception e)
    {
      printf("SHGLScene::setOrthographicViewport: exception: %s\n", e.getDesc_cstr());
    }
  }
}

bool SHGLScene::sceneHierarchyChanged() {
  RTVal shGLScene;
  if(getSHGLScene(shGLScene))
  {
    try 
    {
      return shGLScene.callMethod("Boolean", "sceneHierarchyChanged", 0, 0).getBoolean();
    }
    catch(Exception e)
    {
      printf("SHGLScene::setOrthographicViewport: sceneHierarchyChanged: %s\n", e.getDesc_cstr());
    }
  }
  return false;
}

RTVal SHGLScene::getSelectedObjects() {
  RTVal shGLScene, selectedObject;
  if(getSHGLScene(shGLScene))
  {
    try 
    {
      selectedObject = shGLScene.callMethod("SGObject[]", "getSelectedObjects", 0, 0);
    }
    catch(Exception e)
    {
      printf("SHGLScene::setPlayback: exception: %s\n", e.getDesc_cstr());
    }
  }
  return selectedObject;
}

void SHGLScene::sceneItemSelected(RTVal obj) {
  RTVal shGLScene;
  if(getSHGLScene(shGLScene))
  {
    try 
    {
      shGLScene.callMethod("", "sceneItemSelected", 1, &obj);
    }
    catch(Exception e)
    {
      printf("SHGLScene::sceneItemSelected: exception: %s\n", e.getDesc_cstr());
    }
  }
}

void SHGLScene::treeItemSelected(RTVal obj) {
  RTVal shGLScene;
  if(getSHGLScene(shGLScene))
  {
    try 
    {
      shGLScene.callMethod("", "treeItemSelected", 1, &obj);
    }
    catch(Exception e)
    {
      printf("SHGLScene::treeItemSelected: exception: %s\n", e.getDesc_cstr());
    }
  }
}

void SHGLScene::treeItemDeselected(RTVal obj) {
  RTVal shGLScene;
  if(getSHGLScene(shGLScene))
  {
    try 
    {
      shGLScene.callMethod("", "treeItemDeselected", 1, &obj);
    }
    catch(Exception e)
    {
      printf("SHGLScene::treeItemDeselected: exception: %s\n", e.getDesc_cstr());
    }
  }
}

bool SHGLScene::showTreeViewByDefault(uint32_t &level) {
  RTVal shGLScene;
  bool showTreeViewByDefault;
  if(getSHGLScene(shGLScene))
  {
    try 
    {
      RTVal levelArg = RTVal::ConstructUInt32(getClient(), 0);
      showTreeViewByDefault = shGLScene.callMethod( "Boolean", "showTreeViewByDefault", 1, &levelArg).getBoolean();
      level = levelArg.getUInt32();
      return showTreeViewByDefault;
    }
    catch(Exception e)
    {
      printf("SHGLScene::treeItemDeselected: exception: %s\n", e.getDesc_cstr());
    }
  }
  return false;
}

void SHGLScene::addExternalFileList(QStringList pathList, float pos[3], bool forceExpand) {
  RTVal shGLScene;
  if(getSHGLScene(shGLScene))
  {
    try 
    {
      RTVal klPathList = RTVal::ConstructVariableArray(getClient(), "String");
      klPathList.setArraySize(pathList.size());
      for(int i=0; i<pathList.size(); ++i) 
        klPathList.setArrayElement(i, RTVal::ConstructString(getClient(), pathList[i].toUtf8().constData()));
  
      RTVal posVal = RTVal::Construct(getClient(), "Vec3", 0, 0);
      posVal.setMember("x", RTVal::ConstructFloat32(getClient(), pos[0]));
      posVal.setMember("y", RTVal::ConstructFloat32(getClient(), pos[1]));
      posVal.setMember("z", RTVal::ConstructFloat32(getClient(), pos[2]));

      RTVal args[3] = {
        klPathList,
        posVal,
        RTVal::ConstructBoolean(getClient(), forceExpand)
      };
      shGLScene.callMethod("", "addExternalFileList", 3, &args[0]);
    }
    catch(Exception e)
    {
      printf("SHGLScene::addExternalFileList: exception: %s\n", e.getDesc_cstr());
    }
  }
}

void SHGLScene::setObjectColor(float color[4], bool local) {
  RTVal shGLScene;
  if(getSHGLScene(shGLScene))
  {
    try 
    {
      RTVal colorVal = RTVal::Construct(getClient(), "Color", 0, 0);
      colorVal.setMember("r", RTVal::ConstructFloat32(getClient(), color[0]));
      colorVal.setMember("g", RTVal::ConstructFloat32(getClient(), color[1]));
      colorVal.setMember("b", RTVal::ConstructFloat32(getClient(), color[2]));
      colorVal.setMember("a", RTVal::ConstructFloat32(getClient(), color[3]));

      RTVal args[2] = {
        colorVal,
        RTVal::ConstructBoolean(getClient(), local)
      };
      shGLScene.callMethod("", "setObjectColor", 2, &args[0]);
    }
    catch(Exception e)
    {
      printf("SHGLScene::setObjectColor: exception: %s\n", e.getDesc_cstr());
    }
  }
}

void SHGLScene::addLight(uint32_t lightType, float pos[3]) {
  RTVal shGLScene;
  if(getSHGLScene(shGLScene))
  {
    try 
    {
     RTVal posVal = RTVal::Construct(getClient(), "Vec3", 0, 0);
      posVal.setMember("x", RTVal::ConstructFloat32(getClient(), pos[0]));
      posVal.setMember("y", RTVal::ConstructFloat32(getClient(), pos[1]));
      posVal.setMember("z", RTVal::ConstructFloat32(getClient(), pos[2]));

      RTVal args[2] = {
        RTVal::ConstructUInt32(getClient(), lightType),
        posVal
      };
      shGLScene.callMethod("", "addLight", 2, &args[0]);
    }
    catch(Exception e)
    {
      printf("SHGLScene::addLight: exception: %s\n", e.getDesc_cstr());
    }
  }
}

void SHGLScene::setlightProperties(float color[4], float intensity) {
  RTVal shGLScene;
  if(getSHGLScene(shGLScene))
  {
    try 
    {
      RTVal colorVal = RTVal::Construct(getClient(), "Color", 0, 0);
      colorVal.setMember("r", RTVal::ConstructFloat32(getClient(), color[0]));
      colorVal.setMember("g", RTVal::ConstructFloat32(getClient(), color[1]));
      colorVal.setMember("b", RTVal::ConstructFloat32(getClient(), color[2]));
      colorVal.setMember("a", RTVal::ConstructFloat32(getClient(), color[3]));

      RTVal args[2] = {
        colorVal,
        RTVal::ConstructFloat32(getClient(), intensity)
      };
      shGLScene.callMethod("", "setlightProperties", 2, &args[0]);
    }
    catch(Exception e)
    {
      printf("SHGLScene::setlightProperties: exception: %s\n", e.getDesc_cstr());
    }
  }
}

void SHGLScene::exportToAlembic(QString filePath) {
  RTVal shGLScene;
  if(getSHGLScene(shGLScene))
  {
    try 
    {
      RTVal filePathVal = RTVal::ConstructString(getClient(), filePath.toUtf8().constData());
      shGLScene.callMethod("", "exportToAlembic", 1, &filePathVal);
    }
    catch(Exception e)
    {
      printf("SHGLScene::exportToAlembic: exception: %s\n", e.getDesc_cstr());
    }
  }
}

bool SHGLScene::getCmdManager(RTVal &cmdManager) {
  RTVal shGLScene;
  if(getSHGLScene(shGLScene))
  {
    try 
    {
      cmdManager = shGLScene.callMethod("CmdManager", "getOrCreateCmdManager", 0, 0);
      return (cmdManager.isValid() && !cmdManager.isNullObject());
    }
    catch(Exception e)
    {
      printf("SHGLScene::getCmdManager: exception: %s\n", e.getDesc_cstr());
    }
  }
  return false;
}

RTVal SHGLScene::setParamValue(std::string type, std::string value) {
  RTVal cmdManager, paramsVal;
  if(getCmdManager(cmdManager))
  {
    try 
    {
      paramsVal = RTVal::Construct(getClient(), type.c_str(), 0, 0);
      DecodeRTValFromJSON(getClient(), paramsVal, value.c_str());
    }
    catch(Exception e)
    {
      printf("SHGLScene::setParamValue: exception: %s\n", e.getDesc_cstr());
    }
  } 
  return paramsVal;
}

RTVal SHGLScene::retrieveCmd(uint32_t index) {
  RTVal cmdManager, cmdVal;
  if(getCmdManager(cmdManager))
  {
    try 
    {
      RTVal indexVal = RTVal::ConstructUInt32(getClient(), index);
      cmdVal = cmdManager.callMethod("SGCmd", "getCmdInUndoStack", 1, &indexVal);
    }
    catch(Exception e)
    {
      printf("SHGLScene::retrieveCmd: exception: %s\n", e.getDesc_cstr());
    }
  }
  return cmdVal;
}

uint32_t SHGLScene::getNumCmdInUndoStack() {
  RTVal cmdManager;
  uint32_t nbCommands = 0;
  if(getCmdManager(cmdManager))
  {
    try 
    {
      nbCommands = cmdManager.callMethod("Size", "getNumCmdInUndoStack", 0, 0).getUInt32();
    }
    catch(Exception e)
    {
      printf("SHGLScene::getNumCmdInUndoStack: exception: %s\n", e.getDesc_cstr());
    }
  }
  return nbCommands;
}

void SHGLScene::executeCmd(std::string cmdName, std::vector<RTVal> &params)  {
  RTVal cmdManager;
  if(getCmdManager(cmdManager))
  {
    try 
    {
      getSG().callMethod("", cmdName.c_str(), params.size(), &params[0]);
    }
    catch(Exception e)
    {
      printf("SHGLScene::executeCmd: exception: %s\n", e.getDesc_cstr());
    }
  }
}

void SHGLScene::undoCmd(uint32_t undoCount) {
  RTVal cmdManager;
  if(getCmdManager(cmdManager))
  {
    try 
    {
      for(unsigned i=0; i<undoCount; ++i)
        cmdManager.callMethod("Boolean", "undo", 0, 0);
    }
    catch(Exception e)
    {
      printf("SHGLScene::undoCmd: exception: %s\n", e.getDesc_cstr());
    }
  }
}

void SHGLScene::redoCmd(uint32_t redoCount) {
  RTVal cmdManager;
  if(getCmdManager(cmdManager))
  {
    try 
    {
      for(unsigned i=0; i<redoCount; ++i)
        cmdManager.callMethod("Boolean", "redo", 0, 0);
    }
    catch(Exception e)
    {
      printf("SHGLScene::redoCmd: exception: %s\n", e.getDesc_cstr());
    }
  }
}

// ****************
/// Encodes a rtVal into a Json, saves the rtVal
/// \param client The core client
/// \param rtVal The value to encode
std::string SHGLScene::EncodeRTValToJSON(FabricCore::Client client, FabricCore::RTVal rtVal) {
  if(rtVal.isValid())
  {
    if(rtVal.isObject())
    {
      if(!rtVal.isNullObject())
      {
        FabricCore::RTVal cast = FabricCore::RTVal::Construct(client, "RTValToJSONEncoder", 1, &rtVal);
        if(cast.isValid())
        {
          if(!cast.isNullObject())
          {
            FabricCore::RTVal result = cast.callMethod("String", "convertToString", 0, 0);
            if(result.isValid())
            {
              FTL::CStrRef ref = result.getStringCString();
              if(ref.size() > 0)
              {
                std::string json;
                {
                  FTL::JSONEnc<> jsonEnc( json );
                  FTL::JSONStringEnc<> jsonStringEnc( jsonEnc, ref );
                }
                return json;
              }
            }
          }
        }
      }
    }
  }
  FabricCore::RTVal valueJSON = rtVal.getJSON();
  return valueJSON.getStringCString();
}

/// Decodes a rtVal from a Json, reads the rtVal
/// \param client The core client
/// \param rtVal The result value
/// \param json The string to decode
void SHGLScene::DecodeRTValFromJSON(FabricCore::Client client, FabricCore::RTVal & rtVal, FTL::CStrRef json) {

  if(json.size() > 2)
  {
    try
    {
      if(rtVal.isValid())
      {
        if(rtVal.isObject())
        {
          if(rtVal.isNullObject())
            rtVal = FabricCore::RTVal::Create( client, rtVal.getTypeName().getStringCString(), 0, NULL );
          
          std::string decodedString;
          {
            FTL::JSONStrWithLoc strWithLoc( json );
            FTL::JSONDec jsonDec( strWithLoc );
            FTL::JSONEnt jsonEnt;
            if ( jsonDec.getNext( jsonEnt ) && jsonEnt.getType() == jsonEnt.Type_String )
              jsonEnt.stringAppendTo( decodedString );
          }

          if(decodedString.length() > 0)
          {
            FabricCore::RTVal cast = FabricCore::RTVal::Construct(client, "RTValFromJSONDecoder", 1, &rtVal);
            if(cast.isInterface())
            {
              if(!cast.isNullObject())
              {
                FabricCore::RTVal data =
                  FabricCore::RTVal::ConstructString(
                    client,
                    decodedString.data(),
                    decodedString.size()
                    );
                FabricCore::RTVal result = cast.callMethod("Boolean", "convertFromString", 1, &data);
                if(result.isValid())
                {
                  if(result.getBoolean())
                  {
                    return;
                  }
                }
              }
            }
          }
        }
      }
    }
    catch(FabricCore::Exception e)
    {
      printf("decodeRTValFromJSON: Hit exception: %s\n", e.getDesc_cstr());
    }
  }

  rtVal.setJSON( json.c_str() );
}
