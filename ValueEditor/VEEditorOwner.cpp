//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "VEEditorOwner.h"

#include <FabricUI/ModelItems/BindingModelItem.h>
#include <FabricUI/ModelItems/GetModelItem.h>
#include <FabricUI/ModelItems/InstModelItem.h>
#include <FabricUI/ModelItems/SetModelItem.h>
#include <FabricUI/ModelItems/VarModelItem.h>
#include <FabricUI/ValueEditor/ItemMetadata.h>
#include <FabricUI/ValueEditor/BaseViewItem.h>
#include <FabricUI/ValueEditor/VETreeWidget.h>
#include <FabricUI/ValueEditor/VETreeWidgetItem.h>

using namespace FabricUI;
using namespace FabricUI::ValueEditor;
using namespace ModelItems;

VEEditorOwner::VEEditorOwner()
  : m_valueEditor( NULL )
  , m_modelRoot( NULL )
{
  m_valueEditor = new FabricUI::ValueEditor::VETreeWidget();
}

VEEditorOwner::~VEEditorOwner()
{
  delete m_modelRoot;
}

QWidget* VEEditorOwner::getWidget() const
{
  return m_valueEditor;
}

void VEEditorOwner::initConnections()
{
  connect(
    this, SIGNAL( modelItemInserted( FabricUI::ValueEditor::BaseModelItem*, int, const char* ) ),
    m_valueEditor, SLOT( onModelItemChildInserted( FabricUI::ValueEditor::BaseModelItem*, int, const char* ) )
    );
  connect(
    this, SIGNAL( modelItemInserted( FabricUI::ValueEditor::BaseModelItem*, int, const char* ) ),
    this, SLOT( onModelItemInserted( FabricUI::ValueEditor::BaseModelItem*, int, const char* ) )
    );
  connect(
    this, SIGNAL( modelItemTypeChange( FabricUI::ValueEditor::BaseModelItem*, const char* ) ),
    m_valueEditor, SLOT( onModelItemTypeChanged( FabricUI::ValueEditor::BaseModelItem*, const char* ) )
    );
  connect(
    this, SIGNAL( modelItemRemoved( FabricUI::ValueEditor::BaseModelItem* ) ),
    m_valueEditor, SLOT( onModelItemRemoved( FabricUI::ValueEditor::BaseModelItem* ) )
    );
  connect(
    this, SIGNAL( modelItemRenamed( FabricUI::ValueEditor::BaseModelItem* ) ),
    m_valueEditor, SLOT( onModelItemRenamed( FabricUI::ValueEditor::BaseModelItem* ) )
    );
  connect(
    this, SIGNAL( modelItemChildrenReordered( FabricUI::ValueEditor::BaseModelItem*, const QList<int>& ) ),
    m_valueEditor, SLOT( onModelItemChildrenReordered( FabricUI::ValueEditor::BaseModelItem*, const QList<int>& ) )
    );
  connect(
    this, SIGNAL( replaceModelRoot( FabricUI::ValueEditor::BaseModelItem* ) ),
    m_valueEditor, SLOT( onSetModelItem( FabricUI::ValueEditor::BaseModelItem* ) )
    );
}

void VEEditorOwner::onModelItemInserted( FabricUI::ValueEditor::BaseModelItem* parent, int index, const char* childName )
{
  BaseModelItem * child = parent->getChild(childName);
  if(child)
  {
    connect(
      child, SIGNAL( modelValueChanged( QVariant const & ) ),
      this, SLOT( onModelValueChanged( QVariant const & ) )
      );
  }
}

void VEEditorOwner::onModelValueChanged( QVariant const &newValue )
{
  BaseModelItem * item = (BaseModelItem *)QObject::sender();
  emit modelItemValueChanged(item, newValue);
}
