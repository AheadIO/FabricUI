//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include <QtCore/QObject.h>

class BaseViewItem;
class BaseComplexViewItem;

class ViewItemChildRouter : public QObject
{
  Q_OBJECT

  BaseComplexViewItem *m_viewItem;
  int m_index;

public:

  ViewItemChildRouter(
    BaseComplexViewItem *viewItem,
    int index
    );

  void connectToChild( BaseViewItem *childViewItem );

  void emitModelValueChanged( QVariant const &value );

signals:

  void modelValueChanged( QVariant const &value );

  public slots:

  void onViewValueChanged( QVariant const &value, bool commit );
};
