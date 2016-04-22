//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_VALUEEDITOR_UINTVIEWITEM_H
#define FABRICUI_VALUEEDITOR_UINTVIEWITEM_H

#include "BaseViewItem.h"

class QSpinBox;


namespace FabricUI {
namespace ValueEditor {
  
class ItemMetadata;

class UIntViewItem : public BaseViewItem
{
  Q_OBJECT

public:
  
  static BaseViewItem *CreateItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metaData
    );
  static const int Priority;

  UIntViewItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metadata
    );
  ~UIntViewItem();

  virtual QWidget *getWidget() /*override*/;
  
  virtual void onModelValueChanged( QVariant const &value ) /*override*/;

  void deleteMe() { delete this; }

private:

  QSpinBox* m_spinner;

private slots:
  void OnSpinnerChanged(int value);
  void OnEditFinished();
};

} // namespace FabricUI 
} // namespace ValueEditor 

#endif // FABRICUI_VALUEEDITOR_UINTVIEWITEM_H
