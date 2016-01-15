//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include "BaseComplexViewItem.h"

#include <QtGui/QVector4D>

class QLayout;
class QLineEdit;
class ItemMetadata;

class Vec4ViewItem : public BaseComplexViewItem
{
  Q_OBJECT

private:

  QVector4D m_vec4dValue;

  QWidget *m_widget;
  QLineEdit* m_xEdit;
  QLineEdit* m_yEdit;
  QLineEdit* m_zEdit;
  QLineEdit* m_tEdit;

public:
  
  static BaseViewItem *CreateItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metaData
    );
  static const int Priority;

  Vec4ViewItem(
    QString const &name,
    QVariant const &value
    );
  ~Vec4ViewItem();

  virtual QWidget *getWidget() /*override*/;

  virtual void onModelValueChanged( QVariant const &value ) /*override*/;
  
  virtual void onChildViewValueChanged(
    int index,
    QVariant const &value,
    bool commit
    ) /*override*/;

  virtual void doAppendChildViewItems( QList<BaseViewItem *>& items );

  void deleteMe() { delete this; }

public slots:

  void onTextEditXChanged();
  void onTextEditYChanged();
  void onTextEditZChanged();
  void onTextEditTChanged();
};
