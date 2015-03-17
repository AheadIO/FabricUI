// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_TreeView_TreeEditorFactory__
#define __UI_TreeView_TreeEditorFactory__

#include "WidgetTreeItem.h"
#include "TreeEditorWidget.h"

namespace FabricUI
{

  namespace TreeView
  {
    class TreeEditorFactory
    {
    public:

      TreeEditorFactory();
      virtual ~TreeEditorFactory();

      virtual TreeEditorWidget * createEditor(QWidget * parent, WidgetTreeItem * item) const;
      virtual void registerEditor(TreeEditorWidget::CreatorFunc creator, TreeEditorWidget::CanDisplayFunc canDisplay);

    private:

      struct Editor
      {
        TreeEditorWidget::CreatorFunc creator;
        TreeEditorWidget::CanDisplayFunc canDisplay;
      };
      std::vector<Editor> m_editors;
    };

  };

};

#endif // __UI_TreeView_TreeEditorFactory__
