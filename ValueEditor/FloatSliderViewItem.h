//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_VALUEEDITOR_FLOATSLIDERVIEWITEM_H
#define FABRICUI_VALUEEDITOR_FLOATSLIDERVIEWITEM_H

#include "BaseViewItem.h"

class QWidget;


namespace FabricUI {
namespace ValueEditor {

class DoubleSlider;
class ItemMetadata;
class VELineEdit;

#define FLOAT_SLIDER_DECIMALS 2

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
  double m_softMinimum;
  double m_softMaximum;
  double m_hardMinimum;
  double m_hardMaximum;
  bool m_isSettingValue;

private slots:

  void onLineEditTextModified( QString text );

  void onSliderPressed();
  void onDoubleValueChanged( double value );
  void onSliderReleased();
};

} // namespace FabricUI 
} // namespace ValueEditor 

#endif // FABRICUI_VALUEEDITOR_FLOATSLIDERVIEWITEM_H
