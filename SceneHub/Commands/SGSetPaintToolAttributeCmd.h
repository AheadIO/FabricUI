/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_SGSetPaintToolAttributeCmd_H__
#define __UI_SCENEHUB_SGSetPaintToolAttributeCmd_H__

 
#include <FabricUI/Util/macros.h>
#include <QtGui/QKeyEvent>
#include <FabricUI/SceneHub/Commands/SHCmd.h>
 
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
        /// \param shObject A reference to SceneHub application
        /// \param cmdDes The command desciprtion
        /// \param params The command parameters
        /// \param exec If true executes the command, just add it to the Qt stack otherwise
        SGSetPaintToolAttributeCmd(RTVal &shObject, const string &cmdDes, vector<RTVal> &params, bool exec) :
          SHCmd(shObject, SGSetPaintToolAttributeCmd_Str, cmdDes, params, exec) {};

        /// Adds an object to the scene-graph
        /// \param client A reference to the fabric client
        /// \param shObject A reference to SceneHub application
        /// \param command The command to create
        /// \param exec If true executes the command, just add it to the Qt stack otherwise
        static SHCmd* Create(Client &client, RTVal &shObject, const string &command, bool exec) {
          // Not scriptable
          if(exec == true) 
            return 0;
          
          vector<string> params;
          if(SHCmd::ExtractParams(command, params) && params.size() == 1)
          {
            // Get the name of the object
            string fullPath = RemoveSpace(params[0]); 
            FABRIC_TRY_RETURN("SGSetPaintToolAttributeCmd::Create", false,
              vector<RTVal> params(1);
              params[0] = RTVal::ConstructString(client, fullPath.c_str());
              return new SGSetPaintToolAttributeCmd(shObject, command, params, exec);
            );
          }
          return 0;
        };
 
        /// Gets the KL command parameters.
        /// \param client A reference to the fabric client
        /// \param shObject A reference to SceneHub application
        /// \param index The name of the object
        static string Get(Client &client, RTVal &shObject, uint32_t index) {

          FABRIC_TRY_RETURN("SGSetPaintToolAttributeCmd::Get", false,
            FabricCore::RTVal sgCmd = SHCmd::RetrieveCmd(client, shObject, index);
            RTVal keyVal = RTVal::ConstructString(client, "fullPath");
            RTVal fullPathVal = sgCmd.callMethod("String", "getStringParam", 1, &keyVal);
            string fullPath = string(fullPathVal.getStringCString());
            return string( SGSetPaintToolAttributeCmd_Str + "(" + fullPath + ")" );
          );
        };
        
    };
  };  
};

#endif // __UI_SCENEHUB_SGSetPaintToolAttributeCmd_H__
