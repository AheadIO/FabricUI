/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include <iostream>
#include <FabricUI/SceneHub/Commands/SHCmd.h>
#include <FTL/OwnedPtr.h>
#include <FabricCore.h>
#include <FTL/JSONValue.h>
#include <FTL/JSONEnc.h>
#include <QtGui/QMessageBox>

using namespace FabricUI;
using namespace FabricUI::SceneHub;


/// Encodes a rtVal into a Json, saves the rtVal
/// \param client The core client
/// \param rtVal The value to encode
std::string SHCmd::EncodeRTValToJSON(FabricCore::Client const& client, FabricCore::RTVal const& rtVal) {
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
void SHCmd::DecodeRTValFromJSON(FabricCore::Client const& client, FabricCore::RTVal & rtVal, FTL::CStrRef json) {

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

/// Extracts the parameters from the command string.
/// \param client The core client
/// \param command The command
/// \param params The array of parameters as string
bool SHCmd::ExtractParams(const std::string &command, std::vector<std::string> &params) {
  std::vector<std::string> split = Split(command, '(');
  if(split.size() == 2)
  {
    std::vector<std::string> split_ = Split(split[1], ')');
    if(split_.size() == 2)
    {             
      std::string paramArray = RemoveSpace(split_[0]);

      // Parameter like {...,...,...}
      size_t begin = paramArray.find("{");
      size_t end = paramArray.find_last_of("}");
    
      if(begin < end)
      {
        // Get the index to insert the {} params
        std::string temp = paramArray.substr(0, begin);
        size_t index = std::count(temp.begin(), temp.end(), ',');

        // Get the {} params string
        std::string params_ = paramArray.substr(begin, end-begin+1);
        // Get the {} params string
        size_t index2 = paramArray.substr(end, paramArray.length()-1).find(",");
        paramArray.erase(begin, end+index2+1-begin);
        
        // Construct the vector of parameters
        std::vector<std::string> tempArray = Split(paramArray, ',');
        for(uint32_t i=0; i<index; ++i) params.push_back(tempArray[i]);
        params.push_back(params_);
        for(uint32_t i=index; i<tempArray.size(); ++i) params.push_back(tempArray[i]);
      }
      else params = Split(paramArray, ',');

      return true;
    }
  }
  return false;
}

/// From the parameter type and its value, creates 
/// \param client The core client
/// \param type The parameter type
/// \param value The parameter value JSon encoded
FabricCore::RTVal SHCmd::SetParamValue(FabricCore::Client const& client, std::string const& type, std::string const& value) {
  FABRIC_TRY_RETURN("SHCmd::SetParamValue", false,
    FabricCore::RTVal rtVal = FabricCore::RTVal::Construct(client, type.c_str(), 0, 0);
    DecodeRTValFromJSON(client, rtVal, value.c_str());
    return rtVal;
  );
}

/// Extracts the name of a command.
/// \param command The command
/// \param name The command's name
bool SHCmd::ExtractName(const std::string &command, std::string &name) {
  std::vector<std::string> split = FabricUI::SceneHub::Split(command, '(');
  if(split.size() > 1) {
    name = split[0];
    return true;
  }
  return false;
}

/// Gets the command manager.
/// \param shObject A reference to SceneHub application
FabricCore::RTVal SHCmd::GetCmdManager(FabricCore::RTVal &shObject) {
  FABRIC_TRY_RETURN("SHCmd::GetCmdManager", false,
    FabricCore::RTVal sceneGraph = shObject.callMethod("SceneGraph", "getScene", 0, 0);
    if( sceneGraph.isNullObject() )
      return FabricCore::RTVal();
    return sceneGraph.callMethod("SGCmdManager", "getOrCreateHierarchyCmdManager", 0, 0);
  );
}

/// Gets the command at index i of KL stack.
/// \param client A reference to the fabric client
/// \param shObject A reference to SceneHub application
/// \param index The name of the object
FabricCore::RTVal SHCmd::RetrieveCmd(FabricCore::Client &client, FabricCore::RTVal &shObject, uint32_t index) {
  FABRIC_TRY_RETURN("SHCmd::RetrieveCmd", false,
    FabricCore::RTVal indexVal = FabricCore::RTVal::ConstructUInt32(client, index);
    return GetCmdManager(shObject).callMethod("SGCmd", "getCmdInUndoStack", 1, &indexVal);
  );
}


// ************

/// Constructs and executes a command.
/// \param shObject A reference to SceneHub application
/// \param cmdName The name of the command
/// \param cmdDes The command desciprtion
/// \param params The command parameters
/// \param exec If true executes the command, just add it to the Qt stack otherwise
SHCmd::SHCmd(
  FabricCore::RTVal &shObject,
  const std::string &cmdName, 
  const std::string &cmdDes, 
  std::vector<FabricCore::RTVal> &params, 
  bool exec) : 
  m_state(State_New),
  m_coreUndoCount(0), 
  m_shObject(shObject) 
{
  FABRIC_TRY("SHCmd::Constructor",
    setDesc(cmdDes);
    FabricCore::RTVal sceneGraph = m_shObject.callMethod("SceneGraph", "getScene", 0, 0);
    if(exec) sceneGraph.callMethod("", cmdName.c_str(), params.size(), &params[0]);
  );
}

/// Does nothing (don't call the command in KL).
void SHCmd::doit() {
  assert(m_state == State_New);
  ++m_coreUndoCount;
  m_state = State_Done;
}

/// Undoes the command.
void SHCmd::undo() {
  assert(m_state == State_Done || m_state == State_Redone);
  m_state = State_Undone;
  FABRIC_TRY("SHCmd::undo", 
    for(unsigned i=0; i<m_coreUndoCount; ++i)
      GetCmdManager(m_shObject).callMethod("Boolean", "undo", 0, 0);
  );
}

/// Redoes the command.
void SHCmd::redo() {
  assert( m_state = State_Undone );
  m_state = State_Redone;          
  FABRIC_TRY("SHCmd::redo", 
    for(unsigned i=0; i<m_coreUndoCount; ++i)
      GetCmdManager(m_shObject).callMethod("Boolean", "redo", 0, 0);
  );
}
