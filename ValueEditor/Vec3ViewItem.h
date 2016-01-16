//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include "BaseComplexViewItem.h"

#include <QtGui/QVector3D>

class QLayout;
class VELineEdit;
class ItemMetadata;

class Vec3ViewItem : public BaseComplexViewItem
{
	Q_OBJECT

private:

	QVector3D m_vec3dValue;

	QWidget *m_widget;
	VELineEdit* m_xEdit;
	VELineEdit* m_yEdit;
	VELineEdit* m_zEdit;

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

	void onTextEditXModified( QString text );
	void onTextEditYModified( QString text );
	void onTextEditZModified( QString text );
};
