// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __FABRICUI_SceneHub_SGAddPropertyCmd__
#define __FABRICUI_SceneHub_SGAddPropertyCmd__

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
    const string SGAddPropertyCmd_Str = "addPropertyCmd";
    const string SGAddPropertyCmd_Type_Str = "SGAddPropertyCmd";

    class SGAddPropertyCmd : SHCmd
    {
      public:        
        /// Constructs and executes a command.
        /// \param shObject A reference to SceneHub application
        /// \param cmdDes The command desciprtion
        /// \param params The command parameters
        /// \param exec If true executes the command, just add it to the Qt stack otherwise
        SGAddPropertyCmd(RTVal &shObject, const string &cmdDes, vector<RTVal> &params, bool exec) :
          SHCmd(shObject, SGAddPropertyCmd_Str, cmdDes, params, exec) {};

        /// Adds an object to the scene-graph
        /// \param client A reference to the fabric client
        /// \param shObject A reference to SceneHub application
        /// \param command The command to create
        /// \param exec If true executes the command, just add it to the Qt stack otherwise
        static SHCmd* Create(Client &client, RTVal &shObject, const string &command, bool exec) {
          vector<string> params;
          if(SHCmd::ExtractParams(command, params) && params.size() == 2)
          {
            string ownerPath = RemoveSpace(params[0]); 
            string name = RemoveSpace(params[1]); 
         
            FABRIC_TRY_RETURN("SGAddPropertyCmd::Create", false,
              vector<RTVal> params(2);
              params[0] = RTVal::ConstructString(client, ownerPath.c_str());
              params[1] = RTVal::ConstructString(client, name.c_str());
              return new SGAddPropertyCmd(shObject, command, params, exec);
            );
          }
          return 0;
        };
 
        /// Gets the KL command parameters.
        /// \param client A reference to the fabric client
        /// \param shObject A reference to SceneHub application
        /// \param index The name of the object
        static string Get(Client &client, RTVal &shObject, uint32_t index) {

          FABRIC_TRY_RETURN("SGAddPropertyCmd::Get", false,
     
            FabricCore::RTVal sgCmd = SHCmd::RetrieveCmd(client, shObject, index);

            RTVal keyVal = RTVal::ConstructString(client, "ownerPath");
            RTVal nameVal = sgCmd.callMethod("String", "getStringParam", 1, &keyVal);
            string ownerPath = string(nameVal.getStringCString());

            keyVal = RTVal::ConstructString(client, "name");
            nameVal = sgCmd.callMethod("String", "getStringParam", 1, &keyVal);
            string name = string(nameVal.getStringCString());

            return string( SGAddPropertyCmd_Str + "(" + ownerPath + ", " + name + ")" );
          );
        };
    };
  };  
};

#endif // __FABRICUI_SceneHub_SGAddPropertyCmd__


