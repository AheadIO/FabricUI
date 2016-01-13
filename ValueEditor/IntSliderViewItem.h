#pragma once

#include "BaseViewItem.h"

class DoubleSlider;

class IntSliderViewItem : public BaseViewItem
{
  Q_OBJECT

public:

  IntSliderViewItem(
    QString const &name,
    QVariant const &value
    );
  ~IntSliderViewItem();

  virtual QWidget *getWidget() /*override*/;

  virtual void onModelValueChanged( QVariant const &value ) /*override*/;

  virtual void updateMetadata( ItemMetadata* metaData );
  
  void deleteMe() { delete this; }

private:

  QSlider* m_slider;

private slots:
  void OnSpinnerChanged( int value );
  void OnEditFinished();
};
