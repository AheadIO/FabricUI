// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_TreeView_WidgetTreeItem__
#define __UI_TreeView_WidgetTreeItem__

#include "TreeItem.h"
#include "TreeEditorWidget.h"

namespace FabricUI
{

  namespace TreeView
  {
    // forward decls
    class TreeEditorFactory;

    class WidgetTreeItem : public TreeItem
    {
      Q_OBJECT

      friend class TreeEditorFactory;

    public:

      WidgetTreeItem(QString name, TreeEditorFactory * factory, QString type = "widget", QString label = "", QWidget * parent = NULL);
      virtual ~WidgetTreeItem();

      virtual TreeItemType itemType() const;
      virtual QSize sizeHint(const QStyleOptionViewItem & option) const;
      virtual void paint(QPainter * painter, const QStyleOptionViewItem & option) const;

      virtual void onUIChanged() {};
      virtual void onDataChanged();

    private slots:

      void onEditorDestroyed(QObject * obj);
      void updateFromUI();
      void updatePixmap() const;

    protected:

      TreeEditorWidget * editor();
      virtual void setEditor(TreeEditorWidget * editor);
      virtual void setSelected(bool state);
      virtual void setExpanded(bool state);
      
      mutable bool m_needsUpdate;

    private:

      TreeEditorWidget * createTempEditor() const;

      mutable QPixmap * m_pixmap;
      bool m_editable;
      mutable TreeEditorWidget * m_editor;
      TreeEditorFactory * m_factory;
      mutable QSize m_minimumSize;
      QWidget * m_parent;
    };

  };

};

#endif // __UI_TreeView_WidgetTreeItem__
