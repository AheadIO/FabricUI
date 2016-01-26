//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include "BaseViewItem.h"

class QLabel;

class NotInspectableViewItem : public BaseViewItem
{
  QLabel *m_label;

public:
  
  static BaseViewItem *CreateItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metaData
    );
  static const int Priority;

  NotInspectableViewItem(
    QString name,
    QVariant value,
    ItemMetadata* metadata
    );
  virtual void deleteMe() /*override*/
    { delete this; }
  ~NotInspectableViewItem();

  virtual QWidget *getWidget() /*override*/;

  virtual void onModelValueChanged( QVariant const &value ) /*override*/;

  virtual bool hasChildren() const /*override*/;
  virtual void appendChildViewItems( QList<BaseViewItem *>& items) /*override*/;

private:

  void UpdateWidget();
};
