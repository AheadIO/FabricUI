//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_VALUEEDITOR_COMBOBOXVIEWITEM_H
#define FABRICUI_VALUEEDITOR_COMBOBOXVIEWITEM_H

#include "BaseViewItem.h"


namespace FabricUI {
namespace ValueEditor {

class ComboBox;

class ComboBoxViewItem : public BaseViewItem
{
  Q_OBJECT

public:
  
  static BaseViewItem *CreateItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metaData
    );
  static const int Priority;

  ComboBoxViewItem(QString const &name, QVariant const &v, ItemMetadata* metadata, bool isString );
  ~ComboBoxViewItem();

  virtual void metadataChanged( );

  virtual QWidget *getWidget() /*override*/;
  
  virtual void onModelValueChanged( QVariant const &value ) /*override*/;

  void deleteMe() { delete this; }

private:

  ComboBox* m_comboBox;
  bool m_isString;

private slots:
  void entrySelected(int index);
};

} // namespace FabricUI 
} // namespace ValueEditor 

#endif // FABRICUI_VALUEEDITOR_COMBOBOXVIEWITEM_H
