/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
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
    
      protected:
        class WrappedCmd;

      public:
        /// Default constructor
        SHCmdHandler_QUndo() {};

        /// Constructs a new SHCmdHandler_QUndo.
        /// \param qUndoStack A pointer to the Qt undo-redo stack
        SHCmdHandler_QUndo(QUndoStack *qUndoStack) : m_qUndoStack(qUndoStack) {};
  
        virtual ~SHCmdHandler_QUndo() {};

        /// Gets a pointer to the qt command stack.
        QUndoStack* getStack() { return m_qUndoStack; };
        
        /// Adds and executes a command
        /// \param cmd The command
        bool addCommand(SHCmd *cmd);
    };
  };
};

#endif // __FABRICUI_SceneHub_CmdHandler_QUndo__

