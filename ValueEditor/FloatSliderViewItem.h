//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include "BaseViewItem.h"

class DoubleSlider;
class ItemMetadata;

class FloatSliderViewItem : public BaseViewItem
{
  Q_OBJECT

public:
  
  static BaseViewItem *CreateItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metaData
    );
  static const int Priority;

  FloatSliderViewItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metadata
    );
  ~FloatSliderViewItem();

  virtual QWidget *getWidget() /*override*/;

  virtual void onModelValueChanged( QVariant const &value ) /*override*/;

  virtual void metadataChanged( );
  
  void deleteMe() { delete this; }

private:

  DoubleSlider* m_slider;

private slots:

  void onSliderPressed();
  void onDoubleValueChanged( double value );
  void onSliderReleased();
};
