//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include "BaseViewItem.h"

class ItemMetadata;
class QString;
class QVariant;
class QWidget;
class QSlider;
class VELineEdit;

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
    QVariant const &value,
    ItemMetadata* metadata
    );
  virtual void deleteMe() /*override*/
    { delete this; }
  ~IntSliderViewItem();

  virtual QWidget *getWidget() /*override*/;

  virtual void onModelValueChanged( QVariant const &value ) /*override*/;

  virtual void metadataChanged( );
  
private:

  QWidget *m_widget;
  VELineEdit *m_lineEdit;
  QSlider *m_slider;

private slots:

  void onLineEditTextModified( QString text );

  void onSliderPressed();
  void onValueChanged( int value );
  void onSliderReleased();
};
