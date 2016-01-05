#pragma once
#include <QtGui/QTreeWidget>

class BaseViewItem;
class BaseModelItem;
class VETreeWidgetItem;

class VALUEEDIT_API VETreeWidget : public QTreeWidget
{
  Q_OBJECT

public:

  VETreeWidget( );

  void createTreeWidgetItem( BaseViewItem* viewItem, QTreeWidgetItem* parent, int index = -1 );

  VETreeWidgetItem* findTreeWidget( BaseModelItem* pItem ) const;
  VETreeWidgetItem* findTreeWidget( BaseModelItem* pItem, VETreeWidgetItem* pWidget ) const;
public slots:

  void onSetModelItem( BaseModelItem* pItem );

  void onModelItemChildInserted( BaseModelItem* parent, int index, const char* name );
  void onModelItemRemoved( BaseModelItem* item );
  void onModelItemTypeChanged( BaseModelItem* item, const char* newType );
  void onModelItemChildrenReordered( BaseModelItem* parent, const QList<int>& newOrder );

protected slots:

  void onTreeWidgetItemExpanded( QTreeWidgetItem *_treeWidgetItem );

  void onTreeWidgetItemCollapsed( QTreeWidgetItem *_treeWidgetItem );
};
