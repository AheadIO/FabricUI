// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "ValueWidgetFactory.h"

#include "ValueWidget.h"
#include "ValueItem.h"
#include "BooleanValueWidget.h"
#include "StringValueWidget.h"
#include "FilePathValueWidget.h"
#include "IntegerValueWidget.h"
#include "ScalarValueWidget.h"
#include "Vec2ValueWidget.h"
#include "Vec3ValueWidget.h"
#include "Vec4ValueWidget.h"
#include "ColorValueWidget.h"

using namespace FabricUI;
using namespace FabricUI::ValueEditor;

ValueWidgetFactory::ValueWidgetFactory()
{
  // register all known editors
  registerEditor(&ValueWidget::creator, &ValueWidget::canDisplay);
  registerEditor(&BooleanValueWidget::creator, &BooleanValueWidget::canDisplay);
  registerEditor(&StringValueWidget::creator, &StringValueWidget::canDisplay);
  registerEditor(&FilePathValueWidget::creator, &FilePathValueWidget::canDisplay);
  registerEditor(&IntegerValueWidget::creator, &IntegerValueWidget::canDisplay);
  registerEditor(&ScalarValueWidget::creator, &ScalarValueWidget::canDisplay);
  registerEditor(&Vec2ValueWidget::creator, &Vec2ValueWidget::canDisplay);
  registerEditor(&Vec3ValueWidget::creator, &Vec3ValueWidget::canDisplay);
  registerEditor(&Vec4ValueWidget::creator, &Vec4ValueWidget::canDisplay);
  registerEditor(&ColorValueWidget::creator, &ColorValueWidget::canDisplay);
}

TreeView::TreeEditorWidget * ValueWidgetFactory::createEditor(QWidget * parent, TreeView::WidgetTreeItem * item) const
{
  TreeView::TreeEditorWidget * widget = TreeView::TreeEditorFactory::createEditor(parent, item);
  ValueWidget * valueWidget = qobject_cast<ValueWidget*>(widget);
  if(valueWidget)
  {
    ValueItem * valueItem = qobject_cast<ValueItem*>(item);
    if(valueItem)
    {
      valueWidget->setEnabled(valueItem->enabled());
      QObject::connect(
        valueWidget, SIGNAL(beginInteraction(ValueItem*)),
        valueItem, SLOT(onBeginInteraction(ValueItem*))
        );
      QObject::connect(
        valueWidget, SIGNAL(endInteraction(ValueItem*)),
        valueItem, SLOT(onEndInteraction(ValueItem*))
        );
    }
  }
  return widget;
}
