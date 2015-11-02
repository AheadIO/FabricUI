/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef __FABRICUI_SceneHub_CmdHandler_QUndo__
#define __FABRICUI_SceneHub_CmdHandler_QUndo__

#include <QtGui/QUndoStack>
#include <FabricUI/SceneHub/SHCmdHandler.h>
#include <iostream>
#include <string>
#include <FTL/JSONEnc.h>
#include <FTL/Str.h>
#include <FTL/MapCharSingle.h>

namespace FabricUI
{
  namespace SceneHub
  {
    class SHCmdHandler_QUndo : public SHCmdHandler
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

        /// Add an object to the scene-graph
        void addSGObject(std::string name, bool isGlobalObject); 

    };
  };
};

#endif // __FABRICUI_SceneHub_CmdHandler_QUndo__

