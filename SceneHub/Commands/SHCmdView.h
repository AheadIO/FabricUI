// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __FABRICUI_SceneHub_CmdView_
#define __FABRICUI_SceneHub_CmdView_

#include <QtCore/QString >
#include <QtGui/QWidget>
#include <QtGui/QUndoView>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QKeyEvent>
#include <FTL/OwnedPtr.h>
#include <FTL/JSONEnc.h>
#include <FabricUI/SceneHub/macros.h>
#include <FabricUI/SceneHub/Commands/SHCmdHandler_QUndo.h>
   
namespace FabricUI
{
  namespace SceneHub
  {
    class SHCmdView : public QWidget
    {
      Q_OBJECT

      public:
        /// Constructs a new SHCmdWidget.
        /// \param client A reference to the fabric client
        /// \param shObject A reference to SceneHub application
        /// \param qUndoStack A pointer to the Qt undo-redo stack
        SHCmdView(FabricCore::Client &client, FabricCore::RTVal &shObject, QUndoStack *QUndoStack);

        ~SHCmdView() {};
 

      public slots:
        /// Gets the "Return/Enter" key event, validate the command.
        /// \param event The Key event
        virtual void keyPressEvent(QKeyEvent *event);

      private:
        /// \internal
        /// Parses and executes a command.
        /// \param command The command to execute
        bool parseAndExecuteCmd(const std::string &command);

        /// \internal
        /// Writes commands
        QLineEdit *m_edit;
        /// \internal
        /// View displaying an history of the commands
        QUndoView *m_qUndoView;
        /// \internal
        /// Reference on the SHCmdHandler.
        SHCmdHandler_QUndo m_shCmdHandler;
    };
  };
};

#endif // __FABRICUI_SceneHub_CmdView_
