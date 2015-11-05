/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#include <FabricUI/SceneHub/Commands/SHCmdView.h>
#include <FTL/OwnedPtr.h>
#include <FabricCore.h>
#include <FTL/JSONValue.h>
#include <FTL/JSONEnc.h>
#include <QtGui/QMessageBox>

using namespace FabricUI;
using namespace FabricUI::SceneHub;


/// Constructs a new SHCmdWidget.
/// \param client A reference to the fabric client
/// \param shObject A reference to SceneHub application
/// \param qUndoStack A pointer to the Qt undo-redo stack
SHCmdView::SHCmdView(FabricCore::Client &client, FabricCore::RTVal &shObject, QUndoStack *qUndoStack) {

  m_shCmdHandler = FabricUI::SceneHub::SHCmdHandler_QUndo(client, shObject, qUndoStack);

  m_qUndoView = new QUndoView(m_shCmdHandler.getUndoStack());
  m_qUndoView->setEmptyLabel("New Scene");
  m_edit = new QLineEdit();

  QVBoxLayout * layout = new QVBoxLayout();
  layout->setSpacing(0);
  layout->addWidget(m_edit);
  layout->addWidget(m_qUndoView);
  layout->setContentsMargins(0, 0, 0, 0);
  setLayout(layout);
};

/// Gets the "Return" key event, validate the command.
/// \param event The Key event
void SHCmdView::keyPressEvent(QKeyEvent *event) {
  // Check if the user hit the Enter (Return) key -> validate the command
  if ( (event->key()==Qt::Key_Enter) || (event->key()==Qt::Key_Return) ) 
    // Check if the QTextEdit m_edit has the focus
    if(m_edit->hasFocus()) parseAndExecuteCmd(m_edit->text().toStdString());
}

/// \internal
/// Parses and executes a command.
/// \param command The command to execute
bool SHCmdView::parseAndExecuteCmd(const std::string &command) {

  std::vector<std::string> split = Split(command, '(');
  if(split.size() > 1)
  {   
    // Get the command's name
    std::string cmdName = split[0];

    // addSGObject
    if(cmdName.compare("addSGObject") == 0) m_shCmdHandler.addSGObject(command);
     
    // If the command is not recognized, do nothing
    else return false;

    return true;
  }

  return false;
}