// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include <QtGui/QLayout>
#include "DFGGetTextDialog.h"

using namespace FabricUI;
using namespace FabricUI::DFG;

DFGGetTextDialog::DFGGetTextDialog(QWidget * parent, QString text, const DFGConfig & dfgConfig)
: DFGBaseDialog(parent, false, dfgConfig)
{
  m_textEdit = new QTextEdit();
  m_textEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  m_textEdit->append(text);
  m_textEdit->selectAll();

  addInput(m_textEdit);
}

DFGGetTextDialog::~DFGGetTextDialog()
{
}

QString DFGGetTextDialog::text() const
{
  return m_textEdit->toPlainText();
}

