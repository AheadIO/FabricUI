/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef __FABRICUI_SceneHub_SHTreeView_h
#define __FABRICUI_SceneHub_SHTreeView_h

#include <QtCore/QAbstractItemModel>
#include <QtGui/QMenu>
#include <QtGui/QTreeView>
#include <FTL/OwnedPtr.h>
#include <FTL/SharedPtr.h>
#include <FabricCore.h>
#include <vector>
#include <assert.h>
#include <iostream>

#include "SHTreeModel.h"

namespace FabricUI
{
  namespace SceneHub
  {
    class SHTreeView;

    class SHTreeView_ViewIndexTarget : public QObject
    {
      Q_OBJECT

    public:

      SHTreeView_ViewIndexTarget(
        SHTreeView *view,
        QModelIndex const &index,
        QObject *parent
        )
        : QObject( parent )
        , m_view( view )
        , m_index( index )
        {}

    public slots:

      void expandRecursively()
        { expandRecursively( m_index ); }

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

      SHTreeView( QWidget *parent = 0 )
        : QTreeView( parent )
      {
        setHeaderHidden( true );
        setSelectionMode( QAbstractItemView::SingleSelection );
        setContextMenuPolicy( Qt::CustomContextMenu );
        connect(
          this, SIGNAL(customContextMenuRequested(const QPoint &)),
          this, SLOT(onCustomContextMenu(const QPoint &))
          );
      }

      void setSelectedObjects( FabricCore::RTVal selectedSGObjectArray );

    signals:
      
      void itemSelected( FabricUI::SceneHub::SHTreeItem *item );
      void itemDeselected( FabricUI::SceneHub::SHTreeItem *item );

    public slots:

      void onCustomContextMenu( const QPoint &point )
      {
        QModelIndex index = indexAt( point );
        if ( index.isValid() )
        {
          QMenu *menu = new QMenu(this);

          SHTreeView_ViewIndexTarget *viewIndexTarget =
            new SHTreeView_ViewIndexTarget( this, index, menu );

          QAction *expandAction = new QAction( "Expand recursively", 0 );
          connect(
            expandAction, SIGNAL(triggered()),
            viewIndexTarget, SLOT(expandRecursively())
            );
          menu->addAction( expandAction );

          QAction *loadAction = new QAction( "Load recursively", 0 );
          connect(
            loadAction, SIGNAL(triggered()),
            viewIndexTarget, SLOT(loadRecursively())
            );
          menu->addAction( loadAction );

          menu->popup( mapToGlobal( point ) );
        }
      }

    protected:

      void selectionChanged(
        const QItemSelection &selected,
        const QItemSelection &deselected
        )
      {
        QTreeView::selectionChanged( selected, deselected );
        foreach ( QModelIndex index, deselected.indexes() )
        {
          SHTreeItem *item =
            static_cast<SHTreeItem *>( index.internalPointer() );
          emit itemDeselected( item );
        }

        foreach ( QModelIndex index, selected.indexes() )
        {
          SHTreeItem *item =
            static_cast<SHTreeItem *>( index.internalPointer() );
          emit itemSelected( item );
        }
      }
    };
  };
};

#endif // __FABRIC_SceneHubStandalone_SHTree_h
