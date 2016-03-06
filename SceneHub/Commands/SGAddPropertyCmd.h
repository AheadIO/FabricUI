/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_SGAddPropertyCmd_H__
#define __UI_SCENEHUB_SGAddPropertyCmd_H__
 
#include "SHCmd.h"
#include <FabricUI/Util/StringUtils.h>

using namespace std;
using namespace FabricCore;
 

namespace FabricUI
{
  namespace SceneHub
  {
    const string SGAddPropertyCmd_Str = "addPropertyCmd";
    const string SGAddPropertyCmd_Type_Str = "SGAddPropertyCmd";

    class SGAddPropertyCmd : SHCmd
    {
      public:        
        /// Constructs and executes a command.
        /// \param shGLScene A reference to SHGLScene
        /// \param cmdDes The command desciprtion
        /// \param params The command parameters
        /// \param exec If true executes the command, just add it to the Qt stack otherwise
        SGAddPropertyCmd(SHGLScene *shGLScene, string cmdDes, vector<RTVal> &params, bool exec) :
          SHCmd(shGLScene, SGAddPropertyCmd_Str, cmdDes, params, exec) {};
        
        /// Adds an object to the scene-graph
        /// \param shGLScene A reference to SHGLScene
        /// \param command The command to create
        /// \param exec If true executes the command, just add it to the Qt stack otherwise
        static SHCmd* Create(SHGLScene *shGLScene, const string &command, bool exec) {
          SHCmd* cmd = 0;
          vector<string> params;
          if(SHCmd::ExtractParams(command, params) && params.size() == 2)
          {
            string ownerPath = FabricUI::Util::RemoveSpace(params[0]); 
            string name = FabricUI::Util::RemoveSpace(params[1]); 
            try 
            {
              vector<RTVal> params(2);
              params[0] = RTVal::ConstructString(shGLScene->getClient(), ownerPath.c_str());
              params[1] = RTVal::ConstructString(shGLScene->getClient(), name.c_str());
              cmd = new SGAddPropertyCmd(shGLScene, command, params, exec);
            }
            catch(Exception e)
            {
              printf("SGAddPropertyCmd::Create: exception: %s\n", e.getDesc_cstr());
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
            RTVal keyVal = RTVal::ConstructString(shGLScene->getClient(), "ownerPath");
            RTVal nameVal = sgCmd.callMethod("String", "getStringParam", 1, &keyVal);
            string ownerPath = string(nameVal.getStringCString());

            keyVal = RTVal::ConstructString(shGLScene->getClient(), "name");
            nameVal = sgCmd.callMethod("String", "getStringParam", 1, &keyVal);
            string name = string(nameVal.getStringCString());

            cmd = string( SGAddPropertyCmd_Str + "(" + ownerPath + ", " + name + ")" );
          }
          catch(Exception e)
          {
            printf("SGAddPropertyCmd::Get: exception: %s\n", e.getDesc_cstr());
          }
          return cmd;
        }
    };
  }  
}

#endif // __UI_SCENEHUB_SGAddPropertyCmd_H__


