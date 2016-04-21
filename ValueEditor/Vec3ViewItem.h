//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_VALUEEDITOR_VEC3VIEWITEM_H
#define FABRICUI_VALUEEDITOR_VEC3VIEWITEM_H

#include "BaseComplexViewItem.h"
#include <QtGui/QVector3D>

class QLayout;


namespace FabricUI {
namespace ValueEditor {
	
class VEDoubleSpinBox;
class ItemMetadata;

class Vec3ViewItem : public BaseComplexViewItem
{
	Q_OBJECT

private:

	QVector3D m_vec3dValue;

	QWidget *m_widget;
	VEDoubleSpinBox* m_xSpinBox;
	VEDoubleSpinBox* m_ySpinBox;
	VEDoubleSpinBox* m_zSpinBox;

public:
  
  static BaseViewItem *CreateItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metaData
    );
  static const int Priority;

	Vec3ViewItem(
		QString const &name,
    QVariant const &value,
    ItemMetadata* metadata
	  );
  virtual void deleteMe() /*override*/
    { delete this; }
	~Vec3ViewItem();

	virtual QWidget *getWidget() /*override*/;

	virtual void onModelValueChanged( QVariant const &value ) /*override*/;
	
	virtual void onChildViewValueChanged(
		int index,
		QVariant value
		) /*override*/;

  virtual void doAppendChildViewItems( QList<BaseViewItem *>& items );

  virtual void metadataChanged() /*override*/; 

public slots:

	void onXSpinBoxValueChanged( double value );
	void onYSpinBoxValueChanged( double value );
	void onZSpinBoxValueChanged( double value );
};

} // namespace FabricUI 
} // namespace ValueEditor 

#endif // FABRICUI_VALUEEDITOR_VEC3VIEWITEM_H
