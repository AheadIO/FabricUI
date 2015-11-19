/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include <QtGui/QMessageBox>
#include <FabricUI/SceneHub/Commands/SHCmdView.h>
#include <FabricUI/SceneHub/Commands/SGAddObjectCmd.h>
#include <FabricUI/SceneHub/Commands/SGAddPropertyCmd.h>
#include <FabricUI/SceneHub/Commands/SGSetPropertyValueCmd.h>

using namespace FabricUI;
using namespace FabricUI::SceneHub;


/// Constructs a new SHCmdWidget.
/// \param client A reference to the fabric client
/// \param shObject A reference to SceneHub application
/// \param qUndoStack A pointer to the Qt undo-redo stack
SHCmdView::SHCmdView(FabricCore::Client &client, FabricCore::RTVal &shObject, QUndoStack *qUndoStack) :
  m_client(client),
  m_shObject(shObject) 
{
  m_shCmdHandler = FabricUI::SceneHub::SHCmdHandler_QUndo(qUndoStack);

  m_qUndoView = new QUndoView(m_shCmdHandler.getStack());
  m_qUndoView->setEmptyLabel("addObjectCmd(root, true)");
  m_edit = new QLineEdit();
  m_synchronize = new QPushButton("Synch");
  QObject::connect(
    m_synchronize, SIGNAL( clicked() ),
    this, SLOT( synchronize() )
  );

  QHBoxLayout *cmdLayout = new QHBoxLayout();
  cmdLayout->addWidget(m_edit);
  cmdLayout->addWidget(m_synchronize);

  m_text = new QPlainTextEdit(this);
  //m_text->setFont( m_config.fixedFont );
  m_text->setReadOnly(true);
  m_text->setContextMenuPolicy( Qt::CustomContextMenu );

  QVBoxLayout *layout = new QVBoxLayout();
  layout->setSpacing(0);
  layout->addLayout(cmdLayout);
  layout->addWidget(m_qUndoView);
  layout->addWidget(m_text);
  layout->setContentsMargins(0, 0, 0, 0);
  setLayout(layout);

  synchronize();
};

/// Logs the application callback within the log windows.
/// \param stringData The string to log
void SHCmdView::logCallBack(char const*stringData) {
  QString message(stringData);
  bool isError = message.toLower().indexOf("error") > -1;
  if(isError) m_text->appendHtml("<font color=\"red\">"+message+"</font>");
  else m_text->appendPlainText(QString(stringData));
}
 
/// Synchronizes the Qt stack from the KL stack.
void SHCmdView::synchronize() {
  
  m_shCmdHandler.clearStack();
    
  // Get the number of commands already done in the KL stack
  FabricCore::RTVal nbCommandsVal = SHCmd::GetCmdManager(m_shObject).callMethod("Size", "getNumCmdInUndoStack", 0, 0);
  for(uint32_t i=0; i<nbCommandsVal.getUInt32(); ++i)
  {
    FabricCore::RTVal typeVal = SHCmd::RetrieveCmd(m_client, m_shObject, i).callMethod("String", "type", 0, 0);
    std::string type = std::string(typeVal.getStringCString());
 
    // SGAddObjectCmd
    if(ToLower(type).compare(ToLower(SGAddObjectCmd_Type_Str)) == 0)
      addCommand(SGAddObjectCmd::Get(m_client, m_shObject, i), false);

    // SGAddPropertyCmd
    else if(ToLower(type).compare(ToLower(SGAddPropertyCmd_Type_Str)) == 0)
      addCommand(SGAddPropertyCmd::Get(m_client, m_shObject, i), false);

    // SGSetPropertyValueCmd
    else if(ToLower(type).compare(ToLower(SGSetPropertyValueCmd_Type_Str)) == 0)
      addCommand(SGSetPropertyValueCmd::Get(m_client, m_shObject, i), false);

    else std::cerr << "SHCmdView::synchronize : Command type " << type << " wasn't founded" << std::endl;
  }
}

/// Gets the "Return" key event, validate the command.
/// \param event The Key event
void SHCmdView::keyPressEvent(QKeyEvent *event) {
  // Check if the user hit the Enter (Return) key -> validate the command
  if( (event->key()==Qt::Key_Enter) || (event->key()==Qt::Key_Return) ) 
    // Check if the QTextEdit m_edit has the focus
    if(m_edit->hasFocus()) addCommand(m_edit->text().toStdString());
}

/// Adds a command.
/// \param command The command to execute
/// \param exec If true executes the command, just add it to the Qt stack otherwise
bool SHCmdView::addCommand(const std::string &command, bool exec) {
  std::string name;
  if(SHCmd::ExtractName(command, name)) 
  {
    // SGAddObjectCmd
    if(ToLower(name).compare(ToLower(SGAddObjectCmd_Str)) == 0) 
      return m_shCmdHandler.addCommand(SGAddObjectCmd::Create(m_client, m_shObject, command, exec));
    
    // SGAddPropertyCmd
    else if(ToLower(name).compare(ToLower(SGAddPropertyCmd_Str)) == 0) 
      return m_shCmdHandler.addCommand(SGAddPropertyCmd::Create(m_client, m_shObject, command, exec));
    
    // SGSetPropertyValueCmd
    else if(ToLower(name).compare(ToLower(SGSetPropertyValueCmd_Str)) == 0) 
      return m_shCmdHandler.addCommand(SGSetPropertyValueCmd::Create(m_client, m_shObject, command, exec));

    // log an error
    else std::cerr << "SHCmdView::addCommand : Command name " << name << " wasn't founded" << std::endl;
  }
  return false;
}
