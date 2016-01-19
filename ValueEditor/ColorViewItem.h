//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include "BaseComplexViewItem.h"

#include <QtGui/QColor>

class QPushButton;
class ItemMetadata;
class QComboBox;

class ColorViewItem : public BaseComplexViewItem
{
private:

  Q_OBJECT

  QWidget* m_widget;
  QPushButton* m_button;

  QColor m_color;
  // We store the desired format of
  // the color (HSV or RGB)
  QColor::Spec m_spec;
  QComboBox* m_specCombo;
  

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
    QVariant value
    ) /*override*/;

  virtual void doAppendChildViewItems( QList<BaseViewItem *>& items );

  void setButtonColor( const QColor & color );

  void deleteMe() { delete this; }

  virtual void updateMetadata( ItemMetadata* metaData ) /*override*/;

public slots:

  // Slot triggered when pushing background button
  void pickColor();
  void onColorChanged( QColor color );
  void onColorSelected( QColor color );

  void formatChanged( const QString& format );
};
