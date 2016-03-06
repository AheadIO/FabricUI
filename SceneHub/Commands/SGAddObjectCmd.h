/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_SGAddObjectCmd_H__
#define __UI_SCENEHUB_SGAddObjectCmd_H__

#include "SHCmd.h"
#include <FabricUI/Util/StringUtils.h>

using namespace std;
using namespace FabricCore;
 
namespace FabricUI
{
  namespace SceneHub
  {
    const string SGAddObjectCmd_Str = "addObjectCmd";
    const string SGAddObjectCmd_Type_Str = "SGAddObjectCmd";

    class SGAddObjectCmd : SHCmd
    {
      public:        
        /// Constructs and executes a command.
        /// \param shGLScene A reference to SHGLScene
        /// \param cmdDes The command desciprtion
        /// \param params The command parameters
        /// \param exec If true executes the command, just add it to the Qt stack otherwise
        SGAddObjectCmd(SHGLScene *shGLScene, string cmdDes, vector<RTVal> &params, bool exec) :
          SHCmd(shGLScene, SGAddObjectCmd_Str, cmdDes, params, exec) {};

        /// Adds an object to the scene-graph
        /// \param shGLScene A reference to SHGLScene
        /// \param command The command to create
        /// \param exec If true executes the command, just add it to the Qt stack otherwise
        static SHCmd* Create(SHGLScene *shGLScene, string command, bool exec) {
          SHCmd* cmd = 0;
          vector<string> params;
          if(SHCmd::ExtractParams(command, params) && params.size() == 2)
          {
            // Get the name of the object
            string name = FabricUI::Util::RemoveSpace(params[0]); 
            string isGlobalStr = FabricUI::Util::RemoveSpace(params[1]); 
            // Get if it a global object
            bool isGlobal = false;
            if(FabricUI::Util::IsNumber(isGlobalStr)) isGlobal = bool(FabricUI::Util::ToNum<int>(isGlobalStr));
            else isGlobal = (FabricUI::Util::ToLower(isGlobalStr).compare("true") == 0) ? true : false;   

            try 
            {
              vector<RTVal> params(2);
              params[0] = RTVal::ConstructString(shGLScene->getClient(), name.c_str());
              params[1] = RTVal::ConstructBoolean(shGLScene->getClient(), isGlobal);
              cmd = new SGAddObjectCmd(shGLScene, command, params, exec);
            }
            catch(Exception e)
            {
              printf("SGAddObjectCmd::Create: exception: %s\n", e.getDesc_cstr());
            }
          }
          return cmd;
        }
 
        /// Gets the KL command parameters.
        /// \param shGLScene A reference to SHGLScene
        /// \param index The name of the object
        static string Get(SHGLScene *shGLScene, uint32_t index) {
          string cmd;
          try 
          {
            RTVal sgCmd = shGLScene->retrieveCmd(index);
            RTVal keyVal = RTVal::ConstructString(shGLScene->getClient(), "name");
            RTVal nameVal = sgCmd.callMethod("String", "getStringParam", 1, &keyVal);
            string name = string(nameVal.getStringCString());

            keyVal = RTVal::ConstructString(shGLScene->getClient(), "isGlobal");
            RTVal isGlobalVal = sgCmd.callMethod("Boolean", "getBooleanParam", 1, &keyVal);
            bool isGlobal = isGlobalVal.getBoolean();
            cmd = string( SGAddObjectCmd_Str + "(" + name + ", " + FabricUI::Util::ToStr(isGlobal) + ")" );
          }
          catch(Exception e)
          {
            printf("SGAddObjectCmd::Get: exception: %s\n", e.getDesc_cstr());
          }
          return cmd;
        }
        
    };
  }  
}

#endif // __UI_SCENEHUB_SGAddObjectCmd_H__


