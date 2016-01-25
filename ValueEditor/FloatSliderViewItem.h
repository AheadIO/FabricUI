//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include "BaseViewItem.h"

class DoubleSlider;
class ItemMetadata;
class QWidget;
class VELineEdit;

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
  virtual void deleteMe() /*override*/
    { delete this; }
  ~FloatSliderViewItem();

  virtual QWidget *getWidget() /*override*/;

  virtual void onModelValueChanged( QVariant const &value ) /*override*/;

  virtual void metadataChanged() /*override*/; 

private:

  QWidget *m_widget;
  VELineEdit *m_lineEdit;
  DoubleSlider *m_slider;

private slots:

  void onLineEditTextModified( QString text );

  void onSliderPressed();
  void onDoubleValueChanged( double value );
  void onSliderReleased();
};
