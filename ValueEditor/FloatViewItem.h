#pragma once

#include "BaseViewItem.h"

class QDoubleSpinBox;

class FloatViewItem : public BaseViewItem
{
	Q_OBJECT

public:

	FloatViewItem(
    QString const &name,
    QVariant const &value
    );
	~FloatViewItem();

	virtual QWidget *getWidget() /*override*/;
  
	virtual void onModelValueChanged( QVariant const &value ) /*override*/;

  void deleteMe() { delete this; }

private:

	QDoubleSpinBox* m_spinner;

private slots:
	void OnSpinnerChanged(double value);
	void OnEditFinished();
};
