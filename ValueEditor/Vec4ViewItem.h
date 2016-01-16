//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include "BaseComplexViewItem.h"

#include <QtGui/QVector4D>

class QLayout;
class VELineEdit;
class ItemMetadata;

class Vec4ViewItem : public BaseComplexViewItem
{
  Q_OBJECT

private:

  QVector4D m_vec4dValue;

  QWidget *m_widget;
  VELineEdit* m_xEdit;
  VELineEdit* m_yEdit;
  VELineEdit* m_zEdit;
  VELineEdit* m_tEdit;

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
    QVariant value
    ) /*override*/;

  virtual void doAppendChildViewItems( QList<BaseViewItem *>& items );

  void deleteMe() { delete this; }

public slots:

  void onTextEditXModified( QString text );
  void onTextEditYModified( QString text );
  void onTextEditZModified( QString text );
  void onTextEditTModified( QString text );
};
