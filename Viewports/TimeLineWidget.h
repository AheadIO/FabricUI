#ifndef __TIMELINEWIDGET_H__
#define __TIMELINEWIDGET_H__

#include <QtGui/QWidget>
#include <QtOpenGL/QGLWidget>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QMenu>
#include <QtGui/QHBoxLayout>
#include <QtCore/QTimer>
#include <QtCore/QTime>

#include <QtGui/QDoubleSpinBox>
#include <QtGui/QSlider>
#include <QtGui/QPushButton>
#include <QtGui/QComboBox>

#include <map>
#include <set>
#include <ctime>

namespace FabricUI
{
  namespace TimeLine
  {
    /// \brief an Basic Time slider Widget using QT
    class TimeLineWidget : public QWidget
    {
      Q_OBJECT

      public:

        TimeLineWidget();
        ~TimeLineWidget() {};

        // loop mode constants.
        #define LOOP_MODE_PLAY_ONCE   0
        #define LOOP_MODE_LOOP        1
        #define LOOP_MODE_OSCILLATE   2
        #define LOOP_MODE_DEFAULT     LOOP_MODE_LOOP

        /// update the internal time and also emit the signals
        void updateTime(int frame, bool onLoadingScene = false);

        /// return the current Time
        int getTime();

        /// return the current Time
        int getRangeStart();

        /// return the current Time
        int getRangeEnd();

        /// setup the time range
        void setTimeRange(int start , int end);
        
        /// setup the frame rate (will clamp to combo box options)
        void setFrameRate( float framesPerSecond );
        
        /// returns the loop mode
        int loopMode() const { return m_loopMode; }

        /// sets the loop mode
        void setLoopMode(int mode);

        /// returns the sim mode
        int simulationMode() const { return m_simMode; }

        /// sets the sim mode
        void setSimulationMode(int mode);

        /// pointer at QTimer
        QTimer *getTimer()  { return m_timer; }

        /// set the timer from a QTimer interval.
        void setTimerFromInterval(int interval);

        /// returns the frame rate (frame per seconds) currently set in m_frameRateComboBox.
        double getFrameRateFromComboBox();

        /// returns the framerate. 1000 = max.
        float framerate() const { return float(m_fps); }

        /// returns the framerate. 1000 = max.
        bool isPlaying() const { return m_timer->isActive(); }

      signals :
        /// this signal is emited when ever the time on the widget changed
        /// client would connect this slight to any slots that need to know about the time
        void frameChanged(int frame);

        /// this is emitted whenever playback changes
        void playbackChanged(bool isPlaying);
      
      public slots:

        /// slider value changed
        void sliderChanged(int frame);

        /// slider value changed by an increment
        void frameChangedBy(int frame);

        /// the current Frame value change on the DoubleSpinBox
        void currentFrameChanged();
        
        /// either the start or end time of the slider changed , this will adjust both regarderless which one changed
        void updateFrameRange();
        
        /// start or stop the playback ( act as a switch on the playing state )
        void play();
        
        /// stop the playback
        void pause();

        /// set the frame to the start of the current time slider range
        void goToStartFrame();
        
        /// goes one frame back based on the current frame 
        void goToPreviousFrame();
        
        /// goes one frame forward based on the current frame 
        void goToNextFrame();

        /// set the frame to the end of the current time slider range
        void goToEndFrame();

        /// called each time the timer is triggered ( basicly when playing )
        void timerUpdate();

      private slots:

        /// called when the frameRate is changed ( only few fixed option at moment )
        void frameRateChanged(int index);

        /// called when the looping scheme changed
        void loopModeChanged(int index);

        /// called when the simulation mode changed
        void simModeChanged(int index);
        
        /// internal time changing function 
        /// if you want to set the time from the outside use updateTime(int)
        void setTime(int frame);
        
      protected:
      
        /// the timer in charge of the playback . this is paused when not active
        QTimer * m_timer;
        QTime m_lastFrameTime;
        double m_fps;
        
        /// current frame
        int m_lastSteppedFrame;

        /// direction of play 
        int m_direction;
        
        /// are we looping
        int m_loopMode;

        /// are we stepping frames
        int m_simMode;

        /// blocking any additional settime calls
        bool m_settingTime;

        /// elements
        QDoubleSpinBox * m_startSpinBox;
        QSlider * m_frameSlider;
        QDoubleSpinBox * m_endSpinBox;
        QDoubleSpinBox * m_currentFrameSpinBox;
        QPushButton * m_goToStartFrameButton;
        QPushButton * m_goToPreviousFrameButton;
        QPushButton * m_playButton;
        QPushButton * m_goToNextFrameButton;
        QPushButton * m_goToEndFrameButton;
        QComboBox * m_frameRateComboBox;
        QComboBox * m_loopModeComBox;
        QComboBox * m_simModeComBox;

    };
  };
  
}

#endif
