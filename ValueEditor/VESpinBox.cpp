//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "VESpinBox.h"
#include "VELineEdit.h"

#include <algorithm>
#include <FabricUI/Util/LoadPixmap.h>
#include <math.h>
#include <QtGui/QApplication>
#include <QtGui/QHBoxLayout>
#include <QtGui/QMouseEvent>
#include <QtGui/QPaintDevice>
#include <QtGui/QPainter>
#include <QtGui/QPushButton>
#include <QtGui/QStyle>
#include <QtGui/QStyleOption>

VESpinBox_Adjuster::VESpinBox_Adjuster( QWidget *parent )
  : QWidget( parent )
  , m_adjusting( false )
{
  adjustDisplay();
}

void VESpinBox_Adjuster::adjustDisplay()
{
  update();
}

void VESpinBox_Adjuster::resizeEvent( QResizeEvent *event )
{
  QWidget::resizeEvent( event );
}

void VESpinBox_Adjuster::paintEvent( QPaintEvent *event )
{
  QPainter painter( this );

  int w = width();
  int h = height();

  static const QBrush regularBrush( QColor( 104, 104, 104 ) );
  static const QBrush adjustingBrush( QColor( 184, 184, 184 ) );

  QPainterPath path;
  path.moveTo( 1, (h*7+8)/16 );
  path.lineTo( (w+1)/2, 1 );
  path.lineTo( w - 1, (h*7+8)/16 );
  path.moveTo( 1, (h*9+8)/16 );
  path.lineTo( (w+1)/2, h - 1 );
  path.lineTo( w - 1, (h*9+8)/16 );
  painter.fillPath( path, m_adjusting? adjustingBrush: regularBrush );

  event->accept();
}

void VESpinBox_Adjuster::mousePressEvent( QMouseEvent *event )
{
  if ( !m_adjusting )
  {
    if ( QApplication::keyboardModifiers() & Qt::MetaModifier )
      return QWidget::mousePressEvent( event );

    m_adjusting = true;
    adjustDisplay();

    emit pressed();

    event->accept();
  }
}

void VESpinBox_Adjuster::mouseReleaseEvent( QMouseEvent *event )
{
  if ( m_adjusting )
  {
    m_adjusting = false;
    adjustDisplay();

    emit released();

    event->accept();
  }
}

VESpinBox::VESpinBox(
  double initialValue,
  QWidget *parent
  )
  : QWidget( parent )
  , m_value( initialValue )
  , m_trackCount( 0 )
  , m_wheelActive( false )
{
  m_lineEdit = new VELineEdit( this );
  m_lineEdit->setText( QString::number( m_value, 'f', 2 ) ); 
  connect(
    m_lineEdit, SIGNAL(textModified(QString)),
    this, SLOT(onLineEditTextModified(QString))
    );

  m_button = new VESpinBox_Adjuster( this );
  connect(
    m_button, SIGNAL(pressed()),
    this, SLOT(onButtonPressed())
    );
  connect(
    m_button, SIGNAL(released()),
    this, SLOT(onButtonReleased())
    );

  QHBoxLayout *layout = new QHBoxLayout( this );
  layout->setContentsMargins( 0, 0, 0, 0 );
  layout->setSpacing( 2 );
  layout->addWidget( m_lineEdit );
  layout->addWidget( m_button );
}

void VESpinBox::setValue( double value )
{
  if ( m_value != value )
  {
    m_value = value;

    m_lineEdit->setText( QString::number( m_value, 'f', 2 ) ); 

    emit valueChanged( m_value );
  }
}

void VESpinBox::onLineEditTextModified( QString text )
{
  emit valueChanged( text.toDouble() );
}

void VESpinBox::onButtonPressed()
{
  if ( m_trackCount++ == 0 )
  {
    setMouseTracking( true );

    m_startValue = m_value;
    m_adjustAmount = 0;

    m_trackStartPos = QCursor::pos();

    static const QCursor initialOverrideCursor( Qt::SizeVerCursor );
    QApplication::setOverrideCursor( initialOverrideCursor );

    emit interactionBegin();
  }
}

void VESpinBox::resizeEvent( QResizeEvent *event )
{
  QWidget::resizeEvent( event );

  int w = event->size().width();
  int h = event->size().height();

  m_lineEdit->setGeometry( 0, 0, w - h/2 - 2, h );
  m_button->setGeometry( w - h/2, 1, h/2, h - 2 );
}

void VESpinBox::mouseMoveEvent( QMouseEvent *event )
{
  if ( m_trackCount > 0 )
  {
    QPoint trackPos = QCursor::pos();

    m_adjustAmount =
      -( trackPos.y() - m_trackStartPos.y() ) * 600.0 / logicalDpiY();
    adjust();
  }
}

void VESpinBox::onButtonReleased()
{
  if ( --m_trackCount == 0 )
  {
    emit interactionEnd( true );

    QApplication::restoreOverrideCursor();
    
    setMouseTracking( false );
  }    
}

void VESpinBox::keyPressEvent( QKeyEvent * event )
{
  if ( m_trackCount > 0 || m_wheelActive )
    adjust();

  QWidget::keyPressEvent( event );
}

void VESpinBox::keyReleaseEvent( QKeyEvent * event )
{
  if ( m_trackCount > 0 || m_wheelActive )
    adjust();

  QWidget::keyReleaseEvent( event );
}

void VESpinBox::wheelEvent( QWheelEvent *event )
{
  if ( !m_wheelActive )
  {
    m_startValue = m_value;
    m_adjustAmount = 0;

    m_wheelActive = true;

    static const QCursor initialOverrideCursor( Qt::SizeVerCursor );
    QApplication::setOverrideCursor( initialOverrideCursor );

    emit interactionBegin();
  }

  m_adjustAmount -= event->delta();
  adjust();

  event->accept();
}

void VESpinBox::adjust()
{
  static const double minChangePerStep = 0.01;
  static const double maxChangePerStep = 1000.0;
  double changePerStep =
    std::min(
      maxChangePerStep,
      std::max(
        minChangePerStep,
        fabs( m_startValue * 0.1 )
        )
      );

  double velocity;
  if ( QApplication::keyboardModifiers() & Qt::ControlModifier )
  {
    static const QCursor cursor( Qt::CrossCursor );
    QApplication::changeOverrideCursor( cursor );
    velocity = 0.1;
  }
  else
  {
    static const QCursor cursor( Qt::SizeVerCursor );
    QApplication::changeOverrideCursor( cursor );
    velocity = 1.0;
  }

  if ( m_trackCount > 0 )
  {
    velocity *=
      exp( ( QCursor::pos().x() - m_trackStartPos.x() ) / logicalDpiX() );
  }

  double value =
    m_startValue + changePerStep * m_adjustAmount * velocity / 120.0;
  setValue( QString::number( value, 'f', 2 ).toDouble() );
}

void VESpinBox::leaveEvent( QEvent *event )
{
  if ( m_wheelActive )
  {
    m_wheelActive = false;

    QApplication::restoreOverrideCursor();

    emit interactionEnd( true );
  }
}
