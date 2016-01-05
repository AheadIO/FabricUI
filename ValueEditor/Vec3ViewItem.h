#pragma once

#include "BaseComplexViewItem.h"

class Vec3ViewItem : public BaseComplexViewItem
{
	Q_OBJECT

private:

	QVector3D m_vec3dValue;

	QWidget *m_widget;
	QLayout *m_layout;
	QLineEdit* m_xEdit;
	QLineEdit* m_yEdit;
	QLineEdit* m_zEdit;

public:

	Vec3ViewItem(
		QString const &name,
	  QVariant const &value
	  );
	~Vec3ViewItem();

	virtual QWidget *getWidget() /*override*/;

	virtual void onModelValueChanged( QVariant const &value ) /*override*/;
	
	virtual void onChildViewValueChanged(
		int index,
		QVariant const &value,
		bool commit
		) /*override*/;

  virtual void doAppendChildViewItems( QList<BaseViewItem *>& items );

  void deleteMe() { delete this; }

public slots:

	void onTextEditXChanged();
	void onTextEditYChanged();
	void onTextEditZChanged();
};
