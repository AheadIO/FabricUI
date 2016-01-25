#pragma once

#include <QtGui/QSpinBox>

template<typename QT_SPINBOX, typename value_type>
class VEBaseSpinBox : public QT_SPINBOX
{
public:
  VEBaseSpinBox();
  ~VEBaseSpinBox();

  virtual void mousePressEvent( QMouseEvent * ) /*override*/;

  virtual void mouseReleaseEvent( QMouseEvent * ) /*override*/;

  virtual void mouseMoveEvent( QMouseEvent * ) /*override*/;

  virtual void interactionBegin()=0;
  virtual void interactionEnd( bool )=0;

  virtual void updateStep()=0;

protected:
  QPoint m_trackStartPos;
  value_type m_startValue;
  bool m_dragging;

};

