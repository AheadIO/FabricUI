//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include "BaseComplexViewItem.h"

#include <QtGui/QVector2D.h>

class QLayout;
class QLineEdit;
class ItemMetadata;

class Vec2ViewItem : public BaseComplexViewItem
{
  Q_OBJECT

private:

  QVector2D m_vec2dValue;

  QWidget *m_widget;
  QLineEdit* m_xEdit;
  QLineEdit* m_yEdit;

public:
  
  static BaseViewItem *CreateItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metaData
    );
  static const int Priority;

  Vec2ViewItem(
    QString const &name,
    QVariant const &value
    );
  ~Vec2ViewItem();

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
};
