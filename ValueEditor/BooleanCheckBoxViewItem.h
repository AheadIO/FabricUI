//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include "BaseViewItem.h"

class ItemMetadata;
class QString;
class QVariant;
class QWidget;
class QCheckBox;

class BooleanCheckBoxViewItem : public BaseViewItem
{
  Q_OBJECT

public:

  static BaseViewItem *CreateItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metaData
    );
  static const int Priority;

  BooleanCheckBoxViewItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metadata
    );
  ~BooleanCheckBoxViewItem();

  virtual QWidget *getWidget() /*override*/;

  virtual void onModelValueChanged( QVariant const &value ) /*override*/;
  
  void deleteMe() { delete this; }

private:

  QCheckBox *m_checkBox;

private slots:

  void onStateChanged( int state );
};
