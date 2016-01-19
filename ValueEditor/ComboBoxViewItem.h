//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include "BaseViewItem.h"

class QComboBox;

class ComboBoxViewItem : public BaseViewItem
{
  Q_OBJECT

public:
  
  static BaseViewItem *CreateItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metaData
    );
  static const int Priority;

  ComboBoxViewItem(QString const &name);
  ~ComboBoxViewItem();

  virtual void updateMetadata( ItemMetadata* metaData ) /*override*/;

  virtual QWidget *getWidget() /*override*/;
  
  virtual void onModelValueChanged( QVariant const &value ) /*override*/;

  void deleteMe() { delete this; }

private:

  QComboBox* m_comboBox;

private slots:
  void entrySelected(int index);
};
