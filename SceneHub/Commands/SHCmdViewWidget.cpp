/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include "SHCmdViewWidget.h"
#include "SGAddObjectCmd.h"
#include "SGAddPropertyCmd.h"
#include "SGSetPropertyValueCmd.h"
#include "SGSetPaintToolAttributeCmd.h"
#include <FabricUI/Util/StringUtils.h>
#include <QtGui/QVBoxLayout>
using namespace FabricUI;
using namespace FabricUI::SceneHub;

/// Constructs a new SHCmdWidget.
/// \param client A reference to the fabric client
/// \param shObject A reference to SceneHub application
/// \param qUndoStack A pointer to the Qt undo-redo stack
SHCmdViewWidget::SHCmdViewWidget(SHGLScene *shGLScene, QUndoStack *qUndoStack) 
  : m_shGLScene(shGLScene)
{
  m_shCmdHandler = FabricUI::SceneHub::SHDFGUICmdHandler_QUndo(qUndoStack);

  m_qUndoView = new QUndoView(m_shCmdHandler.getStack());
  m_qUndoView->setEmptyLabel("addObjectCmd(root, true)");
  //m_edit = new QLineEdit();
  //QHBoxLayout *cmdLayout = new QHBoxLayout();
  //cmdLayout->addWidget(m_edit);
  
  QVBoxLayout *layout = new QVBoxLayout();
  layout->setSpacing(0);
  //layout->addLayout(cmdLayout);
  layout->addWidget(m_qUndoView);
  layout->setContentsMargins(0, 0, 0, 0);
  setLayout(layout);
};

/// Synchronizes the Qt stack from the KL stack.
void SHCmdViewWidget::synchronize() {

  // Get the number of commands already done in the KL stack
  for(uint32_t i=m_shCmdHandler.getStack()->index(); i<m_shGLScene->getNumCmdInUndoStack(); ++i)
  {
    FabricCore::RTVal typeVal = m_shGLScene->retrieveCmd(i).callMethod("String", "type", 0, 0);
    std::string type = std::string(typeVal.getStringCString());
      
    // SGAddObjectCmd
    if(FabricUI::Util::ToLower(type).compare(FabricUI::Util::ToLower(SGAddObjectCmd_Type_Str)) == 0)
      addCommand(SGAddObjectCmd::Get(m_shGLScene, i), false);

    // SGAddPropertyCmd
    else if(FabricUI::Util::ToLower(type).compare(FabricUI::Util::ToLower(SGAddPropertyCmd_Type_Str)) == 0)
      addCommand(SGAddPropertyCmd::Get(m_shGLScene, i), false);

    // SGSetPropertyValueCmd
    else if(FabricUI::Util::ToLower(type).compare(FabricUI::Util::ToLower(SGSetPropertyValueCmd_Type_Str)) == 0)
      addCommand(SGSetPropertyValueCmd::Get(m_shGLScene, i), false);


    // SGSetPaintToolAttributeCmd
    else if(FabricUI::Util::ToLower(type).compare(FabricUI::Util::ToLower(SGSetPaintToolAttributeCmd_Type_Str)) == 0)
      addCommand(SGSetPaintToolAttributeCmd::Get(m_shGLScene, i), false);

    // log an error
    else std::cerr << "SHCmdViewWidget::synchronize : Command type " << type << " wasn't founded" << std::endl;
  }
}

/// Gets the "Return" key event, validate the command.
/// \param event The Key event
void SHCmdViewWidget::keyPressEvent(QKeyEvent *event) {
  // Check if the user hit the Enter (Return) key -> validate the command
  //if( (event->key()==Qt::Key_Enter) || (event->key()==Qt::Key_Return) ) 
  //  // Check if the QTextEdit m_edit has the focus
  //  if(m_edit->hasFocus()) addCommand(m_edit->text().toStdString());
}

/// Adds a command.
/// \param command The command to execute
/// \param exec If true executes the command, just add it to the Qt stack otherwise
bool SHCmdViewWidget::addCommand(const std::string &command, bool exec) {
  std::string name;
  if(SHCmd::ExtractName(command, name)) 
  {
    // SGAddObjectCmd
    if(FabricUI::Util::ToLower(name).compare(FabricUI::Util::ToLower(SGAddObjectCmd_Str)) == 0) 
      return m_shCmdHandler.addCommand(SGAddObjectCmd::Create(m_shGLScene, command, exec));
    
    // SGAddPropertyCmd
    else if(FabricUI::Util::ToLower(name).compare(FabricUI::Util::ToLower(SGAddPropertyCmd_Str)) == 0) 
      return m_shCmdHandler.addCommand(SGAddPropertyCmd::Create(m_shGLScene, command, exec));
    
    // SGSetPropertyValueCmd
    else if(FabricUI::Util::ToLower(name).compare(FabricUI::Util::ToLower(SGSetPropertyValueCmd_Str)) == 0) 
      return m_shCmdHandler.addCommand(SGSetPropertyValueCmd::Create(m_shGLScene, command, exec));

    // SGSetPaintToolAttributeCmd
    else if(FabricUI::Util::ToLower(name).compare(FabricUI::Util::ToLower(SGSetPaintToolAttributeCmd_Str)) == 0)
      return m_shCmdHandler.addCommand(SGSetPaintToolAttributeCmd::Create(m_shGLScene, command, exec));
 
    // log an error
    else std::cerr << "SHCmdViewWidget::addCommand : Command name " << name << " wasn't founded" << std::endl;
  }
  return false;
}
