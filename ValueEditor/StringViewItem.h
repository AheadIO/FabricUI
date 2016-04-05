//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include "BaseViewItem.h"

class QWidget;


namespace FabricUI {
namespace ValueEditor {
  
class VELineEdit;

class StringViewItem : public BaseViewItem
{
	Q_OBJECT

public:

  static BaseViewItem *CreateItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metaData
    );
  static const int Priority;

	StringViewItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metadata
    );
	~StringViewItem();

	virtual QWidget *getWidget() /*override*/;
  
	virtual void onModelValueChanged( QVariant const &value ) /*override*/;

  void deleteMe() { delete this; }

private:

	VELineEdit* m_edit;
  QWidget* m_widget;

protected slots:
	
	void onTextModified( QString text );
  void onInspect();
};

} // namespace FabricUI 
} // namespace ValueEditor 
