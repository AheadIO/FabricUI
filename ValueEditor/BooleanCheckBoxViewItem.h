//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_VALUEEDITOR_BOOLEANCHECKBOXVIEWITEM_H
#define FABRICUI_VALUEEDITOR_BOOLEANCHECKBOXVIEWITEM_H

#include "BaseViewItem.h"

class QString;
class QVariant;
class QWidget;
class QCheckBox;


namespace FabricUI {
namespace ValueEditor {
  
class ItemMetadata;

class BooleanCheckBoxViewItem : public BaseViewItem
{
  Q_OBJECT

public:

  static BaseViewItem *CreateItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metaData
    );
  static const int Priority;

  BooleanCheckBoxViewItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metadata
    );
  ~BooleanCheckBoxViewItem();

  virtual QWidget *getWidget() /*override*/;

  virtual void onModelValueChanged( QVariant const &value ) /*override*/;
  
  void deleteMe() { delete this; }

private:

  QCheckBox *m_checkBox;

private slots:

  void onStateChanged( int state );
};


} // namespace FabricUI 
} // namespace ValueEditor 

#endif // FABRICUI_VALUEEDITOR_BOOLEANCHECKBOXVIEWITEM_H
