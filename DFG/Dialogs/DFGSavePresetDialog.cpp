// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtGui/QLayout>
#include "DFGSavePresetDialog.h"

using namespace FabricUI;
using namespace FabricUI::DFG;

DFGSavePresetDialog::DFGSavePresetDialog(
  QWidget * parent,
  FabricCore::DFGHost & host,
  QString name,
  // QString version,
  const DFGConfig & dfgConfig
  )
: DFGBaseDialog(parent, true, dfgConfig)
{
  m_nameEdit = new QLineEdit(name, this);
  addInput(m_nameEdit, "name");
  // m_versionEdit = new QLineEdit(version, this);
  // addInput(m_versionEdit, "version");
  m_presetTree = new PresetTreeWidget(this, host, dfgConfig, true, false);
  addInput(m_presetTree, "location");

  TreeView::TreeModel * model = m_presetTree->getTreeModel();
  QObject::connect(model, SIGNAL(itemSelected(FabricUI::TreeView::TreeItem*)), 
    this, SLOT(onItemSelected(FabricUI::TreeView::TreeItem*)));
}

DFGSavePresetDialog::~DFGSavePresetDialog()
{
}

QString DFGSavePresetDialog::name() const
{
  return m_nameEdit->text();
}

// QString DFGSavePresetDialog::version() const
// {
//   return m_versionEdit->text();
// }

QString DFGSavePresetDialog::location() const
{
  return m_location;
}

void DFGSavePresetDialog::onItemSelected(FabricUI::TreeView::TreeItem * item)
{
  if(item->type() == "NameSpace")
    m_location = item->path();
  else if(item->type() == "Preset")
  {
    m_nameEdit->setText(item->name());
    m_location = item->path();
    m_location = m_location.left(m_location.length() - item->name().length() - 1);
  }
}
