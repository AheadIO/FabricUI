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
    Qt::KeyboardModifiers keyboardModifiers =
      QApplication::keyboardModifiers();

    if ( keyboardModifiers & Qt::MetaModifier )
      return QLabel::mousePressEvent( event );

    double adjuster;
    if ( QApplication::keyboardModifiers() & Qt::ControlModifier )
      adjuster = 0.1;
    else
      adjuster = 1.0;

    m_adjusting = true;
    adjustDisplay();

    emit pressed( adjuster );

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
{
  m_lineEdit = new VELineEdit( this );
  m_lineEdit->setText( QString::number( m_value, 'f', 2 ) ); 
  connect(
    m_lineEdit, SIGNAL(textModified(QString)),
    this, SLOT(onLineEditTextModified(QString))
    );

  m_button = new VESpinBox_Adjuster( this );
  connect(
    m_button, SIGNAL(pressed(double)),
    this, SLOT(onButtonPressed(double))
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

void VESpinBox::onButtonPressed( double adjuster )
{
  if ( m_trackCount++ == 0 )
  {
    setMouseTracking( true );

    m_trackAdjuster = adjuster;
    m_trackStartPos = QCursor::pos();
    m_trackStartValue = m_value;

    QApplication::setOverrideCursor( QCursor( Qt::SizeVerCursor ) );

    emit interactionBegin();
  }
}

void VESpinBox::mouseMoveEvent( QMouseEvent *event )
{
  if ( m_trackCount > 0 )
  {
    QPoint trackPos = QCursor::pos();
    int diff = -( trackPos.y() - m_trackStartPos.y() );
    int dpiY = logicalDpiY();
    static const double minChangePerInch = 0.1;
    double changePerInch = std::max( fabs( m_trackStartValue ), minChangePerInch );
    double value = m_trackStartValue + changePerInch * m_trackAdjuster * double(diff) / double(dpiY);
    setValue( QString::number( value, 'f', 2 ).toDouble() );
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
