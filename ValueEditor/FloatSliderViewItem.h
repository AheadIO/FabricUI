//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include "BaseViewItem.h"

class DoubleSlider;

class FloatSliderViewItem : public BaseViewItem
{
  Q_OBJECT

public:
  
  static BaseViewItem *CreateItem(
    QString const &name,
    QVariant const &value,
    FTL::JSONObject* /*metaData*/
    );
  static const int Priority;

  FloatSliderViewItem(
    QString const &name,
    QVariant const &value
    );
  ~FloatSliderViewItem();

  virtual QWidget *getWidget() /*override*/;

  virtual void onModelValueChanged( QVariant const &value ) /*override*/;

  virtual void updateMetadata( FTL::JSONObject* /*metaData*/ );
  
  void deleteMe() { delete this; }

private:

  DoubleSlider* m_slider;

private slots:
  void OnSpinnerChanged( double value );
  void OnEditFinished();
};
