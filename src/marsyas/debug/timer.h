/*
** Copyright (C) 1998-2013 George Tzanetakis <gtzan@cs.uvic.ca>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

// WARNING: Do not install this header, because it is platform-specific.

#ifndef MARSYAS_DEBUG_TIMER_INCLUDED
#define MARSYAS_DEBUG_TIMER_INCLUDED

#include <marsyas/common_source.h>
#include <marsyas/export.h>

#if defined(MARSYAS_LINUX)

#include <time.h>

namespace Marsyas { namespace Debug {

class Timer
{
  timespec m_start_cpu_time;
  timespec m_start_real_time;
  timespec m_cpu_time;
  timespec m_real_time;

public:
  Timer()
  {}

  void start()
  {
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &m_start_cpu_time);
    clock_gettime(CLOCK_REALTIME, &m_start_real_time);
  }

  void measure()
  {
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &m_cpu_time);
    clock_gettime(CLOCK_REALTIME, &m_real_time);
  }

  double cpuTime()
  {
    return difference(m_cpu_time, m_start_cpu_time);
  }

  double realTime()
  {
    return difference(m_real_time, m_start_real_time);
  }

private:
  double difference( const timespec &t, const timespec &t0 )
  {
    double seconds = t.tv_sec - t0.tv_sec;
    long nanoseconds = t.tv_nsec - t0.tv_nsec;
    seconds += (double) nanoseconds * 10.0e-10;
    return seconds;
  }
};

}} // namespace Marsyas::Debug

#else

#include <chrono>

namespace Marsyas { namespace Debug {

using namespace std::chrono;

class marsyas_EXPORT Timer
{
    high_resolution_clock::time_point m_real_time_start;
    high_resolution_clock::time_point m_real_time_measure;
public:
  void start()
  {
      m_real_time_start = high_resolution_clock::now();
  }
  void measure()
  {
      m_real_time_measure = high_resolution_clock::now();
  }
  double cpuTime()
  {
      return 0.0;
  }
  double realTime()
  {
      duration<double> m_real_time_dur =
              m_real_time_measure - m_real_time_start;
      return m_real_time_dur.count();
  }
};

}} // namespace Marsyas::Debug

#endif

#endif // MARSYAS_DEBUG_TIMER_INCLUDED
