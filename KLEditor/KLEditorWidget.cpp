// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "KLEditorWidget.h"

#include <QtGui/QHBoxLayout>

using namespace FabricServices;
using namespace FabricUI::KLEditor;

KLEditorWidget::KLEditorWidget(QWidget * parent, FabricServices::ASTWrapper::KLASTManager * manager, const EditorConfig & config)
: QWidget(parent)
{
  m_config = config;

  setContentsMargins(0, 0, 0, 0);

  m_lineNumbers = new LineNumberWidget(this, config);
  m_sourceCodeWidget = new KLSourceCodeWidget(this, manager, config);

  QObject::connect(m_sourceCodeWidget, SIGNAL(documentScrolled(unsigned int)), m_lineNumbers, SLOT(setLineOffset(unsigned int)));

  QHBoxLayout * layout = new QHBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(m_config.editorSpacing);
  layout->addWidget(m_lineNumbers);
  layout->addWidget(m_sourceCodeWidget);
  setLayout(layout);
}

KLEditorWidget::~KLEditorWidget()
{
}

KLSourceCodeWidget * KLEditorWidget::sourceCodeWidget()
{
  return m_sourceCodeWidget;
}
