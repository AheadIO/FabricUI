/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __FABRICUI_SceneHub_SGAddObjectCmd__
#define __FABRICUI_SceneHub_SGAddObjectCmd__

#include <FabricCore.h>
#include <FTL/StrRef.h>
#include <FTL/StrRef.h>
#include <FabricUI/SceneHub/macros.h>
#include <FabricUI/SceneHub/SHHotKeys.h>
#include <FabricUI/SceneHub/Commands/SHCmd.h>
 
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
        /// \param shObject A reference to SceneHub application
        /// \param cmdDes The command desciprtion
        /// \param params The command parameters
        /// \param exec If true executes the command, just add it to the Qt stack otherwise
        SGAddObjectCmd(RTVal &shObject, const string &cmdDes, vector<RTVal> &params, bool exec) :
          SHCmd(shObject, SGAddObjectCmd_Str, cmdDes, params, exec) {};

        /// Adds an object to the scene-graph
        /// \param client A reference to the fabric client
        /// \param shObject A reference to SceneHub application
        /// \param command The command to create
        /// \param exec If true executes the command, just add it to the Qt stack otherwise
        static SHCmd* Create(Client &client, RTVal &shObject, const string &command, bool exec) {
          vector<string> params;
          if(SHCmd::ExtractParams(command, params) && params.size() == 2)
          {
            // Get the name of the object
            string name = RemoveSpace(params[0]); 
            string isGlobalStr = RemoveSpace(params[1]); 
            // Get if it a global object
            bool isGlobal = false;
            if(IsNumber(isGlobalStr)) isGlobal = bool(ToNum<int>(isGlobalStr));
            else isGlobal = (ToLower(isGlobalStr).compare("true") == 0) ? true : false;   

            FABRIC_TRY_RETURN("SGAddObjectCmd::Create", false,
              vector<RTVal> params(2);
              params[0] = RTVal::ConstructString(client, name.c_str());
              params[1] = RTVal::ConstructBoolean(client, isGlobal);

              return new SGAddObjectCmd(shObject, command, params, exec);
            );
          }
          return 0;
        };
 
        /// Gets the KL command parameters.
        /// \param client A reference to the fabric client
        /// \param shObject A reference to SceneHub application
        /// \param index The name of the object
        static string Get(Client &client, RTVal &shObject, uint32_t index) {

          FABRIC_TRY_RETURN("SGAddObjectCmd::Get", false,
     
            FabricCore::RTVal sgCmd = SHCmd::RetrieveCmd(client, shObject, index);

            RTVal keyVal = RTVal::ConstructString(client, "name");
            RTVal nameVal = sgCmd.callMethod("String", "getStringParam", 1, &keyVal);
            string name = string(nameVal.getStringCString());

            keyVal = RTVal::ConstructString(client, "isGlobal");
            RTVal isGlobalVal = sgCmd.callMethod("Boolean", "getBooleanParam", 1, &keyVal);
            bool isGlobal = isGlobalVal.getBoolean();

            return string( SGAddObjectCmd_Str + "(" + name + ", " + ToStr(isGlobal) + ")" );
          );
        };
        
    };
  };  
};

#endif // __FABRICUI_SceneHub_SGAddObjectCmd__


