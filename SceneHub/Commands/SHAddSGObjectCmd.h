// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __FABRICUI_SceneHub_AddSGObjectCmd__
#define __FABRICUI_SceneHub_AddSGObjectCmd__

#include <FabricCore.h>
#include <FTL/StrRef.h>
#include <FTL/StrRef.h>
#include <FabricUI/SceneHub/macros.h>
#include <FabricUI/SceneHub/SHHotKeys.h>
#include <FabricUI/SceneHub/Commands/SHCmd.h>
 
namespace FabricUI
{
  namespace SceneHub
  {
    class SHAddSGObjectCmd 
    {
      public:        
        /// Adds an object to the scene-graph
        /// \param client A reference to the fabric client
        /// \param shObject A reference to SceneHub application
        /// \param name The name of the object
        /// \param isGlobal True to add a global object, False a pinned object
        /// \param exec If true executes the command, just add it to the Qt stack otherwise
        static SHCmd* exec(
          FabricCore::Client &client,
          FabricCore::RTVal &shObject,
          const std::string &name, 
          bool isGlobal, 
          bool exec = true) 
        {
          FABRIC_TRY_RETURN("SHAddSGObjectCmd::exec", false,
            std::vector<FabricCore::RTVal> params(2);
            params[0] = FabricCore::RTVal::ConstructString(client, name.c_str());
            params[1] = FabricCore::RTVal::ConstructBoolean(client, isGlobal);

            FabricCore::RTVal sceneGraph = shObject.callMethod("SceneGraph", "getScene", 0, 0);
            
            if(exec) sceneGraph.callMethod("", "addObjectCmd", 2, &params[0]);

            FabricCore::RTVal cmdManager = sceneGraph.callMethod("SGCmdManager", "getOrCreateHierarchyCmdManager", 0, 0);
            SHCmd *cmd = new SHCmd(cmdManager);

            cmd->setDesc("addSGObject : " + name);

            return cmd;
          );
        }

        /// Adds an object to the scene-graph
        /// \param client A reference to the fabric client
        /// \param shObject A reference to SceneHub application
        /// \param command The command to be parsed
        static SHCmd* parseAndExec(FabricCore::Client &client, FabricCore::RTVal &shObject, const std::string &command) {

          FABRIC_TRY_RETURN("SHAddSGObjectCmd::parseAndExec", false,

            std::vector<std::string> split1 = Split(command, '(');
            if(split1.size() == 2)
            {
              std::vector<std::string> split2 = Split(split1[1], ')');
              if(split2.size() == 2)
              {                
                std::vector<std::string> split = Split(split2[0], ',');
                if(split.size() == 2)
                {
                  // Get the name of the object
                  std::string name = RemoveWithSpace(split[0]); 
                  std::string isGlobalStr = RemoveWithSpace(split[1]); 
                  // Get if it a global object
                  bool isGlobal = false;
                  if(IsNumber(isGlobalStr)) isGlobal = bool(ToNum<int>(isGlobalStr));
                  else isGlobal = (ToLower(isGlobalStr).compare("true") == 0) ? true : false;          
                  return SHAddSGObjectCmd::exec(client, shObject, name, isGlobal);
                }
                return 0;
              }
              return 0;
            }
            return 0;
          );
        }

        /// Gets the KL command parameters.
        /// \param client A reference to the fabric client
        /// \param shObject A reference to SceneHub application
        /// \param isRestorePoint The name of the object
        /// \param name The name of the object
        /// \param isGlobal True to add a global object, False a pinned object
        /// \param fullPath The path of the object within the scenegraph
        /// \param index The unique index of the object within the scenegraph
        static void getParams(
          FabricCore::Client &client, 
          FabricCore::RTVal &sgCmdVal,
          bool &isRestorePoint,
          std::string &name,
          bool &isGlobal,
          std::string &fullPath,
          uint32_t &index) 
        {
          FABRIC_TRY("SHAddSGObjectCmd::getParams",

            FabricCore::RTVal keyVal = FabricCore::RTVal::ConstructString(client, "isRestorePoint");
            FabricCore::RTVal isRestorePointVal = sgCmdVal.callMethod("Boolean", "getBooleanParam", 1, &keyVal);
            isRestorePoint = isRestorePointVal.getBoolean();

            keyVal = FabricCore::RTVal::ConstructString(client, "name");
            FabricCore::RTVal nameVal = sgCmdVal.callMethod("String", "getStringParam", 1, &keyVal);
            name = std::string(nameVal.getStringCString());

            keyVal = FabricCore::RTVal::ConstructString(client, "isGlobal");
            FabricCore::RTVal isGlobalVal = sgCmdVal.callMethod("Boolean", "getBooleanParam", 1, &keyVal);
            isGlobal = isGlobalVal.getBoolean();

            keyVal = FabricCore::RTVal::ConstructString(client, "fullPath");
            FabricCore::RTVal fullPathVal = sgCmdVal.callMethod("String", "getStringParam", 1, &keyVal);
            fullPath = std::string(fullPathVal.getStringCString());

            keyVal = FabricCore::RTVal::ConstructString(client, "index");
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
