// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __FABRICUI_SceneHub_SGSetPropertyValueCmd__
#define __FABRICUI_SceneHub_SGSetPropertyValueCmd__

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
    const string SGSetPropertyValueCmd_Str = "sg.setPropertyValueCmd";
    const string SGSetPropertyValueCmd_Type_Str = "SGSetPropertyValueCmd";

    class SGSetPropertyValueCmd : SHCmd
    {
      public:        
        /// Constructs and executes a command.
        /// \param shObject A reference to SceneHub application
        /// \param cmdDes The command desciprtion
        /// \param params The command parameters
        /// \param exec If true executes the command, just add it to the Qt stack otherwise
        SGSetPropertyValueCmd(RTVal &shObject, const string &cmdDes, vector<RTVal> &params, bool exec) :
          SHCmd(shObject, SGSetPropertyValueCmd_Type_Str, cmdDes, params, exec) {};

        /// Adds an object to the scene-graph
        /// \param client A reference to the fabric client
        /// \param shObject A reference to SceneHub application
        /// \param command The command to create
        /// \param exec If true executes the command, just add it to the Qt stack otherwise
        static SHCmd* Create(Client &client, RTVal &shObject, const string &command, bool exec) {
          vector<string> params;
          uint32_t nbParams = 4;
          if(SHCmd::ExtractParams(command, params) && params.size() == nbParams)
          {
            // Get the name of the object
            string fullPath = RemoveSpace(params[0]); 
            string type = RemoveSpace(params[1]); 
            string data = RemoveSpace(params[2]); 
            uint64_t dataSize = ToNum<uint32_t>(params[3]); 

            FABRIC_TRY_RETURN("SGSetPropertyValueCmd::Create", false,

              RTVal val = SHCmd::SetParamValue(client, type.c_str(), data);
              
              vector<RTVal> params(nbParams);
              params[0] = RTVal::ConstructString(client, fullPath.c_str());
              params[1] = val.callMethod("String", "type", 0, 0);
              params[2] = val;
              params[3] = RTVal::ConstructUInt64(client, dataSize);

              return new SGSetPropertyValueCmd(shObject, command, params, exec);
            );
          }
          return 0;
        };

        /// Gets the KL command parameters.
        /// \param client A reference to the fabric client
        /// \param shObject A reference to SceneHub application
        /// \param index The name of the object
        static string Get(Client &client, RTVal &shObject, uint32_t index) {

          FABRIC_TRY_RETURN("SGSetPropertyValueCmd::Get", false,
     
            FabricCore::RTVal sgCmd = SHCmd::RetrieveCmd(client, shObject, index);
                
            RTVal keyVal = RTVal::ConstructString(client, "fullPath");
            RTVal fullPathVal = sgCmd.callMethod("String", "getStringParam", 1, &keyVal);
            string fullPath = string(fullPathVal.getStringCString());

            vector<RTVal> params(2);
            params[0] = RTVal::ConstructUInt64(client, 0);
            params[1] = RTVal::ConstructString(client, string("").c_str());
            sgCmd.callMethod("", "getPropertyParam", 2, &params[0]);
            uint64_t dataSize = params[0].getUInt64();
            string type = string(params[1].getStringCString());

            RTVal propertyVal = sgCmd.callMethod("SGObjectProperty", "getProperty", 0, 0);
            RTVal structVal = RTVal::Construct(client, type.c_str(), 0, 0);
            params[0] = structVal.callMethod("Data", "data", 0, 0);
            params[1] = structVal.callMethod("Size", "dataSize", 0, 0);
            propertyVal.callMethod("", "getSimpleStructValue", 2, &params[0]);
                 
            string data = EncodeRTValToJSON(client, structVal);
        
            string res = string( SGSetPropertyValueCmd_Str + "(" + fullPath + ", " + type + 
              ", " + RemoveSpace(RemoveNewLine(data)) + ", " + ToStr(dataSize) + ")" );

            //std::cerr << "res " << res << std::endl;
            return res;
          );
        };     
    };
  };  
};

#endif // __FABRICUI_SceneHub_SGSetPropertyValueCmd__
