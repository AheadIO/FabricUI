/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include <FabricUI/SceneHub/Commands/SHLogWidget.h>
#include <QtGui/QHBoxLayout>
#include <QtGui/QMenu>
#include <QtGui/QVBoxLayout>

using namespace FabricUI;
using namespace FabricUI::SceneHub;

std::vector<SHLogWidget*> SHLogWidget::sLogWidgets;
std::vector<std::string> SHLogWidget::sUnconsumedMessages;

/// Constructor.
SHLogWidget::SHLogWidget() {
  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

  QVBoxLayout * layout = new QVBoxLayout();
  setLayout(layout);
  setContentsMargins(0, 0, 0, 0);
  layout->setContentsMargins(0, 0, 0, 0);

  m_text = new QPlainTextEdit(this);
  //m_text->setFont( m_config.fixedFont );
  m_text->setReadOnly(true);
  m_text->setContextMenuPolicy( Qt::CustomContextMenu );
  connect(
    m_text, SIGNAL(customContextMenuRequested(const QPoint&)),
    this, SLOT(showContextMenu(const QPoint&))
    );
  layout->addWidget(m_text);

  sLogWidgets.push_back(this);

  for(size_t i=0;i<sUnconsumedMessages.size();i++)
    log(sUnconsumedMessages[i].c_str());
  sUnconsumedMessages.clear();
}

/// Destructor --> Remove all the SHLogWidget
SHLogWidget::~SHLogWidget() {

  size_t index = sLogWidgets.size();
  for(size_t i=0;i<sLogWidgets.size();i++)
  {
    if(sLogWidgets[i] == this)
    {
      index = i;
      break;
    }
  }

  if(index < sLogWidgets.size())
  {
    std::vector<SHLogWidget*>::iterator it = sLogWidgets.begin();
    sLogWidgets.erase(it + index);
  }
}

/// Log the content of message.
void SHLogWidget::log(const char * message) {
  callback( NULL, FabricCore::ReportSource_System, FabricCore::ReportLevel_Info, message, 0 );
}

// Logs a message from the KL core.
/// \params userData A pointer to specific user data.
/// \params source The source (file) form where the log is called.
/// \params level Level (depth) of logging.
/// \params stringData A pointer to the meassage to log.
/// \params stringLength The length of the string message.
void SHLogWidget::callback(
  void * userData, 
  FabricCore::ReportSource source,
  FabricCore::ReportLevel level,
  char const * stringData,
  uint32_t stringLength)
{
  if(sLogWidgets.size() > 0)
  {
    QString message(stringData);
    bool isError = message.toLower().indexOf("error") > -1;
    for(size_t i=0;i<sLogWidgets.size();i++)
    {
      if(isError) sLogWidgets[i]->m_text->appendHtml("<font color=\"red\">"+message+"</font>");
      else sLogWidgets[i]->m_text->appendPlainText(QString(stringData));
    }
  }
  else sUnconsumedMessages.push_back(stringData);
 

  printf("%s\n", stringData);
}

/// Gets the key event.
/// \param event The Key event
void SHLogWidget::keyPressEvent(QKeyEvent * event) {
  event->accept();
}

/// Show the context menu --> clear the content
void SHLogWidget::showContextMenu(QPoint const &pos) {
  QMenu *menu = m_text->createStandardContextMenu();
  menu->addSeparator();
  QAction *clearAction = new QAction( "Clear", menu );
  connect(clearAction, SIGNAL(triggered()),this, SLOT(clear()));
  menu->addAction( clearAction );
  menu->popup( m_text->mapToGlobal(pos) );
}

/// Clear the window content
void SHLogWidget::clear() {
  m_text->clear();
}
