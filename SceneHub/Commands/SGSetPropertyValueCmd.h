/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_SGSetPropertyValueCmd_H__
#define __UI_SCENEHUB_SGSetPropertyValueCmd_H__

#include "SHCmd.h"
#include <FabricUI/Util/StringUtils.h>

using namespace std;
using namespace FabricCore;
 
namespace FabricUI
{
  namespace SceneHub
  {
    const string SGSetPropertyValueCmd_Str = "setPropertyValueCmd";
    const string SGSetPropertyValueCmd_Type_Str = "SGSetPropertyValueCmd";

    class SGSetPropertyValueCmd : SHCmd
    {
      public:   
        /// Constructs and executes a command.
        /// \param shGLScene A reference to SHGLScene
        /// \param cmdDes The command desciprtion
        /// \param params The command parameters
        /// \param exec If true executes the command, just add it to the Qt stack otherwise
        SGSetPropertyValueCmd(SHGLScene *shGLScene, string cmdDes, vector<RTVal> &params, bool exec) :
          SHCmd(shGLScene, SGSetPropertyValueCmd_Str, cmdDes, params, exec) {};
     
        /// Adds an object to the scene-graph
        /// \param shGLScene A reference to SHGLScene
        /// \param command The command to create
        /// \param exec If true executes the command, just add it to the Qt stack otherwise
        static SHCmd* Create(SHGLScene *shGLScene, const string &command, bool exec) {
          SHCmd* cmd = 0;
          vector<string> params;
          uint32_t nbParams = 1;//4;
          if(SHCmd::ExtractParams(command, params) && params.size() == nbParams)
          {
            // Get the name of the object
            string fullPath = FabricUI::Util::RemoveSpace(params[0]); 
            //string type = FabricUI::Util::RemoveSpace(params[1]); 
            //string data = FabricUI::Util::RemoveSpace(params[2]); 
            //uint64_t dataSize = FabricUI::Util::ToNum<uint64_t>(params[3]); 
            try 
            {
              //RTVal val = SHCmd::SetParamValue(client, type.c_str(), data);
              vector<RTVal> params(nbParams);
              params[0] = RTVal::ConstructString(shGLScene->getClient(), fullPath.c_str());
              //params[1] = val.callMethod("Type", "type", 0, 0);
              //params[2] = val.callMethod("Data", "data", 0, 0);
              //params[3] = RTVal::ConstructUInt64(client, dataSize);
              cmd = new SGSetPropertyValueCmd(shGLScene, command, params, exec);
            }
            catch(FabricCore::Exception e)
            {
              printf("SGSetPropertyValueCmd::Create: exception: %s\n", e.getDesc_cstr());
            }
          }
          return cmd;
        }

        /// Gets the KL command parameters.
        /// \param shGLScene A reference to SHGLScene
        /// \param index The name of the object
        static string Get(SHGLScene *shGLScene, uint32_t index) {
          string res;
          try 
          {
            RTVal sgCmd = shGLScene->retrieveCmd(index);            
            RTVal keyVal = RTVal::ConstructString(shGLScene->getClient(), "fullPath");
            RTVal fullPathVal = sgCmd.callMethod("String", "getStringParam", 1, &keyVal);
            string fullPath = string(fullPathVal.getStringCString());

            //vector<RTVal> params(3);
            //params[0] = RTVal::ConstructUInt64(shGLScene->getClient(), 0);
            //params[1] = RTVal::ConstructString(shGLScene->getClient(), string("").c_str());
            //sgCmd.callMethod("", "getPropertyParam", 2, &params[0]);
            //uint64_t dataSize = params[0].getUInt64();
            //string type = string(params[1].getStringCString());
            //RTVal structVal = RTVal::Construct(shGLScene->getClient(), type.c_str(), 0, 0);
            //RTVal dataVal = structVal.callMethod("Data", "data", 0, 0);
            //sgCmd.callMethod("", "getPropertyData", 1, &dataVal);
            //string data = EncodeRTValToJSON(shGLScene->getClient(), structVal);
        
            res = string( SGSetPropertyValueCmd_Str + "(" + fullPath + ")" ); // ", " + type +  
              //", " + FabricUI::Util::RemoveSpace(FabricUI::Util::RemoveNewLine(data)) + ", " + FabricUI::Util::ToStr(dataSize) + ")" );
          }
          catch(FabricCore::Exception e)
          {
            printf("SGSetPropertyValueCmd::Get: exception: %s\n", e.getDesc_cstr());
          }
          return res;
        }     
    };

  }  
}

#endif // __UI_SCENEHUB_SGSetPropertyValueCmd_H__
