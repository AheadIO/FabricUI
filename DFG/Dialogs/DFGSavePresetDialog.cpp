// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtGui/QLayout>
#include <QtGui/QMessageBox>
#include "DFGSavePresetDialog.h"
#include "DFGGetStringDialog.h"
#include <FTL/FS.h>

using namespace FabricUI;
using namespace FabricUI::DFG;

DFGSavePresetDialog::DFGSavePresetDialog(
  QWidget * parent,
  DFGController *dfgController,
  QString name,
  const DFGConfig & dfgConfig
  )
  : DFGBaseDialog(parent, true, dfgConfig)
  , m_dfgController( dfgController )
{
  m_presetTree =
    new PresetTreeWidget(
      this,
      dfgController,
      dfgConfig,
      true,
      false
      );

  addInput(m_presetTree, "location");
  m_nameEdit = new QLineEdit(name, this);
  addInput(m_nameEdit, "name");

  TreeView::TreeModel * model = m_presetTree->getTreeModel();
  QObject::connect(model, SIGNAL(itemSelected(FabricUI::TreeView::TreeItem*)), 
    this, SLOT(onItemSelected(FabricUI::TreeView::TreeItem*)));
  QObject::connect(m_presetTree->getTreeView(), SIGNAL(customContextMenuRequested(QPoint, FabricUI::TreeView::TreeItem *)), 
    this, SLOT(onCustomContextMenuRequested(QPoint, FabricUI::TreeView::TreeItem *)));
}

DFGSavePresetDialog::~DFGSavePresetDialog()
{
}

QString DFGSavePresetDialog::name() const
{
  return m_nameEdit->text();
}

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

void DFGSavePresetDialog::onCustomContextMenuRequested(QPoint globalPos, FabricUI::TreeView::TreeItem * item)
{
  if(item->type() != "NameSpace")
    return;

  m_contextPath = item->path();
  if(m_contextPath.startsWith("Fabric.") || m_contextPath.startsWith("Variables.") || 
    m_contextPath == "Fabric" || m_contextPath == "Variables")
    return;

  QMenu menu(NULL);
  menu.addAction("Create Folder");

  QObject::connect(&menu, SIGNAL(triggered(QAction*)), this, SLOT(onContextMenuAction(QAction*)));
  menu.exec(globalPos);
}

void DFGSavePresetDialog::onContextMenuAction(QAction * action)
{
  if(action->text() == "Create Folder")
  {
    try
    {
      FabricCore::DFGHost &host = m_dfgController->getCoreDFGHost();

      FTL::StrRef path =
        host.getPresetImportPathname(m_contextPath.toUtf8().constData());

      if(!FTL::FSExists(path))
      {
        QMessageBox msg(QMessageBox::Warning, "Fabric Warning", 
          "Unexpected, path '"+QString(path.data())+"' does not exist!");
        msg.addButton("Ok", QMessageBox::AcceptRole);
        msg.exec();
        return;
      }

      if(!FTL::FSIsDir(path))
      {
        QMessageBox msg(QMessageBox::Warning, "Fabric Warning", 
          "Unexpected, path '"+QString(path.data())+"' is not a directory");
        msg.addButton("Ok", QMessageBox::AcceptRole);
        msg.exec();
        return;
      }

      DFGGetStringDialog dialog(this, "name", m_dfgConfig);
      if(dialog.exec() != QDialog::Accepted)
        return;

      QString name = dialog.text();

      std::string dirToMake = path;
      dirToMake += "/";
      dirToMake += name.toUtf8().constData();

      if(!FTL::FSExists(dirToMake.c_str()))
      {
        if(!FTL::FSMkDir(dirToMake.c_str()))
        {
          QMessageBox msg(QMessageBox::Warning, "Fabric Warning", 
            "Unexpected, cannot create '"+QString(dirToMake.c_str())+"' directory");
          msg.addButton("Ok", QMessageBox::AcceptRole);
          msg.exec();
          return;
        }
      }

      host.createPresetDir(m_contextPath.toUtf8().constData(), name.toUtf8().constData());
      m_presetTree->refresh();
    }
    catch(FabricCore::Exception e)
    {
      printf("Exception: %s\n", e.getDesc_cstr());
    }
  }
}
