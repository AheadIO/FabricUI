//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include "BaseViewItem.h"

class VESpinBox;
class ItemMetadata;

class FloatViewItem : public BaseViewItem
{
	Q_OBJECT

public:
  
  static BaseViewItem *CreateItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metaData
    );
  static const int Priority;

	FloatViewItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metadata
    );
	~FloatViewItem();

	virtual QWidget *getWidget() /*override*/;
  
	virtual void onModelValueChanged( QVariant const &value ) /*override*/;

  void deleteMe() { delete this; }

private:

	VESpinBox *m_spinBox;

private slots:

	void onSpinBoxValueChanged(double value);
};
