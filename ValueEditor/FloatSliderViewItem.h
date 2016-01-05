#pragma once

#include "BaseViewItem.h"

class DoubleSlider;

class FloatSliderViewItem : public BaseViewItem
{
  Q_OBJECT

public:

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
