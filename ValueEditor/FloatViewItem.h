//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include "BaseViewItem.h"

class VEDoubleSpinBox;
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
  void deleteMe() { delete this; }

	virtual QWidget *getWidget() /*override*/;
  
	virtual void onModelValueChanged( QVariant const &value ) /*override*/;

  virtual void metadataChanged() /*override*/; 

private:

	VEDoubleSpinBox *m_spinBox;

private slots:

	void onSpinBoxValueChanged(double value);
};
