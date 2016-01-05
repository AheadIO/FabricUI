#pragma once

class BaseViewItem;
class BaseComplexViewItem;

class VALUEEDIT_API ViewItemChildRouter : public QObject
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
