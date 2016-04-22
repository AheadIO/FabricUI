//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_VALUEEDITOR_SINTVIEWITEM_H
#define FABRICUI_VALUEEDITOR_SINTVIEWITEM_H

#include "BaseViewItem.h"

class QSpinBox;


namespace FabricUI {
namespace ValueEditor {
  
class ItemMetadata;

class SIntViewItem : public BaseViewItem
{
  Q_OBJECT

public:
  
  static BaseViewItem *CreateItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metaData
    );
  static const int Priority;

  SIntViewItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metadata
    );
  ~SIntViewItem();

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

#endif // FABRICUI_VALUEEDITOR_SINTVIEWITEM_H
