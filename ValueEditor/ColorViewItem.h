#pragma once
#include "BaseComplexViewItem.h"

class QPushButton;

class ColorViewItem : public BaseComplexViewItem
{
private:

  Q_OBJECT

  QPushButton* m_button;
  
  QColor m_color;

public:

  ColorViewItem( const QVariant& value, const QString& name );
  ~ColorViewItem();

  virtual QWidget *getWidget() /*override*/;

  virtual void onModelValueChanged( QVariant const &value ) /*override*/;

  virtual void onChildViewValueChanged(
    int index,
    QVariant const &value,
    bool commit
    ) /*override*/;

  virtual void doAppendChildViewItems( QList<BaseViewItem *>& items );

  void setButtonColor( const QColor & color );

  void deleteMe() { delete this; }

public slots:

  // Slot triggered when pushing background button
  void pickColor();
  void onColorChanged( QColor color );
  void onColorSelected( QColor color );
};
