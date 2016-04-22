//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_VALUEEDITOR_BASECOMPLEXVIEWITEM_H
#define FABRICUI_VALUEEDITOR_BASECOMPLEXVIEWITEM_H

#include <QtCore/QVector>

#include "BaseViewItem.h"

namespace FabricUI {
namespace ValueEditor {

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

  BaseComplexViewItem( QString const &name, ItemMetadata* metadata );
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
  virtual void doAppendChildViewItems( QList<BaseViewItem *>& items ) {};

  void onChildInteractionBegin( int index )
    { emit interactionBegin(); }

  // Implement this slot if we need to react when a child changes
  virtual void onChildViewValueChanged( int index, QVariant value ) {};

  void onChildInteractionEnd( int index, bool accept )
    { emit interactionEnd( accept ); }

  // This function should be called to trigger an update to our children.
  void routeModelValueChanged( int index, const QVariant& value );
};

} // namespace FabricUI 
} // namespace ValueEditor 

#endif // FABRICUI_VALUEEDITOR_BASECOMPLEXVIEWITEM_H
