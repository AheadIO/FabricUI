/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef __FABRICUI_SceneHub_CmdHandler_QUndo__
#define __FABRICUI_SceneHub_CmdHandler_QUndo__

#include <QtGui/QUndoStack>
#include <FabricUI/SceneHub/macros.h>
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
        FabricCore::Client m_client;
        FabricCore::RTVal m_shObject;

      protected:
        class WrappedCmd;


      public:
        SHCmdHandler_QUndo() {};

        /// Constructs a new SHCmdHandler_QUndo.
        /// \param client A reference to the fabric client
        /// \param shObject A reference to SceneHub application
        /// \param qUndoStack A pointer to the Qt undo-redo stack
        SHCmdHandler_QUndo(
          FabricCore::Client &client, 
          FabricCore::RTVal &shObject, 
          QUndoStack *qUndoStack) : 
          m_client(client),
          m_shObject(shObject),
          m_qUndoStack(qUndoStack) 
        {
          std::cerr << "SHCmdHandler_QUndo" << std::endl;
          addSGObject("named1");
          addSGObject("addSGObject(pinned, false)");
          addSGObject("addSGObject(named2, true)");
        };
  
        virtual ~SHCmdHandler_QUndo() {};

        /// Encodes a rtVal into a Json, saves the rtVal
        /// \param context The core context
        /// \param rtVal The value to encode
        static std::string encodeRTValToJSON(
          FabricCore::Context const& context, 
          FabricCore::RTVal const& rtVal);

        /// Decodes a rtVal from a Json, reads the rtVal
        /// \param context The core context
        /// \param rtVal The result value
        /// \param json The string to decode
        static void decodeRTValFromJSON(
          FabricCore::Context const& context, 
          FabricCore::RTVal &rtVal, 
          FTL::CStrRef json); 

        /// Gets a pointer to the qt command stack.
        QUndoStack* getUndoStack() { return m_qUndoStack; };

        /// Synchronize the Qt stack with the KL stack.
        void synchronize();

        /// Adds an object to the scene-graph
        /// \param command The command as string
        void addSGObject(const std::string &command);
    };
  };
};

#endif // __FABRICUI_SceneHub_CmdHandler_QUndo__

