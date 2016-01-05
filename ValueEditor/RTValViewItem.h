#pragma once

#include "BaseComplexViewItem.h"

typedef std::vector<std::string> NameList;

// A default RTVal implementation that can display any RTVal class
class RTValViewItem : public BaseComplexViewItem
{
	// Cache the val for easier reconstruction.  Why?  cause why not?
	FabricCore::RTVal m_val; 

  // A composite widget to represent any/all values in UI
	QLabel *m_widget;

  // A list of 
  NameList m_childNames;

public:

	RTValViewItem(QString name, const FabricCore::RTVal& value );
	~RTValViewItem();

  virtual QWidget *getWidget() /*override*/;

  virtual void onModelValueChanged( QVariant const &value ) /*override*/;

  virtual void doAppendChildViewItems( QList<BaseViewItem *>& items );

  virtual void onChildViewValueChanged(
    int index,
    QVariant const &value,
    bool commit
    ) /*override*/;

  void deleteMe() { delete this; }

private:

  void UpdateWidget();
};
