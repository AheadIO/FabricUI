/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include <iostream>
#include <QtGui/QMessageBox>
#include "SHValueEditor.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::SceneHub;

SHValueEditor::SHValueEditor(
	DFG::DFGController *controller, 
	const DFG::DFGConfig &config,
	SHTreeView *shTreeView)
  : DFG::DFGValueEditor(controller, config)
  , m_shTreeView(shTreeView)
{
  QObject::connect(m_shTreeView, SIGNAL(itemSelected(FabricUI::SceneHub::SHTreeItem *)), 
  	this, SLOT(onSHArgsChanged(FabricUI::SceneHub::SHTreeItem *)));
}
 
void SHValueEditor::clear() {
  DFG::DFGValueEditor::clear();
}

void SHValueEditor::onSHArgsChanged(SHTreeItem *shTreeItem) {
  //clear();
  //FabricCore::RTVal nameVal = 
  //	FabricCore::RTVal::ConstructString(m_controller->getClient(), 
  //		shTreeItem->desc().toStdString().c_str());
  //
  //FabricCore::RTVal sgObjectVal = shTreeItem->getSGObject();
  //ValueItem * nameItem = addValue("Name", nameVal);
  //ValueItem * sgObjectItem = addValue("SGObject", nameVal);
}
