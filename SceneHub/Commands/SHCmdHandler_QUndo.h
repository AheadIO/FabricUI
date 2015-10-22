//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#ifndef __FABRICUI_SceneHub_CmdHandler_QUndo__
#define __FABRICUI_SceneHub_CmdHandler_QUndo__

#include <QtGui/QUndoStack>
#include "SHCmdHandler.h"
 
namespace FabricUI
{
  namespace SceneHub
  {
    class SHCmdHandler_QUndo : public SHCmdHandler
    {
      private:
        QUndoStack *m_qUndoStack;

      protected:
        class WrappedCmd;

      public:
        SHCmdHandler_QUndo(QUndoStack *qUndoStack) : m_qUndoStack(qUndoStack) {}
       
        virtual ~SHCmdHandler_QUndo() {};

      
        //virtual void dfgDoRemoveNodes(
        //  FabricCore::DFGBinding const &binding,
        //  FTL::CStrRef execPath,
        //  FabricCore::DFGExec const &exec,
        //  FTL::ArrayRef<FTL::StrRef> nodeNames
        //  ) = 0;

        //virtual void dfgDoConnect(
        //  FabricCore::DFGBinding const &binding,
        //  FTL::CStrRef execPath,
        //  FabricCore::DFGExec const &exec,
        //  FTL::CStrRef srcPath, 
        //  FTL::CStrRef dstPath
        //  ) = 0;
    };
  };
};

#endif // __FABRICUI_SceneHub_CmdHandler_QUndo__

