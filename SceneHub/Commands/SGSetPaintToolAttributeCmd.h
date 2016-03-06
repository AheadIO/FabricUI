/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_SGSetPaintToolAttributeCmd_H__
#define __UI_SCENEHUB_SGSetPaintToolAttributeCmd_H__

 
#include "SHCmd.h"
#include <FabricUI/Util/StringUtils.h>

using namespace std;
using namespace FabricCore;
 

namespace FabricUI
{
  namespace SceneHub
  {
    const string SGSetPaintToolAttributeCmd_Str = "setPaintToolAttributeCmd";
    const string SGSetPaintToolAttributeCmd_Type_Str = "SGSetPaintToolAttributeCmd";

    class SGSetPaintToolAttributeCmd : SHCmd
    {
      public:        
        /// Constructs and executes a command.
        /// \param shGLScene A reference to SHGLScene
        /// \param cmdDes The command desciprtion
        /// \param params The command parameters
        /// \param exec If true executes the command, just add it to the Qt stack otherwise
        SGSetPaintToolAttributeCmd(SHGLScene *shGLScene, string cmdDes, vector<RTVal> &params, bool exec) :
          SHCmd(shGLScene, SGSetPaintToolAttributeCmd_Str, cmdDes, params, exec) {};
      
        /// Adds an object to the scene-graph
        /// \param shGLScene A reference to SHGLScene
        /// \param command The command to create
        /// \param exec If true executes the command, just add it to the Qt stack otherwise
        static SHCmd* Create(SHGLScene *shGLScene, const string &command, bool exec) {
          // Not scriptable
          if(exec == true) 
            return 0;

          SHCmd* cmd = 0;
          vector<string> params;
          if(SHCmd::ExtractParams(command, params) && params.size() == 1)
          {
            // Get the name of the object
            string fullPath = FabricUI::Util::RemoveSpace(params[0]); 
            try 
            {
              vector<RTVal> params(1);
              params[0] = RTVal::ConstructString(shGLScene->getClient(), fullPath.c_str());
              cmd = new SGSetPaintToolAttributeCmd(shGLScene, command, params, exec);
            }
            catch(Exception e)
            {
              printf("SGSetPaintToolAttributeCmd::Create: exception: %s\n", e.getDesc_cstr());
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
            RTVal keyVal = RTVal::ConstructString(shGLScene->getClient(), "fullPath");
            RTVal fullPathVal = sgCmd.callMethod("String", "getStringParam", 1, &keyVal);
            string fullPath = string(fullPathVal.getStringCString());
            cmd = string( SGSetPaintToolAttributeCmd_Str + "(" + fullPath + ")" );
          }
          catch(Exception e)
          {
            printf("SGSetPaintToolAttributeCmd::Get: exception: %s\n", e.getDesc_cstr());
          }
          return cmd;
        }
        
    };
  }  
}

#endif // __UI_SCENEHUB_SGSetPaintToolAttributeCmd_H__
