//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "VESpinBox.h"
#include "VELineEdit.h"

#include <algorithm>
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
  : QLabel( parent )
  , m_adjusting( false )
{
  adjustDisplay();
}

void VESpinBox_Adjuster::adjustDisplay()
{
  if ( m_adjusting )
    setText("---");
  else
    setText("adj");
}

void VESpinBox_Adjuster::mousePressEvent( QMouseEvent *event )
{
  if ( !m_adjusting )
  {
    if ( QApplication::keyboardModifiers() & Qt::MetaModifier )
      return QLabel::mousePressEvent( event );

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
  , m_interactionEndOnLeave( false )
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

  QHBoxLayout *layout = new QHBoxLayout;
  layout->setContentsMargins( 0, 0, 0, 0 );
  layout->addWidget( m_lineEdit );
  layout->addWidget( m_button );
  setLayout( layout );
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

    m_trackLastPos = QCursor::pos();

    QApplication::setOverrideCursor( QCursor( Qt::SizeVerCursor ) );

    emit interactionBegin();
  }
}

void VESpinBox::mouseMoveEvent( QMouseEvent *event )
{
  if ( m_trackCount > 0 )
  {
    QPoint trackPos = QCursor::pos();
    int dpiY = logicalDpiY();
    double diff = -( trackPos.y() - m_trackLastPos.y() ) * 600.0 / dpiY;
    onDelta( diff );
    m_trackLastPos = trackPos;
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

void VESpinBox::wheelEvent( QWheelEvent *event )
{
  if ( !m_interactionEndOnLeave )
  {
    m_interactionEndOnLeave = true;

    QApplication::setOverrideCursor( QCursor( Qt::SizeVerCursor ) );

    emit interactionBegin();
  }

  onDelta( -event->delta() );

  event->accept();
}

void VESpinBox::onDelta( double delta )
{
  static const double minChangePerStep = 0.01;
  double changePerStep = std::max( fabs( m_value * 0.1 ), minChangePerStep );
  double adjuster;
  if ( QApplication::keyboardModifiers() & Qt::ControlModifier )
    adjuster = 0.1;
  else
    adjuster = 1.0;
  double value = m_value + changePerStep * delta * adjuster / 120.0;
  setValue( QString::number( value, 'f', 2 ).toDouble() );
}

void VESpinBox::leaveEvent( QEvent *event )
{
  if ( m_interactionEndOnLeave )
  {
    m_interactionEndOnLeave = false;

    QApplication::restoreOverrideCursor();

    emit interactionEnd( true );
  }
}
