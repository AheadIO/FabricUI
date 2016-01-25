#include "VEBaseSpinBox.h"
#include "VELineEdit.h"
#include "Util/QTSignalBlocker.h"

#include <QTGui/QLineEdit>
#include <QtGui/qevent.h>
#include <QtGui/QApplication>


//////////////////////////////////////////////////////////////////////////
template<typename QT_SPINBOX, typename value_type>
VEBaseSpinBox<QT_SPINBOX, value_type>::VEBaseSpinBox()
  : m_startValue(0)
  , m_dragging(false)
{
  VELineEdit* newLineEdit = new VELineEdit( this );
  newLineEdit->setAlignment( Qt::AlignRight | Qt::AlignCenter );
  setLineEdit( newLineEdit );

  // By default, the spinbox updates every time text changes.
  // Disconnect the signal setup to update on every text change, 
  // to only send an update on editing finished.
  disconnect( newLineEdit, SIGNAL( textChanged( QString ) ),
           this, SLOT( _q_editorTextChanged( QString ) ) );

  connect( newLineEdit, SIGNAL( textModified( QString ) ), 
           this, SLOT( _q_editorTextChanged( QString ) ) );
}

template<typename QT_SPINBOX, typename value_type>
VEBaseSpinBox<QT_SPINBOX, value_type>::~VEBaseSpinBox()
{

}

template<typename QT_SPINBOX, typename value_type>
void VEBaseSpinBox<QT_SPINBOX, value_type>::mousePressEvent( QMouseEvent * event )
{
  m_trackStartPos = event->pos();
  m_startValue = value();

  updateStep();

  emit interactionBegin();

  m_dragging = false;
}

template<typename QT_SPINBOX, typename value_type>
void VEBaseSpinBox<QT_SPINBOX, value_type>::mouseReleaseEvent( QMouseEvent * event )
{
  if (!m_dragging)
  {
    QT_SPINBOX::mousePressEvent( event );
    QT_SPINBOX::mouseReleaseEvent( event );
  }

  QApplication::restoreOverrideCursor();

  emit interactionEnd( true );
}

template<typename QT_SPINBOX, typename value_type>
void VEBaseSpinBox<QT_SPINBOX, value_type>::mouseMoveEvent( QMouseEvent *event )
{
  Qt::MouseButtons button = event->buttons();
  if (button != Qt::LeftButton) {
    return;
  }

  if (!m_dragging)
  {
    static const QCursor initialOverrideCursor( Qt::SizeVerCursor );
    QApplication::setOverrideCursor( initialOverrideCursor );
  }

  QPoint trackPos = event->pos();
  int nSteps = m_trackStartPos.y() - trackPos.y();
  // 
  float sensitivity = 10.0f / logicalDpiY();
  // Slow down movement if Ctrl is pressed
  if (QApplication::keyboardModifiers() & Qt::ControlModifier)
    sensitivity /= 10;
  // Sensitivity is modified by X-pos
  float xOffset = abs( event->x() - m_trackStartPos.x() );
  float xScale = exp( xOffset / logicalDpiX() );
  sensitivity *= xScale;
    

  nSteps *= sensitivity;

  // While dragging, we want to do an absolute value offset,
  // so reset to start value and then increment by abs step
  {
    FabricUI::Util::QTSignalBlocker block( this );
    setValue( m_startValue );
  }
  stepBy( nSteps );
  m_dragging = true;
  event->accept();
}