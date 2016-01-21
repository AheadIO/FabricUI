//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include "BaseComplexViewItem.h"

#include <QtGui/QVector2D>

class QLayout;
class VESpinBox;
class ItemMetadata;

class Vec2ViewItem : public BaseComplexViewItem
{
  Q_OBJECT

private:

  QVector2D m_vec2dValue;

  QWidget *m_widget;
  VESpinBox* m_xSpinBox;
  VESpinBox* m_ySpinBox;

public:
  
  static BaseViewItem *CreateItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metaData
    );
  static const int Priority;

  Vec2ViewItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metadata
    );
  ~Vec2ViewItem();
  virtual void deleteMe() /*override*/ { delete this; }

  virtual QWidget *getWidget() /*override*/;

  virtual void onModelValueChanged( QVariant const &value ) /*override*/;
  
  virtual void onChildViewValueChanged(
    int index,
    QVariant value
    ) /*override*/;

  virtual void doAppendChildViewItems( QList<BaseViewItem *>& items );

  virtual void metadataChanged() /*override*/; 

public slots:

  void onXSpinBoxValueChanged( double value );
  void onYSpinBoxValueChanged( double value );
};
