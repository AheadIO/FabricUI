/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#include <FabricUI/SceneHub/Commands/SHCmd.h>
#include <FabricUI/SceneHub/Commands/SHAddSGObjectCmd.h>
#include <FabricUI/SceneHub/Commands/SHCmdHandler_QUndo.h>
#include <FabricUI/SceneHub/macros.h>
#include <FTL/OwnedPtr.h>
#include <FabricCore.h>
#include <FTL/JSONValue.h>
#include <FTL/JSONEnc.h>

using namespace FabricUI;
using namespace FabricUI::SceneHub;


/// \internal
/// Wrap a SHCmd into a Qt command
class SHCmdHandler_QUndo::WrappedCmd : public QUndoCommand {

  public:
    WrappedCmd( SHCmd *shCmd ) : QUndoCommand(), 
      m_shCmd( shCmd ),
      m_didit( false ) {};

  protected:
    virtual void redo() {
      try {
        if(m_didit) m_shCmd->redo();
        
        else {
          m_didit = true;
          m_shCmd->doit();
          QUndoCommand::setText( m_shCmd->getDesc().c_str() );
        }
      }
      catch (FabricCore::Exception e) {
        printf("Caught FabricCore::Exception: %s\n", e.getDesc_cstr() );
      }
    }

    virtual void undo() {
      try {
        assert(m_didit);
        m_shCmd->undo();
      }
      catch (FabricCore::Exception e) {
        printf( "Caught FabricCore::Exception: %s\n", e.getDesc_cstr() );
      }
    }

  private:
    FTL::OwnedPtr<SHCmd> m_shCmd;
    bool m_didit;
};
 
/// Encodes a rtVal into a Json, saves the rtVal
/// \param context The core context
/// \param rtVal The value to encode
std::string SHCmdHandler_QUndo::encodeRTValToJSON(FabricCore::Context const& context, FabricCore::RTVal const& rtVal) {
  if(rtVal.isValid())
  {
    if(rtVal.isObject())
    {
      if(!rtVal.isNullObject())
      {
        FabricCore::RTVal cast = FabricCore::RTVal::Construct(context, "RTValToJSONEncoder", 1, &rtVal);
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
/// \param context The core context
/// \param rtVal The result value
/// \param json The string to decode
void SHCmdHandler_QUndo::decodeRTValFromJSON(FabricCore::Context const& context, FabricCore::RTVal & rtVal, FTL::CStrRef json) {
  if(json.size() > 2)
  {
    try
    {
      if(rtVal.isValid())
      {
        if(rtVal.isObject())
        {
          if(rtVal.isNullObject())
            rtVal = FabricCore::RTVal::Create( context, rtVal.getTypeName().getStringCString(), 0, NULL );
          
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
            FabricCore::RTVal cast = FabricCore::RTVal::Construct(context, "RTValFromJSONDecoder", 1, &rtVal);
            if(cast.isInterface())
            {
              if(!cast.isNullObject())
              {
                FabricCore::RTVal data =
                  FabricCore::RTVal::ConstructString(
                    context,
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

/// Synchronizes the Qt and KL stacks.
void SHCmdHandler_QUndo::synchronize() {

  // m_qUndoStack->clear();
  
  // Get the number of commands already done in the KL stack
  FabricCore::RTVal sceneGraphVal = m_shObject.callMethod("SceneGraph", "getScene", 0, 0);
  FabricCore::RTVal cmdManagerVal = sceneGraphVal.callMethod("SGCmdManager", "getOrCreateHierarchyCmdManager", 0, 0);

  FabricCore::RTVal nbCommandsVal = cmdManagerVal.callMethod("Size", "getNumCmdInUndoStack", 0, 0);
  std::cerr << "\n\n\n\nNumber of commands in undo stack " <<  nbCommandsVal.getUInt32() << std::endl;

  for(uint32_t i=0; i<nbCommandsVal.getUInt32(); ++i)
  {
    FabricCore::RTVal indexVal = FabricCore::RTVal::ConstructUInt32(m_client, i);
    FabricCore::RTVal sgCmdVal = cmdManagerVal.callMethod("SGBaseCmd", "getCmdInUndoStack", 1, &indexVal);
    
    FabricCore::RTVal typeVal = sgCmdVal.callMethod("String", "type", 0, 0);
    std::string type = std::string(typeVal.getStringCString());

    if(type.compare("SGAddObjectCmd") == 0)
    {
      uint32_t index;
      std::string name, fullPath;
      bool isRestorePoint, isGlobal;

      SHAddSGObjectCmd::getParams(m_client, sgCmdVal, isRestorePoint, name, isGlobal, fullPath, index);
      //addSGObject(name, isGlobal, false);
    }
  }
}

/// Adds an object to the scene-graph
/// \param command The command as string
void SHCmdHandler_QUndo::addSGObject(const std::string &command) {
  SHCmd *cmd = SHAddSGObjectCmd::parseAndExec(m_client, m_shObject, command);
  if(cmd) m_qUndoStack->push( new WrappedCmd(cmd) );
  //synchronize();
}