//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include "BaseComplexViewItem.h"

#include <algorithm>
#include <FabricCore.h>
#include <math.h>
#include <QtGui/QColor>
#include <QtGui/QPushButton>

class ItemMetadata;
class ComboBox;

class AlphaWidget : public QWidget
{
  Q_OBJECT

public:

  AlphaWidget( QWidget *parent = NULL );

  void setColor( QColor color )
  {
    m_color = color;
    update();
  }

protected:

  virtual void paintEvent( QPaintEvent * event ) /*override*/;

private:

  QColor m_color;
};

class ColorViewItem : public BaseComplexViewItem
{
private:

  Q_OBJECT

  QWidget* m_widget;
  AlphaWidget *m_alphaWidget;

  FabricCore::RTVal m_colorRTVal;

  // We store the desired format of
  // the color (HSV or RGB)
  QColor::Spec m_spec;
  ComboBox* m_specCombo;
  
  // We need to keep an instance of the
  // metadata we pass to our children (as they
  // expect it to be valid for the extent of
  // their lives
  ViewItemMetadata m_childMetadata;

public:

  static BaseViewItem *CreateItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metaData
    );
  static const int Priority;

  ColorViewItem( const QVariant& value, const QString& name, ItemMetadata* metadata );
  ~ColorViewItem();

  virtual QWidget *getWidget() /*override*/;

  virtual void onModelValueChanged( QVariant const &value ) /*override*/;

  virtual void onChildViewValueChanged(
    int index,
    QVariant value
    ) /*override*/;

  virtual void doAppendChildViewItems( QList<BaseViewItem *>& items );

  void setButtonColor( const QColor & color );

  void deleteMe() { delete this; }

  virtual void metadataChanged( );

public slots:

  // Slot triggered when pushing background button
  void pickColor();
  void onColorChanged( QColor color );
  void onColorSelected( QColor color );

  void formatChanged( const QString& format );

protected:

  static QColor QColorFromComponents(
    float r,
    float g,
    float b,
    float a
    )
  {
    return QColor(
      int( roundf( std::max( 0.0f, std::min( 1.0f, r ) ) * 255.0f ) ),
      int( roundf( std::max( 0.0f, std::min( 1.0f, g ) ) * 255.0f ) ),
      int( roundf( std::max( 0.0f, std::min( 1.0f, b ) ) * 255.0f ) ),
      int( roundf( std::max( 0.0f, std::min( 1.0f, a ) ) * 255.0f ) )
      );
  }

  void toComponents(
    float &r,
    float &g,
    float &b,
    float &a
    ) const;

  QColor toQColor() const;

  void fromQColor( QColor color );

  void sync();
};
