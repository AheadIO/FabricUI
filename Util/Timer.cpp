/*
 *  Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
 */

#include "Timer.h"

using namespace FabricUI::Util;

std::map<std::string, TimerPtr> Timer::s_timers;

TimerPtr Timer::getTimer(const char * title)
{
  std::map<std::string, TimerPtr>::iterator it = s_timers.find(std::string(title));
  if(it != s_timers.end())
    return it->second;
  TimerPtr timer = new Timer(title);
  s_timers.insert(std::pair<std::string, TimerPtr>(title, timer));
  return timer;
}

void Timer::resetAllTimers()
{
  std::map<std::string, TimerPtr>::iterator it;
  for(it = s_timers.begin(); it != s_timers.end(); it++)
    it->second->reset();
}

void Timer::removeTimer(const char * title)
{
  std::map<std::string, TimerPtr>::iterator it = s_timers.find(std::string(title));
  if(it == s_timers.end())
    return;
  s_timers.erase(it);
}

void Timer::removeAllTimers()
{
  s_timers.clear();
}

std::map<std::string, TimerPtr> & Timer::getAllTimers()
{
  return s_timers;
}
