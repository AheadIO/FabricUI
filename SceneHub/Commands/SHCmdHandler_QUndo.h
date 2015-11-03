/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef __FABRICUI_SceneHub_CmdHandler_QUndo__
#define __FABRICUI_SceneHub_CmdHandler_QUndo__

#include <QtGui/QUndoStack>
#include <iostream>
#include <string>
#include <FTL/JSONEnc.h>
#include <FTL/Str.h>
#include <FTL/MapCharSingle.h>
#include <FabricUI/SceneHub/Commands/SHCmd.h>

namespace FabricUI
{
  namespace SceneHub
  {
    class SHCmdHandler_QUndo 
    {
      private:
        QUndoStack *m_qUndoStack;
        FabricCore::Client *m_client;
        FabricCore::RTVal m_shObject;

      protected:
        class WrappedCmd;


      public:
        SHCmdHandler_QUndo() {};

        SHCmdHandler_QUndo(
          FabricCore::Client client, 
          FabricCore::RTVal shObject,
          QUndoStack *qUndoStack) : 
          m_client(&client),
          m_shObject(shObject),
          m_qUndoStack(qUndoStack) 
        {
          std::cerr << "SHCmdHandler_QUndo" << std::endl;
          addSGObject("named1", true);
          addSGObject("pinned", false);
          addSGObject("named2", true);
        };
  
        virtual ~SHCmdHandler_QUndo() {};

        /// Synchronize the Qt and KL stacks.
        void synchronizeStack();

        static std::string encodeRTValToJSON(FabricCore::Context const& context, FabricCore::RTVal const& rtVal);

        static void decodeRTValFromJSON(FabricCore::Context const& context, FabricCore::RTVal & rtVal, FTL::CStrRef json); 

        /// Adds an object to the scene-graph
        /// \param name The name of the object
        /// \param isGlobal True to add a global object, False a pinned object
        void addSGObject(std::string name, bool isGlobal, bool exec = true);
 
    };
  };
};

#endif // __FABRICUI_SceneHub_CmdHandler_QUndo__

