// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtGui/QLayout>
#include "DFGNewVariableDialog.h"

using namespace FabricUI;
using namespace FabricUI::DFG;

DFGNewVariableDialog::DFGNewVariableDialog(
  QWidget * parent,
  FabricCore::Client & client,
  FabricCore::DFGBinding & binding,
  FTL::CStrRef currentExecPath,
  QString name,
  QString dataType,
  QString extension,
  const DFGConfig & dfgConfig
  )
: DFGBaseDialog(parent, true, dfgConfig)
{
  m_nameEdit = new DFGVariablePathLineEdit(this, binding, currentExecPath, name);
  addInput(m_nameEdit, "name");
  m_dataTypeEdit = new DFGRegisteredTypeLineEdit(this, client, dataType);
  addInput(m_dataTypeEdit, "type");
  m_extensionEdit = new DFGExtensionLineEdit(this, client, dataType);
  addInput(m_extensionEdit, "extension");
  m_nameEdit->selectAll();
}

DFGNewVariableDialog::~DFGNewVariableDialog()
{
}

QString DFGNewVariableDialog::name() const
{
  return m_nameEdit->text();
}

QString DFGNewVariableDialog::dataType() const
{
  return m_dataTypeEdit->text();
}

QString DFGNewVariableDialog::extension() const
{
  return m_extensionEdit->text();
}
