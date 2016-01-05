#pragma once

#include "BaseViewItem.h"

class BaseModelItem;
class QLabel;

class DefaultViewItem : public BaseViewItem
{
private:
	
  QLabel *m_label;

public:

	DefaultViewItem(
    QString const &name,
    QVariant const &value
    );
	~DefaultViewItem();

	virtual QWidget *getWidget() /*override*/;

	virtual void onModelValueChanged( QVariant const &value ) /*override*/;

	virtual void onChildViewValueChanged(
    QVariant const &,
    QString const &,
    bool
    ) /*override*/
	  {}

  void deleteMe() { delete this; }
};
