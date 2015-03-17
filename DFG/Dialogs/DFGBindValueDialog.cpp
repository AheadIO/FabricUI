// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtGui/QLayout>
#include "DFGBindValueDialog.h"

using namespace FabricUI;
using namespace FabricUI::DFG;

DFGBindValueDialog::DFGBindValueDialog(QWidget * parent, QString dataType, const DFGConfig & dfgConfig)
: DFGBaseDialog(parent, true, dfgConfig)
{
  if(dataType.length() > 0)
  {
    if(dataType[0] == '$')
      dataType = "";
  }
  m_dataTypeEdit = new QLineEdit(dataType.length() == 0 ? "Scalar" : dataType);
  m_extensionEdit = new QLineEdit("");

  addInput(m_dataTypeEdit, "data type");
  addInput(m_extensionEdit, "extension");
}

DFGBindValueDialog::~DFGBindValueDialog()
{
}

QString DFGBindValueDialog::dataType() const
{
  return m_dataTypeEdit->text();
}

QString DFGBindValueDialog::extension() const
{
  return m_extensionEdit->text();
}

