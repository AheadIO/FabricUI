/*
 *  Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
 */

#ifndef _FABRICUI_UTIL_TIMER_H
#define _FABRICUI_UTIL_TIMER_H

#include "Ticks.h"
#include <stdio.h>
#include <map>
#include <string>
#include <FTL/SharedPtr.h>

namespace FabricUI
{
  namespace Util
  {
    class Timer;
    typedef FTL::SharedPtr<Timer> TimerPtr;

    class Timer : public FTL::Shareable
    {
    public:

      Timer(const char * title = 0)
      {
        m_title = title;
        reset();
      }

      const char * title() const{
        return m_title;
      }

      void reset(bool running = false)
      {
        m_running = running;
        m_offset = 0.0;
        m_begin = GetCurrentTicks();
      }

      double getElapsedMS( bool reset = false )
      {
        if(!m_running)
          return m_offset;
        uint64_t currentTicks = GetCurrentTicks();
        double elapsed =
          m_offset + GetSecondsBetweenTicks( m_begin, currentTicks ) * 1e3;
        if ( reset )
          m_begin = currentTicks;
        return elapsed;
      }

      void pause()
      {
        if(!m_running)
          return;
        m_offset +=
          GetSecondsBetweenTicks( m_begin, GetCurrentTicks() ) * 1e3;
        m_running = false;
      }

      void resume()
      {
        if(m_running)
          return;
        m_begin = GetCurrentTicks();
        m_running = true;
      }

      static TimerPtr getTimer(const char * title);
      static void resetAllTimers();
      static void removeTimer(const char * title);
      static void removeAllTimers();
      static std::map<std::string, TimerPtr> & getAllTimers();

    private:

      static std::map<std::string, TimerPtr> s_timers;
      const char * m_title;
      bool m_running;
      double m_offset;
      uint64_t m_begin;
    };
  }
}

#endif //_FABRICUI_UTIL_TIMER_H
