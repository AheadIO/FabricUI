//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include "BaseComplexViewItem.h"

#include <QtGui/QVector4D>

class QLayout;
class VESpinBox;
class ItemMetadata;

class Vec4ViewItem : public BaseComplexViewItem
{
  Q_OBJECT

private:

  QVector4D m_vec4dValue;

  QWidget *m_widget;
  VESpinBox* m_xSpinBox;
  VESpinBox* m_ySpinBox;
  VESpinBox* m_zSpinBox;
  VESpinBox* m_tSpinBox;

public:
  
  static BaseViewItem *CreateItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metaData
    );
  static const int Priority;

  Vec4ViewItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metadata
    );
  virtual void deleteMe() /*override*/
    { delete this; }
  ~Vec4ViewItem();

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
  void onZSpinBoxValueChanged( double value );
  void onTSpinBoxValueChanged( double value );
};
