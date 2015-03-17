// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_ValueEditor_ValueWidgetFactory__
#define __UI_ValueEditor_ValueWidgetFactory__

#include <TreeView/TreeEditorFactory.h>

namespace FabricUI
{

  namespace ValueEditor
  {
    class ValueWidgetFactory : public TreeView::TreeEditorFactory
    {
    public:

      ValueWidgetFactory();
      virtual TreeView::TreeEditorWidget * createEditor(QWidget * parent, TreeView::WidgetTreeItem * item) const;

    };

  };

};

#endif // __UI_ValueEditor_ValueWidgetFactory__
