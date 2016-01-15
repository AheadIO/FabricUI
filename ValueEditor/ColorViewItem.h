//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include "BaseComplexViewItem.h"

#include <QtGui/QColor>

class QPushButton;
class ItemMetadata;

class ColorViewItem : public BaseComplexViewItem
{
private:

  Q_OBJECT

  QPushButton* m_button;
  
  QColor m_color;

public:

  static BaseViewItem *CreateItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metaData
    );
  static const int Priority;

  ColorViewItem( const QVariant& value, const QString& name );
  ~ColorViewItem();

  virtual QWidget *getWidget() /*override*/;

  virtual void onModelValueChanged( QVariant const &value ) /*override*/;

  virtual void onChildViewValueChanged(
    int index,
    QVariant const &value,
    bool commit
    ) /*override*/;

  virtual void doAppendChildViewItems( QList<BaseViewItem *>& items );

  void setButtonColor( const QColor & color );

  void deleteMe() { delete this; }

public slots:

  // Slot triggered when pushing background button
  void pickColor();
  void onColorChanged( QColor color );
  void onColorSelected( QColor color );
};
