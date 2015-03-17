// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtGui/QLayout>
#include "DFGNewPortDialog.h"

using namespace FabricUI;
using namespace FabricUI::DFG;

DFGNewPortDialog::DFGNewPortDialog(QWidget * parent, bool showPortType, const DFGConfig & dfgConfig)
: DFGBaseDialog(parent, true, dfgConfig)
{
  if(showPortType)
  {
    m_portTypeCombo = new QComboBox(this);
    m_portTypeCombo->addItem("In");
    m_portTypeCombo->addItem("Out");
    m_portTypeCombo->addItem("IO");
  }
  else
    m_portTypeCombo = NULL;
  m_titleEdit = new QLineEdit("");
  m_dataTypeEdit = new QLineEdit("$TYPE$");
  m_extensionEdit = new QLineEdit("");

  if(showPortType)
    addInput(m_portTypeCombo, "type");
  addInput(m_titleEdit, "title");
  addInput(m_dataTypeEdit, "data type");
  addInput(m_extensionEdit, "extension");
}

DFGNewPortDialog::~DFGNewPortDialog()
{
}

QString DFGNewPortDialog::portType() const
{
  if(!m_portTypeCombo)
    return "";
  return m_portTypeCombo->currentText();
}

QString DFGNewPortDialog::title() const
{
  return m_titleEdit->text();
}

QString DFGNewPortDialog::dataType() const
{
  return m_dataTypeEdit->text();
}

QString DFGNewPortDialog::extension() const
{
  return m_extensionEdit->text();
}

