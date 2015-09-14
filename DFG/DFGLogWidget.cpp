// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGLogWidget.h"
#include <QtGui/QHBoxLayout>
#include <QtGui/QMenu>
#include <QtGui/QVBoxLayout>

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGController::LogFunc DFGLogWidget::s_logFunc = NULL;
std::vector<DFGLogWidget*> DFGLogWidget::sLogWidgets;
std::vector<std::string> DFGLogWidget::sUnconsumedMessages;

DFGLogWidget::DFGLogWidget( const DFGConfig & config )
  : m_config( config )
{
  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

  QVBoxLayout * layout = new QVBoxLayout();
  setLayout(layout);
  setContentsMargins(0, 0, 0, 0);
  layout->setContentsMargins(0, 0, 0, 0);

  m_text = new QPlainTextEdit(this);
  m_text->setFont( m_config.fixedFont );
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

DFGLogWidget::~DFGLogWidget()
{
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
    std::vector<DFGLogWidget*>::iterator it = sLogWidgets.begin();
    sLogWidgets.erase(it + index);
  }
}

void DFGLogWidget::log(const char * message)
{
  callback( NULL, FabricCore::ReportSource_System,
            FabricCore::ReportLevel_Info, message, 0 );
}

void DFGLogWidget::callback(
  void * userData, 
  FabricCore::ReportSource source,
  FabricCore::ReportLevel level,
  char const * stringData,
  uint32_t stringLength
  )
{
  if(sLogWidgets.size() > 0)
  {
    QString message(stringData);
    bool isError = message.toLower().indexOf("error") > -1;
    for(size_t i=0;i<sLogWidgets.size();i++)
    {
      if(isError)
        sLogWidgets[i]->m_text->appendHtml("<font color=\"red\">"+message+"</font>");
      else
        sLogWidgets[i]->m_text->appendPlainText(QString(stringData));
    }
  }
  else
  {
    sUnconsumedMessages.push_back(stringData);
  }

  if(s_logFunc)
    (*s_logFunc)(stringData);
  else
    printf("%s\n", stringData);
}

void DFGLogWidget::keyPressEvent(QKeyEvent * event) {
  event->accept();
}

void DFGLogWidget::setLogFunc(DFGController::LogFunc func)
{
  s_logFunc = func;
}

void DFGLogWidget::showContextMenu( QPoint const &pos )
{
  QMenu *menu = m_text->createStandardContextMenu();

  menu->addSeparator();

  QAction *clearAction = new QAction( "Clear", menu );
  connect(
    clearAction, SIGNAL(triggered()),
    this, SLOT(clear())
    );
  menu->addAction( clearAction );

  menu->popup( m_text->mapToGlobal(pos) );
}

void DFGLogWidget::clear()
{
  m_text->clear();
}
