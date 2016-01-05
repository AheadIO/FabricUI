#pragma once

#include "BaseViewItem.h"

class ViewItemChildRouter;

// BaseComplexViewItem is a helper class that implements a few 
// of the services for defining ViewItems with their own 
// children (eg, Vec3, Color, etc).
class BaseComplexViewItem : public BaseViewItem
{
protected:

  //typedef ViewItemChildRouter ChildRouter;

  // Routers allow us to distinguish which child
  // has changed when recieving messages
  QVector<ViewItemChildRouter*> m_routers;

public:

  BaseComplexViewItem( QString const &name );
  virtual ~BaseComplexViewItem();

  ViewItemChildRouter *createChildRouter( int index );

  // Call this function to connect the child to this class
  bool connectChild( int index, BaseViewItem* child );

  // Returns true - A complex type implies children 
  virtual bool hasChildren() const;

  // This default implementation of appendChildViewItems will
  // first call doAppend to append the local viewitems, then
  // call the parent append.
  void appendChildViewItems( QList<BaseViewItem *>& items );

  // Complex types can in general append their local children
  // via this method and let the system manage adding the parent's items
  virtual void doAppendChildViewItems( QList<BaseViewItem *>& items ) = 0;

  // Implement this slot if we need to react when a child changes
  virtual void onChildViewValueChanged(
    int index,
    QVariant const &value,
    bool commit
    )=0;

  // This function should be called to trigger an update to our children.
  void routeModelValueChanged( int index, const QVariant& value );
};