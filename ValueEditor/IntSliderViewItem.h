#pragma once

#include "BaseViewItem.h"

class ItemMetadata;
class QString;
class QVariant;
class QWidget;
class QSlider;

class IntSliderViewItem : public BaseViewItem
{
  Q_OBJECT

public:

  static BaseViewItem *CreateItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metaData
    );
  static const int Priority;

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
