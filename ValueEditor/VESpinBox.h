//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include <QtGui/QLabel>
#include <QtGui/QWidget>

class VELineEdit;

class VESpinBox_Adjuster : public QLabel
{
  Q_OBJECT

public:

  VESpinBox_Adjuster( QWidget *parent = 0 );

signals:

  void pressed();
  void released();

protected:

  void adjustDisplay();

  virtual void mousePressEvent( QMouseEvent *event ) /*override*/;
  virtual void mouseReleaseEvent( QMouseEvent *event ) /*override*/;

private:

  bool m_adjusting;
};

class VESpinBox : public QWidget
{
  Q_OBJECT

public:

  VESpinBox( double initialValue, QWidget *parent = 0 );

  void setValue( double value );

signals:

  void interactionBegin();
  void valueChanged( double value );
  void interactionEnd( bool );

protected:

  virtual void mouseMoveEvent( QMouseEvent *event ) /*override*/;
  virtual void wheelEvent( QWheelEvent *event ) /*override*/;
  virtual void leaveEvent( QEvent *event ) /*override*/;

protected slots:
  
  void onLineEditTextModified( QString text );

  void onButtonPressed();
  void onButtonReleased();

  void onDelta( double delta );

private:

  double m_value;
  VELineEdit *m_lineEdit;
  VESpinBox_Adjuster *m_button;
  int m_trackCount;
  QPoint m_trackLastPos;
  bool m_interactionEndOnLeave;
};
