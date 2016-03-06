/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_CMDVIEW_H__
#define __UI_SCENEHUB_CMDVIEW_H__

#include <QtGui/QWidget>
#include <QtGui/QKeyEvent>
#include <QtGui/QUndoView>
#include <QtGui/QLineEdit>

#include <FabricUI/Util/macros.h>
#include "SHDFGUICmdHandler_QUndo.h"
#include <FabricUI/SceneHub/SHGLScene.h>

namespace FabricUI
{
  namespace SceneHub
  {
    class SHCmdViewWidget : public QWidget
    {
      Q_OBJECT

      public:
        /// Constructs a new SHCmdWidget.
        /// \param shObject A reference to SceneHub application
        /// \param qUndoStack A pointer to the Qt undo-redo stack
        SHCmdViewWidget(SHGLScene *shGLScene, QUndoStack *QUndoStack);

        virtual ~SHCmdViewWidget() {};
 
      public slots:
        /// Gets the "Return/Enter" key event, validate the command.
        /// \param event The Key event
        virtual void keyPressEvent(QKeyEvent *event);

        /// Synchronizes the Qt stack from the KL stack.
        void synchronize();
        
      private:
        /// Adds a command.
        /// \param command The command to execute
        /// \param exec If true executes the command, just add it to the Qt stack otherwise
        bool addCommand(const std::string &command, bool exec = true);
        
        /// Writes commands
        QLineEdit *m_edit;        
        /// View displaying an history of the commands
        QUndoView *m_qUndoView;
        /// Reference to the SHCmdHandler.
        SHDFGUICmdHandler_QUndo m_shCmdHandler;
        SceneHub::SHGLScene *m_shGLScene;
   
    };
  }
}

#endif // __UI_SCENEHUB_CMDVIEW_H__
