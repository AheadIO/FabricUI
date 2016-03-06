/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_SHTREEVIEW_H__
#define __UI_SCENEHUB_SHTREEVIEW_H__

#include <QtGui/QMenu>
#include <QtGui/QDrag>
#include <QtGui/QTreeView>
#include <QtGui/QMouseEvent>
#include <FabricCore.h>
#include <FTL/OwnedPtr.h>
#include <FTL/SharedPtr.h>
#include <FabricUI/SceneHub/SHHotkeys.h>
#include <FabricUI/SceneHub/TreeView/SHTreeModel.h>


namespace FabricUI
{
  namespace SceneHub
  {
    class SHTreeView;

    class SHTreeView_ViewIndexTarget : public QObject
    {
      Q_OBJECT

      public:
        SHTreeView_ViewIndexTarget(SHTreeView *view, QModelIndex const &index, QObject *parent)
          : QObject( parent )
          , m_view( view )
          , m_index( index )
          {}

      public slots:
        void expandRecursively() { expandRecursively( m_index ); }
        void loadRecursively();

      protected:
        void expandRecursively( QModelIndex const &index );

      private:
        SHTreeView *m_view;
        QModelIndex m_index;
    };

    class SHTreeView : public QTreeView
    {
      Q_OBJECT

      public:
        SHTreeView(FabricCore::Client &client, QWidget *parent = 0 );
        void setSelectedObjects( FabricCore::RTVal selectedSGObjectArray );
        virtual void mousePressEvent(QMouseEvent *event);
     
      signals:
        void itemSelected( FabricUI::SceneHub::SHTreeItem *item );
        void itemDeselected( FabricUI::SceneHub::SHTreeItem *item );
        void addExternalFile(QStringList, bool);

      public slots:
        void onCustomContextMenu( const QPoint &point );

      protected:
        void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
      
      private:
        /// \internal
        /// Reference to the client -> construct RTVal.
        FabricCore::Client m_client;
    };
  }
}

#endif // __UI_SCENEHUB_SHTREEVIEW_H__
