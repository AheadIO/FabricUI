//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include "BaseComplexViewItem.h"

#include <FabricCore.h>

class QLabel;

// A default RTVal implementation that can display any RTVal class
class RTValViewItem : public BaseComplexViewItem
{
	// Cache the val for easier reconstruction.  Why?  cause why not?
	FabricCore::RTVal m_val; 

  // A composite widget to represent any/all values in UI
	QLabel *m_widget;

  bool m_isEditableType;

public:
  
  static BaseViewItem *CreateItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metaData
    );
  static const int Priority;

	RTValViewItem(QString name, const FabricCore::RTVal& value );
	~RTValViewItem();

  virtual bool hasChildren() const /*override*/;


  virtual QWidget *getWidget() /*override*/;

  virtual void onModelValueChanged( QVariant const &value ) /*override*/;

  virtual void doAppendChildViewItems( QList<BaseViewItem *>& items );

  virtual void onChildViewValueChanged(
    int index,
    QVariant value
    ) /*override*/;

  void deleteMe() { delete this; }

private:

  void UpdateWidget();
};
