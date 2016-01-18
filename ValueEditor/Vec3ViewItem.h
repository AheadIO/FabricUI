//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include "BaseComplexViewItem.h"

#include <QtGui/QVector3D>

class QLayout;
class VESpinBox;
class ItemMetadata;

class Vec3ViewItem : public BaseComplexViewItem
{
	Q_OBJECT

private:

	QVector3D m_vec3dValue;

	QWidget *m_widget;
	VESpinBox* m_xSpinBox;
	VESpinBox* m_ySpinBox;
	VESpinBox* m_zSpinBox;

public:
  
  static BaseViewItem *CreateItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metaData
    );
  static const int Priority;

	Vec3ViewItem(
		QString const &name,
	  QVariant const &value
	  );
	~Vec3ViewItem();

	virtual QWidget *getWidget() /*override*/;

	virtual void onModelValueChanged( QVariant const &value ) /*override*/;
	
	virtual void onChildViewValueChanged(
		int index,
		QVariant value
		) /*override*/;

  virtual void doAppendChildViewItems( QList<BaseViewItem *>& items );

  void deleteMe() { delete this; }

public slots:

	void onXSpinBoxValueChanged( double value );
	void onYSpinBoxValueChanged( double value );
	void onZSpinBoxValueChanged( double value );
};
