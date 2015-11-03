// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __FABRICUI_SceneHub_AddSGObjectCmd__
#define __FABRICUI_SceneHub_AddSGObjectCmd__

#include <FabricCore.h>
#include <FTL/StrRef.h>
#include <FTL/StrRef.h>
#include <FabricUI/SceneHub/macros.h>
#include <FabricUI/SceneHub/Commands/SHCmd.h>
#include <iostream>
#include <vector>
#include <string>

namespace FabricUI
{
  namespace SceneHub
  {
    class SHAddSGObjectCmd 
    {
      public:
        
        /// Adds an object to the scene-graph
        /// \param name The name of the object
        /// \param isGlobalObjet True to add a global object, False a pinned object
        static SHCmd* exec(
          FabricCore::Client *client,
          FabricCore::RTVal &shObject,
          std::string name, 
          bool isGlobalObject, 
          bool exec) 
        {
          FABRIC_TRY_RETURN("SHAddSGObjectCmd::exec", false,
 
            std::vector<FabricCore::RTVal> params(2);
            params[0] = FabricCore::RTVal::ConstructString(*client, name.c_str());
            params[1] = FabricCore::RTVal::ConstructBoolean(*client, isGlobalObject);
            FabricCore::RTVal sceneGraph = shObject.callMethod("SceneGraph", "getScene", 0, 0);
            
            if(exec) sceneGraph.callMethod("", "addObjectCmd", 2, &params[0]);

            FabricCore::RTVal cmdManager = sceneGraph.callMethod("SGCmdManager", "getOrCreateHierarchyCmdManager", 0, 0);
            SHCmd *cmd = new SHCmd(cmdManager);

            cmd->setDesc("addObject : " + name);
            return cmd;
          );
        }

        static void getParams(
          FabricCore::Client 
          *client, FabricCore::RTVal &sgCmdVal,
          bool &isRestorePoint,
          std::string &name,
          bool &isGlobal,
          std::string &fullPath,
          uint32_t &index) 
        {
          FABRIC_TRY("SHAddSGObjectCmd::getParams",

            FabricCore::RTVal keyVal = FabricCore::RTVal::ConstructString(*client, "isRestorePoint");
            FabricCore::RTVal isRestorePointVal = sgCmdVal.callMethod("Boolean", "getBooleanParam", 1, &keyVal);
            isRestorePoint = isRestorePointVal.getBoolean();

            keyVal = FabricCore::RTVal::ConstructString(*client, "name");
            FabricCore::RTVal nameVal = sgCmdVal.callMethod("String", "getStringParam", 1, &keyVal);
            name = std::string(nameVal.getStringCString());

            keyVal = FabricCore::RTVal::ConstructString(*client, "isGlobal");
            FabricCore::RTVal isGlobalVal = sgCmdVal.callMethod("Boolean", "getBooleanParam", 1, &keyVal);
            isGlobal = isGlobalVal.getBoolean();

            keyVal = FabricCore::RTVal::ConstructString(*client, "fullPath");
            FabricCore::RTVal fullPathVal = sgCmdVal.callMethod("String", "getStringParam", 1, &keyVal);
            fullPath = std::string(fullPathVal.getStringCString());

            keyVal = FabricCore::RTVal::ConstructString(*client, "index");
            FabricCore::RTVal indexVal = sgCmdVal.callMethod("UInt32", "getUInt32Param", 1, &keyVal);
            index = indexVal.getUInt32();
            
            std::cerr << "isRestorePoint : "<< isRestorePoint << std::endl;
            std::cerr << "name : "          << name           << std::endl;
            std::cerr << "isGlobal : "      << isGlobal       << std::endl;
            std::cerr << "fullPath : "      << fullPath       << std::endl;
            std::cerr << "index : "         << index          << std::endl << std::endl << std::endl;
          );
        }
    };
  };  
};

#endif // __FABRICUI_SceneHub_AddSGObjectCmd__
