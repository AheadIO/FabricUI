/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_SGSetPropertyValueCmd_H__
#define __UI_SCENEHUB_SGSetPropertyValueCmd_H__

#include <FabricCore.h>
#include <FTL/StrRef.h>
#include <FTL/StrRef.h>
#include <FabricUI/Util/macros.h>
#include <QtGui/QKeyEvent>
#include <FabricUI/SceneHub/Commands/SHCmd.h>
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
        /// \param shObject A reference to SceneHub application
        /// \param cmdDes The command desciprtion
        /// \param params The command parameters
        /// \param exec If true executes the command, just add it to the Qt stack otherwise
        SGSetPropertyValueCmd(RTVal &shObject, const string &cmdDes, vector<RTVal> &params, bool exec) :
          SHCmd(shObject, SGSetPropertyValueCmd_Str, cmdDes, params, exec) {};

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
            string fullPath = FabricUI::Util::RemoveSpace(params[0]); 
            string type = FabricUI::Util::RemoveSpace(params[1]); 
            string data = FabricUI::Util::RemoveSpace(params[2]); 
            uint64_t dataSize = FabricUI::Util::ToNum<uint64_t>(params[3]); 

            FABRIC_TRY_RETURN("SGSetPropertyValueCmd::Create", false,

              RTVal val = SHCmd::SetParamValue(client, type.c_str(), data);
              
              vector<RTVal> params(nbParams);
              params[0] = RTVal::ConstructString(client, fullPath.c_str());
              params[1] = val.callMethod("Type", "type", 0, 0);
              params[2] = val.callMethod("Data", "data", 0, 0);
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

            vector<RTVal> params(3);
            params[0] = RTVal::ConstructUInt64(client, 0);
            params[1] = RTVal::ConstructString(client, string("").c_str());
            sgCmd.callMethod("", "getPropertyParam", 2, &params[0]);
            uint64_t dataSize = params[0].getUInt64();
            string type = string(params[1].getStringCString());

            RTVal structVal = RTVal::Construct(client, type.c_str(), 0, 0);
            RTVal dataVal = structVal.callMethod("Data", "data", 0, 0);
            sgCmd.callMethod("", "getPropertyData", 1, &dataVal);
                  
            string data = EncodeRTValToJSON(client, structVal);
        
            string res = string( SGSetPropertyValueCmd_Str + "(" + fullPath + ", " + type + 
              ", " + FabricUI::Util::RemoveSpace(FabricUI::Util::RemoveNewLine(data)) + ", " + FabricUI::Util::ToStr(dataSize) + ")" );

            return res;
          );
        };     
    };
  };  
};

#endif // __UI_SCENEHUB_SGSetPropertyValueCmd_H__
