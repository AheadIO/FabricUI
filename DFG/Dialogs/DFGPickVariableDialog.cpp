// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtGui/QLayout>
#include "DFGPickVariableDialog.h"

using namespace FabricUI;
using namespace FabricUI::DFG;

DFGPickVariableDialog::DFGPickVariableDialog(
  QWidget * parent,
  FabricCore::Client & client,
  FabricCore::DFGBinding & binding,
  FTL::CStrRef currentExecPath,
  QString name,
  const DFGConfig & dfgConfig
  )
: DFGBaseDialog(parent, true, dfgConfig)
{
  setWindowTitle("Pick Variable");

  m_nameCombo = new DFGVariablePathComboBox(this, binding, currentExecPath, name);
  addInput(m_nameCombo, "name");
}

DFGPickVariableDialog::~DFGPickVariableDialog()
{
}

QString DFGPickVariableDialog::name() const
{
  return m_nameCombo->currentText();
}
