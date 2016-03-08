/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include "SHGLScene.h"
#include <FabricUI/Viewports/QtToKLEvent.h>

using namespace FabricCore;
using namespace FabricUI::SceneHub;


SHGLScene::SHGLScene(Client client, QString sceneName) 
  : SHGLScene(client) 
{
  try 
  {
    m_shGLSceneVal = RTVal::Create(client, sceneName.toUtf8().constData(), 0, 0);
    m_shGLSceneVal.callMethod("", "initializeSceneAndRTR", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLScene::SHGLScene: exception: %s\n", e.getDesc_cstr());
  }
}

SHGLScene::SHGLScene(FabricCore::Client client, FabricCore::RTVal shGLScene) 
  : SHGLScene(client)
{
  m_shGLSceneVal = shGLScene;
}

RTVal SHGLScene::getSG() {
  RTVal sceneVal;
  try 
  {
    sceneVal = m_shGLSceneVal.callMethod("SceneGraph", "getSG", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLScene::getSG: exception: %s\n", e.getDesc_cstr());
  }
  return sceneVal;
}

RTVal SHGLScene::getSceneRoot() {
  RTVal sceneRootVal;
  try 
  {
    sceneRootVal = m_shGLSceneVal.callMethod("SGObject", "getSceneRoot", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLScene::getSceneRoot: exception: %s\n", e.getDesc_cstr());
  }
  return sceneRootVal;
}

RTVal SHGLScene::getMaterialLibraryRoot() {
  RTVal libRootVal;
  try 
  {
    libRootVal = m_shGLSceneVal.callMethod("SGObject", "getMaterialLibraryRoot", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLScene::getMaterialLibraryRoot: exception: %s\n", e.getDesc_cstr());
  }
  return libRootVal;
}

RTVal SHGLScene::getImageLibraryRoot() {
  RTVal libRootVal;
  try 
  {
    libRootVal = m_shGLSceneVal.callMethod("SGObject", "getImageLibraryRoot", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLScene::getImageLibraryRoot: exception: %s\n", e.getDesc_cstr());
  }
  return libRootVal;
}

RTVal SHGLScene::getAssetLibraryRoot() {
  RTVal libRootVal;
  try 
  {
    libRootVal = m_shGLSceneVal.callMethod("SGObject", "getAssetLibraryRoot", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLScene::getAssetLibraryRoot: exception: %s\n", e.getDesc_cstr());
  }
  return libRootVal;
}

void SHGLScene::getSceneStats(uint32_t &mesh, uint32_t &triangle, uint32_t &light) {
  try 
  {
    RTVal args[3] = {
      RTVal::ConstructUInt32(getClient(), 0),
      RTVal::ConstructUInt32(getClient(), 0),
      RTVal::ConstructUInt32(getClient(), 0)
    };
    m_shGLSceneVal.callMethod("", "getSceneStats", 3, &args[0]);
    mesh = args[0].getUInt32();
    triangle = args[1].getUInt32();
    light = args[2].getUInt32();
  }
  catch(Exception e)
  {
    printf("SHGLScene::getSceneStats: exception: %s\n", e.getDesc_cstr());
  }
}

void SHGLScene::prepareSceneForRender() {
  try 
  {
    m_shGLSceneVal.callMethod("", "prepareSceneForRender", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLScene::prepareSceneForRender: exception: %s\n", e.getDesc_cstr());
  }
}

bool SHGLScene::playbackByDefault() {
  try 
  {
    return m_shGLSceneVal.callMethod("Boolean", "playbackByDefault", 0, 0).getBoolean();
  }
  catch(Exception e)
  {
    printf("SHGLScene::playbackByDefault: exception: %s\n", e.getDesc_cstr());
  }
  return false;
}

void SHGLScene::getInitialTimelineState(bool &enable, int &start, int &end, float &fps) {
  try 
  {
    RTVal args[4] = {
      RTVal::ConstructBoolean(getClient(), 0),
      RTVal::ConstructSInt32(getClient(), 0),
      RTVal::ConstructSInt32(getClient(), 0),
      RTVal::ConstructFloat32(getClient(), 0)
    };
    m_shGLSceneVal.callMethod("", "getInitialTimelineState", 4, &args[0]);
    enable = args[0].getBoolean();
    start = args[1].getSInt32();
    end = args[2].getSInt32();
    fps = args[3].getFloat32();
  }
  catch(Exception e)
  {
    printf("SHGLScene::getInitialTimelineState: exception: %s\n", e.getDesc_cstr());
  }
}

void SHGLScene::setFrame(int frame) {
  try 
  {
    RTVal arg = RTVal::ConstructSInt32(getClient(), frame);    
    m_shGLSceneVal.callMethod("", "setFrame", 1, &arg);
  }
  catch(Exception e)
  {
    printf("SHGLScene::setOrthographicViewport: exception: %s\n", e.getDesc_cstr());
  }
}

bool SHGLScene::sceneHierarchyChanged() {
  try 
  {
    return m_shGLSceneVal.callMethod("Boolean", "sceneHierarchyChanged", 0, 0).getBoolean();
  }
  catch(Exception e)
  {
    printf("SHGLScene::setOrthographicViewport: sceneHierarchyChanged: %s\n", e.getDesc_cstr());
  }
  return false;
}

RTVal SHGLScene::getSelectedObjects() {
  RTVal selectedObject;
  try 
  {
    selectedObject = m_shGLSceneVal.callMethod("SGObject[]", "getSelectedObjects", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLScene::setPlayback: exception: %s\n", e.getDesc_cstr());
  }
  return selectedObject;
}

void SHGLScene::sceneItemSelected(RTVal obj) {
  try 
  {
    m_shGLSceneVal.callMethod("", "sceneItemSelected", 1, &obj);
  }
  catch(Exception e)
  {
    printf("SHGLScene::sceneItemSelected: exception: %s\n", e.getDesc_cstr());
  }
}

void SHGLScene::treeItemSelected(RTVal obj) {
  try 
  {
    m_shGLSceneVal.callMethod("", "treeItemSelected", 1, &obj);
  }
  catch(Exception e)
  {
    printf("SHGLScene::treeItemSelected: exception: %s\n", e.getDesc_cstr());
  }
}

void SHGLScene::treeItemDeselected(RTVal obj) {
  try 
  {
    m_shGLSceneVal.callMethod("", "treeItemDeselected", 1, &obj);
  }
  catch(Exception e)
  {
    printf("SHGLScene::treeItemDeselected: exception: %s\n", e.getDesc_cstr());
  }
}

bool SHGLScene::selectionChangedFromManips() {
  try 
  {
    return m_shGLSceneVal.callMethod("Boolean", "selectionChangedFromManips", 0, 0).getBoolean();
  }
  catch(Exception e)
  {
    printf("SHGLScene::selectionChangedFromManips: exception: %s\n", e.getDesc_cstr());
  }
  return false;
}

QString SHGLScene::getSelectionCategory() {
  try 
  {
    return QString(m_shGLSceneVal.callMethod("String", "getSelectionCategory", 0, 0).getStringCString());
  }
  catch(Exception e)
  {
    printf("SHGLScene::getSelectionCategory: exception: %s\n", e.getDesc_cstr());
  }
  return QString();
}

bool SHGLScene::showTreeViewByDefault(uint32_t &level) {
  bool show;
  try 
  {
    RTVal levelVal = RTVal::ConstructUInt32(getClient(), level); 
    show = m_shGLSceneVal.callMethod("Boolean", "showTreeViewByDefault", 1, &levelVal).getBoolean();
    level = levelVal.getUInt32();
  }
  catch(Exception e)
  {
    printf("SHGLScene::getSelectionCategory: exception: %s\n", e.getDesc_cstr());
  }
  return show;
}


void SHGLScene::addExternalFileList(QStringList pathList, float pos[3], bool forceExpand) {
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
    m_shGLSceneVal.callMethod("", "addExternalFileList", 3, &args[0]);
  }
  catch(Exception e)
  {
    printf("SHGLScene::addExternalFileList: exception: %s\n", e.getDesc_cstr());
  }
}

void SHGLScene::setObjectColor(float color[4], bool local) {
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
    m_shGLSceneVal.callMethod("", "setObjectColor", 2, &args[0]);
  }
  catch(Exception e)
  {
    printf("SHGLScene::setObjectColor: exception: %s\n", e.getDesc_cstr());
  }
}

void SHGLScene::addLight(uint32_t lightType, float pos[3]) {
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
    m_shGLSceneVal.callMethod("", "addLight", 2, &args[0]);
  }
  catch(Exception e)
  {
    printf("SHGLScene::addLight: exception: %s\n", e.getDesc_cstr());
  }
}

void SHGLScene::setlightProperties(float color[4], float intensity) {
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
    m_shGLSceneVal.callMethod("", "setlightProperties", 2, &args[0]);
  }
  catch(Exception e)
  {
    printf("SHGLScene::setlightProperties: exception: %s\n", e.getDesc_cstr());
  }
}

void SHGLScene::exportToAlembic(QString filePath) {
  try 
  {
    RTVal filePathVal = RTVal::ConstructString(getClient(), filePath.toUtf8().constData());
    m_shGLSceneVal.callMethod("", "exportToAlembic", 1, &filePathVal);
  }
  catch(Exception e)
  {
    printf("SHGLScene::exportToAlembic: exception: %s\n", e.getDesc_cstr());
  }
}

RTVal SHGLScene::getCmdManager() {
  RTVal cmdManager;
  try 
  {
    cmdManager = m_shGLSceneVal.callMethod("CmdManager", "getOrCreateCmdManager", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLScene::getCmdManager: exception: %s\n", e.getDesc_cstr());
  }
  return cmdManager;
}

RTVal SHGLScene::setParamValue(std::string type, std::string value) {
  RTVal cmdManager, paramsVal;
  try 
  {
    paramsVal = RTVal::Construct(getClient(), type.c_str(), 0, 0);
    DecodeRTValFromJSON(getClient(), paramsVal, value.c_str());
  }
  catch(Exception e)
  {
    printf("SHGLScene::setParamValue: exception: %s\n", e.getDesc_cstr());
  }
  return paramsVal;
}

RTVal SHGLScene::retrieveCmd(uint32_t index) {
  RTVal cmdVal;
  try 
  {
    RTVal indexVal = RTVal::ConstructUInt32(getClient(), index);
    cmdVal = getCmdManager().callMethod("SGCmd", "getCmdInUndoStack", 1, &indexVal);
  }
  catch(Exception e)
  {
    printf("SHGLScene::retrieveCmd: exception: %s\n", e.getDesc_cstr());
  }
  return cmdVal;
}

uint32_t SHGLScene::getNumCmdInUndoStack() {
  try 
  {
    return getCmdManager().callMethod("Size", "getNumCmdInUndoStack", 0, 0).getUInt32();
  }
  catch(Exception e)
  {
    printf("SHGLScene::getNumCmdInUndoStack: exception: %s\n", e.getDesc_cstr());
  }
  return 0;
}

void SHGLScene::executeCmd(std::string cmdName, std::vector<RTVal> &params)  {
  try 
  {
    getSG().callMethod("", cmdName.c_str(), params.size(), &params[0]);
  }
  catch(Exception e)
  {
    printf("SHGLScene::executeCmd: exception: %s\n", e.getDesc_cstr());
  }
}

void SHGLScene::undoCmd(uint32_t undoCount) {
  try 
  {
    for(unsigned i=0; i<undoCount; ++i)
      getCmdManager().callMethod("Boolean", "undo", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLScene::undoCmd: exception: %s\n", e.getDesc_cstr());
  }
}

void SHGLScene::redoCmd(uint32_t redoCount) {
  try 
  {
    for(unsigned i=0; i<redoCount; ++i)
      getCmdManager().callMethod("Boolean", "redo", 0, 0);
  }
  catch(Exception e)
  {
    printf("SHGLScene::redoCmd: exception: %s\n", e.getDesc_cstr());
  }
}


// ****************
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
