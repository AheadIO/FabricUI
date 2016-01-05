#pragma once

class BaseViewItem;

class VETreeWidgetItem : public QTreeWidgetItem
{
public:

  VETreeWidgetItem( BaseViewItem *viewItem );
  ~VETreeWidgetItem();

  BaseViewItem *getViewItem() const;

  // Override the sort to enable changing
  // order without losing all the widget info
  bool operator<( const QTreeWidgetItem &other )const;

private:

  BaseViewItem *m_viewItem;
};