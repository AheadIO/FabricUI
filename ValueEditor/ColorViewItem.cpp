//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "ColorViewItem.h"
#include "QVariantRTVal.h"
#include "ViewItemFactory.h"
#include "ItemMetadata.h"
#include "BaseModelItem.h"
#include "ComboBox.h"

#include <assert.h>
#include <FTL/JSONValue.h>
#include <QtCore/QVariant>
#include <QtGui/QColorDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPainter>

#define IDX_RGB 0
#define IDX_HSV 1
#define META_FORMAT  "displayFormat"

AlphaWidget::AlphaWidget( QWidget *parent )
  : QWidget( parent )
{
  setAutoFillBackground( false );
}

void AlphaWidget::paintEvent( QPaintEvent *event )
{
  QPainter painter( this );

  QRect r = rect();

  QPainterPath clipPath;
  clipPath.addRoundedRect( r, 4, 4 );
  painter.setClipPath( clipPath );

  painter.setPen( Qt::NoPen );

  for ( int row = 0; ; ++row )
  {
    int top = r.top() + 4 * row;
    int bottom = std::min( top + 4, r.bottom() );
    if ( bottom <= top )
      break;

    for ( int col = 0; ; ++col )
    {
      int left = r.left() + 4 * col;
      int right = std::min( left + 4, r.right() );
      if ( right <= left )
        break;
      
      painter.setBrush( ((row + col) % 2 == 0)? Qt::white: Qt::black );
      painter.drawRect( QRect( left, top, right, bottom ) );
    }
  }

  if ( m_color.isValid() )
  {
    painter.setBrush( m_color );
    painter.drawRoundedRect( r, 4, 4 );
  }
}

ColorViewItem::ColorViewItem(
  const QVariant& value,
  const QString& name,
  ItemMetadata* metadata
  )
  : BaseComplexViewItem( name, metadata )
  , m_widget( NULL )
  , m_spec( QColor::Rgb )
  , m_specCombo( NULL )
  , m_childMetadata (metadata)
{

  m_widget = new QWidget;
  m_widget->setObjectName( "ColorItem" );

  m_alphaWidget = new AlphaWidget;
  m_alphaWidget->setSizePolicy(
    QSizePolicy::Expanding,
    QSizePolicy::Expanding
    );

  QHBoxLayout *buttonLayout = new QHBoxLayout;
  buttonLayout->setContentsMargins( 0, 0, 0, 0 );
  buttonLayout->addWidget( m_alphaWidget );

  QPushButton *button = new QPushButton;
  button->setLayout( buttonLayout );
  connect(
    button, SIGNAL( clicked() ),
    this, SLOT( pickColor() )
    );

  QHBoxLayout *layout = new QHBoxLayout( m_widget );
  layout->addWidget( button );

  m_specCombo = new ComboBox;
  m_specCombo->addItem( tr( "RGB" ) );
  m_specCombo->addItem( tr( "HSV" ) );

  connect( m_specCombo, SIGNAL( currentIndexChanged( const QString& ) ),
           this, SLOT( formatChanged( const QString& ) ) );

  layout->addWidget( m_specCombo );

  m_childMetadata.setString( "uiRange", "(0.0, 1.0)" );

  metadataChanged();

  onModelValueChanged( value );
}

ColorViewItem::~ColorViewItem()
{
}

QWidget *ColorViewItem::getWidget()
{
  return m_widget;
}

void ColorViewItem::toComponents(
  float &r,
  float &g,
  float &b,
  float &a
  ) const
{
  if ( m_colorRTVal.hasType( "RGB" ) )
  {
    r = m_colorRTVal.getMemberRef( 0 ).getUInt8() / 255.0f;
    g = m_colorRTVal.getMemberRef( 1 ).getUInt8() / 255.0f;
    b = m_colorRTVal.getMemberRef( 2 ).getUInt8() / 255.0f;
    a = 1.0f;
  }
  else if ( m_colorRTVal.hasType( "RGBA" ) )
  {
    r = m_colorRTVal.getMemberRef( 0 ).getUInt8() / 255.0f;
    g = m_colorRTVal.getMemberRef( 1 ).getUInt8() / 255.0f;
    b = m_colorRTVal.getMemberRef( 2 ).getUInt8() / 255.0f;
    a = m_colorRTVal.getMemberRef( 3 ).getUInt8() / 255.0f;
  }
  else if ( m_colorRTVal.hasType( "Color" ) )
  {
    r = m_colorRTVal.getMemberRef( 0 ).getFloat32();
    g = m_colorRTVal.getMemberRef( 1 ).getFloat32();
    b = m_colorRTVal.getMemberRef( 2 ).getFloat32();
    a = m_colorRTVal.getMemberRef( 3 ).getFloat32();
  }
  else
  {
    r = g = b = 0.0f;
    a = 1.0f;
  }
}

QColor ColorViewItem::toQColor() const
{
  float r, g, b, a;
  toComponents( r, g, b, a );
  return QColorFromComponents( r, g, b, a );
}
 
void ColorViewItem::onModelValueChanged( QVariant const &value )
{
  RTVariant::toRTVal( value, m_colorRTVal );

  sync();
}

void ColorViewItem::sync()
{
  float r, g, b, a;
  toComponents( r, g, b, a );
  QColor qColor = QColorFromComponents( r, g, b, a );

  switch ( m_spec )
  {
    case QColor::Rgb:
    {
      routeModelValueChanged( 0, QVariant( r ) );
      routeModelValueChanged( 1, QVariant( g ) );
      routeModelValueChanged( 2, QVariant( b ) );
      if ( !m_colorRTVal.hasType( "RGB" ) )
        routeModelValueChanged( 3, QVariant( a ) );
    }
    break;

    case QColor::Hsv:
    {
      routeModelValueChanged( 0, QVariant( std::max( 0.0, qColor.hueF() ) ) );
      routeModelValueChanged( 1, QVariant( qColor.saturationF() ) );
      routeModelValueChanged( 2, QVariant( qColor.valueF() ) );
      if ( !m_colorRTVal.hasType( "RGB" ) )
        routeModelValueChanged( 3, QVariant( qColor.alphaF() ) );
    }
    break;

    default:
      assert( !"Error: Bad Color Spec in ColorViewItem" );
      break;
  }

  setButtonColor( qColor );
}

void ColorViewItem::onChildViewValueChanged( int index, QVariant value )
{
  switch ( m_spec )
  {
    case QColor::Hsv:
    {
      qreal v[4];
      toQColor().getHsvF( &v[0], &v[1], &v[2], &v[3] );
      v[0] = std::max( 0.0, v[0] );
      v[index] = value.toDouble();
      fromQColor( QColor::fromHsvF( v[0], v[1], v[2], v[3] ) );
    }
    break;

    case QColor::Rgb:
    {
      if ( m_colorRTVal.hasType( "RGB" ) )
      {
        assert( index >= 0 && index < 3 );
        if ( index >= 0 && index < 3 )
          m_colorRTVal.getMemberRef( index ).setUInt8(
            uint8_t(
              round(
                255.0f * std::max( 0.0f, std::min( 1.0f, float( value.toDouble() ) ) )
                )
              )
            );
      }
      else if ( m_colorRTVal.hasType( "RGBA" ) )
      {
        assert( index >= 0 && index < 4 );
        if ( index >= 0 && index < 4 )
          m_colorRTVal.getMemberRef( index ).setUInt8(
            uint8_t(
              round(
                255.0f * std::max( 0.0f, std::min( 1.0f, float( value.toDouble() ) ) )
                )
              )
            );
      }
      else if ( m_colorRTVal.hasType( "Color" ) )
      {
        assert( index >= 0 && index < 4 );
        if ( index >= 0 && index < 4 )
          m_colorRTVal.getMemberRef( index ).setFloat32( float( value.toDouble() ) );
      }
    }
    break;

    default:
      assert( false );
      break;
  }

  emit viewValueChanged( toVariant( m_colorRTVal ) );
}

void ColorViewItem::doAppendChildViewItems( QList<BaseViewItem*>& items )
{
  ViewItemFactory* factory = ViewItemFactory::GetInstance();
  BaseViewItem *children[4];
  int childCount = 0;
  switch ( m_spec )
  {
    case QColor::Rgb:
    {
      float r, g, b, a;
      toComponents( r, g, b, a );

      children[childCount++] = factory->createViewItem( "R", QVariant( r ), &m_childMetadata );
      children[childCount++] = factory->createViewItem( "G", QVariant( g ), &m_childMetadata );
      children[childCount++] = factory->createViewItem( "B", QVariant( b ), &m_childMetadata );
      if ( !m_colorRTVal.hasType( "RGB" ) )
        children[childCount++] = factory->createViewItem( "A", QVariant( a ), &m_childMetadata );
    }
    break;

    case QColor::Hsv:
    {
      QColor color = toQColor();

      children[childCount++] = factory->createViewItem( "H", QVariant( std::max( 0.0, color.hueF() ) ), &m_childMetadata );
      children[childCount++] = factory->createViewItem( "S", QVariant( color.saturationF() ), &m_childMetadata );
      children[childCount++] = factory->createViewItem( "V", QVariant( color.valueF() ), &m_childMetadata );
      if ( !m_colorRTVal.hasType( "RGB" ) )
        children[childCount++] = factory->createViewItem( "A", QVariant( color.alphaF() ), &m_childMetadata );
    }
    break;

    default:
      assert( !"Invalid Color" );
      return;
  }

  for (int i = 0; i < childCount; ++i)
  {
    connectChild( i, children[i] );
    items.append( children[i] );
  }
}

void ColorViewItem::setButtonColor( const QColor& color ) 
{
  m_alphaWidget->setColor( color );
}

void ColorViewItem::metadataChanged()
{
  FTL::StrRef dispType = m_metadata.getString( META_FORMAT );

  if ( dispType == FTL_STR("HSV") )
  {
    if ( m_spec != QColor::Hsv )
    {
      m_spec = QColor::Hsv;
      m_specCombo->setCurrentIndex( IDX_HSV );
      sync();
    }
  }
  else
  {
    if ( m_spec != QColor::Rgb )
    {
      m_spec = QColor::Rgb;
      m_specCombo->setCurrentIndex( IDX_RGB );
      sync();
    }
  }
}

void ColorViewItem::pickColor()
{
  QColor color = toQColor();
  QColorDialog qcd( color, NULL );
  qcd.setOption(
    QColorDialog::ShowAlphaChannel,
    !m_colorRTVal.hasType( "RGB" )
    );
  
  connect( &qcd, SIGNAL( colorSelected( QColor ) ), 
           this, SLOT( onColorSelected( QColor ) ) );
  connect( &qcd, SIGNAL( currentColorChanged( QColor ) ), 
           this, SLOT( onColorChanged( QColor ) ) );

  emit interactionBegin();
  
  int execResult = qcd.exec();

  // If the user hits cancel, we wish to restore
  // the current state to the previous value
  emit interactionEnd( execResult == QDialog::Accepted );
}

void ColorViewItem::fromQColor( QColor color )
{
  if ( color.isValid() )
  {
    if ( m_colorRTVal.hasType( "RGB" ) )
    {
      m_colorRTVal.getMemberRef( 0 ).setUInt8( uint8_t( color.red() ) );
      m_colorRTVal.getMemberRef( 1 ).setUInt8( uint8_t( color.green() ) );
      m_colorRTVal.getMemberRef( 2 ).setUInt8( uint8_t( color.blue() ) );
    }
    else if ( m_colorRTVal.hasType( "RGBA" ) )
    {
      m_colorRTVal.getMemberRef( 0 ).setUInt8( uint8_t( color.red() ) );
      m_colorRTVal.getMemberRef( 1 ).setUInt8( uint8_t( color.green() ) );
      m_colorRTVal.getMemberRef( 2 ).setUInt8( uint8_t( color.blue() ) );
      m_colorRTVal.getMemberRef( 3 ).setUInt8( uint8_t( color.alpha() ) );
    }
    else if ( m_colorRTVal.hasType( "Color" ) )
    {
      m_colorRTVal.getMemberRef( 0 ).setFloat32( float( color.redF() ) );
      m_colorRTVal.getMemberRef( 1 ).setFloat32( float( color.greenF() ) );
      m_colorRTVal.getMemberRef( 2 ).setFloat32( float( color.blueF() ) );
      m_colorRTVal.getMemberRef( 3 ).setFloat32( float( color.alphaF() ) );
    }
  }
}

void ColorViewItem::onColorChanged( QColor color )
{
  fromQColor( color );
  emit viewValueChanged( toVariant( m_colorRTVal ) );
}

void ColorViewItem::onColorSelected( QColor color )
{
  fromQColor( color );
  emit viewValueChanged( toVariant( m_colorRTVal ) );
}

void ColorViewItem::formatChanged( const QString& format )
{
  BaseModelItem* modelItem = getModelItem();

  // Note: setting metadata will delete this
  // class
  if ( format == tr("HSV") )
  {
    m_spec = QColor::Hsv;
    if ( modelItem != NULL )
      modelItem->setMetadata( META_FORMAT, "HSV", 0 );
  }
  else
  {
    m_spec = QColor::Rgb;
    if ( modelItem != NULL )
      modelItem->setMetadata( META_FORMAT, "RGB", 0 );
  }

  emit rebuildChildren( this );
}



//////////////////////////////////////////////////////////////////////////
//

BaseViewItem *ColorViewItem::CreateItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metaData
  )
{
  if ( value.type() != QVariant::UserType )
    return NULL;
  if ( value.userType() != qMetaTypeId<FabricCore::RTVal>() )
    return NULL;

  FabricCore::RTVal rtVal = value.value<FabricCore::RTVal>();
  if ( !rtVal.isValid() )
    return NULL;
  if ( !rtVal.hasType( "RGB" )
    && !rtVal.hasType( "RGBA" )
    && !rtVal.hasType( "Color" ) )
    return NULL;

  return new ColorViewItem( value, name, metaData );
}

const int ColorViewItem::Priority = 3;
