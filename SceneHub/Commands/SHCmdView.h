/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __FABRICUI_SceneHub_CmdView_
#define __FABRICUI_SceneHub_CmdView_

#include <QtGui/QWidget>
#include <QtCore/QString>
#include <QtGui/QUndoView>
#include <QtGui/QLineEdit>
#include <QtGui/QKeyEvent>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QPlainTextEdit>
#include <FTL/JSONEnc.h>
#include <FTL/OwnedPtr.h>
#include <FabricUI/Util/macros.h>
#include <FabricUI/SceneHub/Commands/SHLogWidget.h>
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

        virtual ~SHCmdView() {};
 
      public slots:
        /// Gets the "Return/Enter" key event, validate the command.
        /// \param event The Key event
        virtual void keyPressEvent(QKeyEvent *event);

        /// Synchronizes the Qt stack from the KL stack.
        void synchronize(bool);
        
      private:
        /// \internal
        /// Adds a command.
        /// \param command The command to execute
        /// \param exec If true executes the command, just add it to the Qt stack otherwise
        bool addCommand(const std::string &command, bool exec = true);
        
        /// \internal
        /// Writes commands
        QLineEdit *m_edit;        
        /// \internal
        /// Log window
        SHLogWidget *m_logWidget;
        /// \internal
        /// View displaying an history of the commands
        QUndoView *m_qUndoView;
        /// \internal
        /// Reference to the SHCmdHandler.
        SHCmdHandler_QUndo m_shCmdHandler;
        /// \internal
        /// Reference to the client --> construct RTVal.
        FabricCore::Client m_client;
        /// \internal
        /// Reference to the SceneHub app.
        FabricCore::RTVal m_shObject;
    };
  };
};

#endif // __FABRICUI_SceneHub_CmdView_
