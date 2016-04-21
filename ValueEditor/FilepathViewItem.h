//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_VALUEEDITOR_FILEPATHVIEWITEM_H
#define FABRICUI_VALUEEDITOR_FILEPATHVIEWITEM_H

#include "BaseViewItem.h"
#include <FabricCore.h>

class QWidget;

namespace FabricUI {
namespace ValueEditor {

class VELineEdit;
class ItemMetadata;

class FilepathViewItem : public BaseViewItem
{
  Q_OBJECT

    FabricCore::RTVal m_val;

public:

  static BaseViewItem *CreateItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metaData
    );
  static const int Priority;

  FilepathViewItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metadata
    );
  ~FilepathViewItem();

  QWidget *getWidget() /*override*/;
  
  void onModelValueChanged( QVariant const &value ) /*override*/;

  void deleteMe() /*override*/ { delete this; }

  void metadataChanged( );


private:

  VELineEdit* m_edit;
  QWidget *m_widget;
  QString m_filter;

private slots:

  void onTextModified( QString text );
  void doBrowse();
};

} // namespace FabricUI 
} // namespace ValueEditor 

#endif // FABRICUI_VALUEEDITOR_FILEPATHVIEWITEM_H
