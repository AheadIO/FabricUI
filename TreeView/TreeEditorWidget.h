// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_TreeView_TreeEditorWidget__
#define __UI_TreeView_TreeEditorWidget__

#include <QtGui/QWidget>

namespace FabricUI
{

  namespace TreeView
  {
    // forward declarations
    class WidgetTreeItem;
    class TreeItemDelegate;

    class TreeEditorWidget : public QWidget
    {
      Q_OBJECT

      friend class TreeItemFactory;
      friend class WidgetTreeItem;

    public:

      typedef TreeEditorWidget * (*CreatorFunc)(QWidget * parent, WidgetTreeItem * item);
      typedef bool (*CanDisplayFunc)(WidgetTreeItem * item);

      TreeEditorWidget(QWidget * parent);

      WidgetTreeItem * item();
      void setItem(WidgetTreeItem * item);
      bool itemSelected() const;
      bool itemExpanded() const;
      bool itemCollapsed() const;

    public slots:
      void changeData();
      virtual void setFocusToFirstInput() = 0;

    signals:
      void dataChanged();
      void itemChanged();
      void selected(bool);
      void expanded(bool);

    private:

      WidgetTreeItem * m_item;
      TreeEditorWidget * m_source;
      bool m_changingData;
    };

  };

};

#endif // __UI_TreeView_TreeEditorWidget__
