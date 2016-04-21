//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_VALUEEDITOR_ARRAYVIEWITEM_H
#define FABRICUI_VALUEEDITOR_ARRAYVIEWITEM_H

#include "BaseComplexViewItem.h"
#include <FabricCore.h>

class QWidget;


namespace FabricUI {
namespace ValueEditor {

class VEIntSpinBox;

// This implementation supports displaying RTVal arrays 
class ArrayViewItem : public BaseComplexViewItem
{
  Q_OBJECT
  // Cache the core value.  Note - this is a reference, not a copy
  FabricCore::RTVal m_val;

  // We cache the limits on the array items we are displaying
  int m_min;
  int m_max;

  // A composite widget to represent any/all values in UI
  QWidget *m_widget;
  VEIntSpinBox *m_minIndexEdit;
  VEIntSpinBox *m_maxIndexEdit;
  VEIntSpinBox *m_arraySizeEdit;

public:

  static BaseViewItem* CreateItem( QString const &name, 
                                   QVariant const &value, 
                                   ItemMetadata* metadata );
  static const int Priority;

  ArrayViewItem( QString name,
                 const FabricCore::RTVal& value,
                 ItemMetadata* metadata );
  ~ArrayViewItem();

  virtual void doAppendChildViewItems( QList<BaseViewItem *>& items ) /*override*/;

  virtual void onChildViewValueChanged( int index, QVariant value ) /*override*/;

  virtual QWidget * getWidget() /*override*/;

  virtual void deleteMe() /*override*/ { delete this; };

  virtual void onModelValueChanged( QVariant const &value ) /*override*/;

private:

  void updateWidgets();

  // Update min and max vars based on current
  // lineedit values and the current m_val
  //bool updateAndLimitMinMax();

public slots:

  void onMinIndexChanged( int value );
  void onMaxIndexChanged( int value );
  void onArraySizeChanged( int value );
};

} // namespace FabricUI 
} // namespace ValueEditor 

#endif // FABRICUI_VALUEEDITOR_ARRAYVIEWITEM_H
