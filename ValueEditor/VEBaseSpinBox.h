//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include "VELineEdit.h"

#include <FabricUI/Util/QTSignalBlocker.h>
#include <math.h>
#include <QtGui/QApplication>
#include <QtGui/qevent.h>
#include <QTGui/QLineEdit>
#include <QtGui/QSpinBox>

template<typename QT_SPINBOX, typename value_type>
class VEBaseSpinBox : public QT_SPINBOX
{
public:

  VEBaseSpinBox()
    : m_startValue( 0 )
    , m_dragging( false )
    , m_wheelActive( false )
  {
    QT_SPINBOX::lineEdit()->setAlignment( Qt::AlignRight | Qt::AlignCenter );
    QT_SPINBOX::setKeyboardTracking( false );
    QT_SPINBOX::setSingleStep( 0 );
  }

  ~VEBaseSpinBox() {}

  virtual void mousePressEvent( QMouseEvent * event ) /*override*/
  {
    m_trackStartPos = event->pos();
    m_startValue = QT_SPINBOX::value();

    static const QCursor initialOverrideCursor( Qt::SizeVerCursor );
    QApplication::setOverrideCursor( initialOverrideCursor );

    emit interactionBegin();

    m_dragging = false;
  }

  virtual void mouseReleaseEvent( QMouseEvent * event ) /*override*/
  {
    if ( !m_dragging )
    {
      // [pzion 20160125] Steps are bigger than dragging
      updateStep( 0.0, 1.0 );

      QT_SPINBOX::mousePressEvent( event );
      QT_SPINBOX::mouseReleaseEvent( event );
    }

    QApplication::restoreOverrideCursor();

    QT_SPINBOX::setSingleStep( 0 );

    emit interactionEnd( true );
  }

  virtual void mouseMoveEvent( QMouseEvent *event ) /*override*/
  {
    Qt::MouseButtons button = event->buttons();
    if ( button != Qt::LeftButton )
      return;

    QPoint trackPos = event->pos();

    int deltaX = trackPos.x() - m_trackStartPos.x();
    double deltaXInInches =
      double( deltaX ) / double( QT_SPINBOX::logicalDpiX() );

    double sensitivity;
    // Slow down movement if Ctrl is pressed
    if ( QApplication::keyboardModifiers() & Qt::ControlModifier )
      sensitivity = 0.01;
    else
      sensitivity = 0.1;

    updateStep( deltaXInInches, sensitivity );

    int nSteps = m_trackStartPos.y() - trackPos.y();

    // While dragging, we want to do an absolute value offset,
    // so reset to start value and then increment by abs step
    {
      FabricUI::Util::QTSignalBlocker block( this );
      QT_SPINBOX::setValue( m_startValue );
    }
    QT_SPINBOX::stepBy( nSteps );
    m_dragging = true;
    event->accept();
  }

  virtual void wheelEvent( QWheelEvent *event ) /*override*/
  {
    if ( !m_wheelActive )
    {
      m_wheelActive = true;
      // [pzion 20160125] Steps are bigger than dragging
      updateStep( 0.0, 1.0 );
      emit interactionBegin();
    }
    QT_SPINBOX::wheelEvent( event );
  }

  virtual void leaveEvent( QEvent *event )
  {
    if ( m_wheelActive )
    {
      m_wheelActive = false;
      QT_SPINBOX::setSingleStep( 0 );
      emit interactionEnd( true );
    }
    QT_SPINBOX::leaveEvent( event );
  }

  virtual void interactionBegin() = 0;
  virtual void interactionEnd( bool ) = 0;

  virtual void updateStep( double deltaXInInches, double sensitivity ) = 0;

protected:

  QPoint m_trackStartPos;
  value_type m_startValue;
  bool m_dragging;
  bool m_wheelActive;
};
