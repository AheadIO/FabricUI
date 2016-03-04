// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "TabbedKLEditorWidget.h"

#include <QtGui/QHBoxLayout>
#include <QtGui/QScrollBar>

using namespace FabricServices;
using namespace FabricUI::KLEditor;

TabbedKLEditorWidget::TabbedKLEditorWidget(QWidget * parent, ASTWrapper::KLASTManager * manager, const EditorConfig & config)
: QTabWidget(parent)
, ASTWrapper::KLASTClient(manager)
{
  m_config = config;

  setContentsMargins(0, 0, 0, 0);

  openEditor();
}

TabbedKLEditorWidget::~TabbedKLEditorWidget()
{
}

bool TabbedKLEditorWidget::setASTManager(ASTWrapper::KLASTManager * manager)
{
  if(KLASTClient::setASTManager(manager))
  {
    for(unsigned int i=0;i<editorCount();i++)
      editor(i)->sourceCodeWidget()->setASTManager(manager);
  }
  return false;
}

unsigned int TabbedKLEditorWidget::editorCount() const
{
  return count();
}

KLEditorWidget * TabbedKLEditorWidget::editor(unsigned int index)
{
  return (KLEditorWidget*)widget(index);
}

KLEditorWidget * TabbedKLEditorWidget::openEditor(QString klFilePath)
{
  KLEditorWidget * widget = new KLEditorWidget(this, getASTManager(), m_config);

  if(klFilePath.length() > 0)
  {
    widget->sourceCodeWidget()->setFilePath(klFilePath);
    widget->sourceCodeWidget()->revertFile();
  }

  addTab(widget, widget->sourceCodeWidget()->fileName());

  QObject::connect(widget->sourceCodeWidget(), SIGNAL(openKLFileRequested(KLDeclPtr)), this, SLOT(receiveOpenFileRequest(KLDeclPtr)));

  setCurrentIndex(editorCount()-1);

  return widget; 
}

void TabbedKLEditorWidget::closeEditor(unsigned int index)
{
  if(editorCount() > 0)
    removeTab(index);
}

void TabbedKLEditorWidget::receiveOpenFileRequest(KLDeclPtr ptr)
{
  QString klFilePath = ptr.decl->getKLFile()->getAbsoluteFilePath();

  KLEditorWidget * newEditor = NULL;
  for(unsigned int i=0;i<editorCount();i++)
  {
    if(editor(i)->sourceCodeWidget()->filePath() == klFilePath)
    {
      newEditor = editor(i);
      setCurrentIndex(i);
      break;
    }
  }

  if(newEditor == NULL)
    newEditor = openEditor(klFilePath);

  if(newEditor)
  {
    KLSourceCodeWidget * widget = newEditor->sourceCodeWidget();

    // todo: one day we might allow to edit here
    widget->setReadOnly(true);

    const ASTWrapper::KLLocation * location = ptr.decl->getLocation();
    if(location)
    {
      widget->verticalScrollBar()->setValue(location->getLine()-5);
      widget->clearHighlightedLocations();
      widget->highlightLocation(location);
    }
  }
}

