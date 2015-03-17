// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtGui/QLayout>
#include "DFGGetStringDialog.h"

using namespace FabricUI;
using namespace FabricUI::DFG;

DFGGetStringDialog::DFGGetStringDialog(QWidget * parent, QString text, const DFGConfig & dfgConfig)
: DFGBaseDialog(parent, false, dfgConfig)
{
  m_lineEdit = new QLineEdit(text);
  m_lineEdit->selectAll();
  addInput(m_lineEdit);
}

DFGGetStringDialog::~DFGGetStringDialog()
{
}

QString DFGGetStringDialog::text() const
{
  return m_lineEdit->text();
}

