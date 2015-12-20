// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "LineEdit.h"

using namespace FabricUI::ValueEditor;

LineEdit::LineEdit(QWidget * parent)
: QLineEdit(parent)
{
  m_lastText = "";

  QObject::connect(this, SIGNAL(editingFinished()), this, SLOT(onEditingFinished()));
}

LineEdit::~LineEdit()
{
}

void LineEdit::onEditingFinished()
{
  QString current = text();
  if(current == m_lastText)
    return;

  m_lastText = current;
  emit lineEditTextEdited(current);
}

void LineEdit::setLineEditText(QString text)
{
  m_lastText = text;
  setText(text);
}
