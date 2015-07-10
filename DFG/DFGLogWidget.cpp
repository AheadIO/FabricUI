// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGLogWidget.h"

#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>

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
  layout->addWidget(m_text);

  sLogWidgets.push_back(this);

  for(size_t i=0;i<sUnconsumedMessages.size();i++)
    callback(NULL, sUnconsumedMessages[i].c_str(), 0);
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
  callback(NULL, message, 0);
}

void DFGLogWidget::callback(void * userData, char const * stringData, uint32_t stringLength)
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

void DFGLogWidget::setLogFunc(DFGController::LogFunc func)
{
  s_logFunc = func;
}


void DFGLogWidget::copy()
{
  m_text->copy();
}
