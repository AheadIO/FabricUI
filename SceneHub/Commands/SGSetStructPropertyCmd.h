// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __FABRICUI_SceneHub_SGSetStructPropertyCmd__
#define __FABRICUI_SceneHub_SGSetStructPropertyCmd__

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
    const std::string SGSetStructPropertyCmd_Str = "sg.setPropertyCmd";
    const std::string SGSetStructPropertyCmd_Type_Str = "SGSetStructPropertyCmd";

    class SGSetStructPropertyCmd : SHCmd
    {
      public:        
        /// Constructs and executes a command.
        /// \param shObject A reference to SceneHub application
        /// \param cmdDes The command desciprtion
        /// \param params The command parameters
        /// \param exec If true executes the command, just add it to the Qt stack otherwise
        SGSetStructPropertyCmd(RTVal &shObject, const string &cmdDes, vector<RTVal> &params, bool exec) :
          SHCmd(shObject, SGSetStructPropertyCmd_Type_Str, cmdDes, params, exec) {};

        /// Adds an object to the scene-graph
        /// \param client A reference to the fabric client
        /// \param shObject A reference to SceneHub application
        /// \param command The command to create
        /// \param exec If true executes the command, just add it to the Qt stack otherwise
        static SHCmd* Create(Client &client, RTVal &shObject, const string &command, bool exec) {
          vector<string> params;
          if(SHCmd::ExtractParams(command, params) && params.size() == 3)
          {
            // Get the name of the object
            string ownerPath = RemoveWithSpace(params[0]); 
            string name = RemoveWithSpace(params[1]); 
            float value = ToNum<float>(RemoveWithSpace(params[2])); 
          
            std::cerr << "\n\n\n\n\n\n\n" << std::endl;

            RTVal val = RTVal::Construct(client, "Mat44", 0, 0);
            std::cerr << SHCmd::EncodeRTValToJSON(client, val) << std::endl;

            std::cerr << "\n\n\n\n\n\n\n" << std::endl;

            FABRIC_TRY_RETURN("SGSetStructPropertyCmd::Create", false,
              vector<RTVal> params(3);
              params[0] = RTVal::ConstructString(client, ownerPath.c_str());
              params[1] = RTVal::ConstructString(client, name.c_str());
              params[2] = RTVal::ConstructFloat32(client, value);

              return new SGSetStructPropertyCmd(shObject, command, params, exec);
            );
          }
          return 0;
        };

        /// Gets the KL command parameters.
        /// \param client A reference to the fabric client
        /// \param shObject A reference to SceneHub application
        /// \param index The name of the object
        static std::string Get(Client &client, RTVal &shObject, uint32_t index) {

          FABRIC_TRY_RETURN("SGSetStructPropertyCmd::Get", false,
     
            FabricCore::RTVal sgCmd = SHCmd::RetrieveCmd(client, shObject, index);
              
            RTVal keyVal = RTVal::ConstructString(client, "ownerPath");
            RTVal ownerPathVal = sgCmd.callMethod("String", "getStringParam", 1, &keyVal);
            string ownerPath = string(ownerPathVal.getStringCString());

            keyVal = RTVal::ConstructString(client, "name");
            RTVal nameVal = sgCmd.callMethod("String", "getStringParam", 1, &keyVal);
            string name = string(nameVal.getStringCString());

            //keyVal = RTVal::ConstructString(client, "value");
            //RTVal valueVal = sgCmd.callMethod("Float32", "getFloat32Param", 1, &keyVal);
            float value = 1.0;// valueVal.getFloat32();

            return string( SGSetStructPropertyCmd_Str + "(" + ownerPath + ", " +  name + ", " + ToStr(value) + ")" );
          );
        };     
    };
  };  
};

#endif // __FABRICUI_SceneHub_SGSetStructPropertyCmd__
