//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include <QtGui/QTreeWidget>

namespace FabricUI {
namespace ValueEditor {

class BaseViewItem;
class BaseModelItem;
class VETreeWidgetItem;

class VETreeWidget : public QTreeWidget
{
  Q_OBJECT

public:

  VETreeWidget();
  ~VETreeWidget();

  void reloadStyles();

  void sortTree();
  bool focusNextPrevChild( bool next );

  VETreeWidgetItem* createTreeWidgetItem( BaseViewItem* viewItem, QTreeWidgetItem* parent, int index = -1 );

  VETreeWidgetItem* findTreeWidget( QWidget* widget ) const;
  VETreeWidgetItem* findTreeWidget( QWidget* widget, VETreeWidgetItem * item ) const;

  VETreeWidgetItem* findTreeWidget( BaseModelItem* pItem ) const;
  VETreeWidgetItem* findTreeWidget( BaseModelItem* pItem, VETreeWidgetItem* pWidget ) const;
  VETreeWidgetItem * findTreeWidget( BaseViewItem * pItem ) const;
  VETreeWidgetItem * findTreeWidget( BaseViewItem * pItem, VETreeWidgetItem * pWidget ) const;

  public slots:

  void onSetModelItem( FabricUI::ValueEditor::BaseModelItem* pItem );

  void onModelItemChildInserted( FabricUI::ValueEditor::BaseModelItem* parent, int index, const char* name );
  void onModelItemRemoved( FabricUI::ValueEditor::BaseModelItem* item );
  void onModelItemRenamed( FabricUI::ValueEditor::BaseModelItem* item );
  void onModelItemTypeChanged( FabricUI::ValueEditor::BaseModelItem* item, const char* newType );
  void onModelItemChildrenReordered( FabricUI::ValueEditor::BaseModelItem* parent, const QList<int>& newOrder );

  void onViewItemChildrenRebuild( FabricUI::ValueEditor::BaseViewItem* item );

  // This slot is triggered when an item is edited in the view
  void onItemEdited( QTreeWidgetItem* item, int column );

protected slots:

  void onTreeWidgetItemExpanded( QTreeWidgetItem *_treeWidgetItem );

  void onTreeWidgetItemCollapsed( QTreeWidgetItem *_treeWidgetItem );

  void prepareMenu( const QPoint& pt );

  void resetItem();
};

} // namespace FabricUI 
} // namespace ValueEditor 
