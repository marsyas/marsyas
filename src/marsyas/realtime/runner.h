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

#ifndef MARSYAS_THREAD_SYSTEM_INCLUDED
#define MARSYAS_THREAD_SYSTEM_INCLUDED

#include "event_queue.h"
#include "atomic_control.h"
#include "any.h"
#include "MarSystem.h"

#include <thread>
#include <atomic>
#include <mutex>
#include <list>
#include <map>

namespace Marsyas {
namespace RealTime {

class Control;
class RunnerThread;

/**
 * @brief Interaction with MarSystem running in real time.
 * @author Jakob Leben (jakob.leben@gmail.com)
 */

class marsyas_EXPORT Runner
{
public:

  Runner( MarSystem * system );

  MarSystem * system() { return m_system; }

  void start();
  void stop();
  void wait();
  bool isRunning() const
  {
    return m_thread != 0;
  }

  Control * control( const std::string & path );

  /**
  @brief Perform staged control value changes
  */
  void update();

#if 0
  any controlValue( const std::string & path )
  {
    if (isRunning()) {
      Control * control = tracked_control(path);
      if (!control) {
        MRSERR("MarSystemThread: Can not get control value - control not being tracked: " << path);
        return any();
      }
      return control->value();
    } else {
      MRSERR("MarSystemThread: Can not get control value while not running.");
      return any();
    }
  }

  void setControlValue( const std::string & path, const any & value )
  {
    if (isRunning())
      push_request( new SetControlEvent(path, value) );
    else
      MRSERR("MarSystemThread: Can not set control value while not running.");
  }
#endif

private:
  friend class Control;
  friend class RunnerThread;

  Control * create_control( const std::string & path );

  void enqueue_control_value( const MarControlPtr & control, const any & value, bool push = true );
  void push_staged_control_values();

  void push_request( Event * request );
  void delete_processed_requests();

private:
  MarSystem * m_system;
  RunnerThread *m_thread;
  SetControlsEvent *m_set_controls_event;

  struct Shared {
    Shared(): request_queue(1000) {}
    EventQueue request_queue;
    std::map<std::string, Control*> controls;
  };

  Shared * m_shared;
};

class marsyas_EXPORT Control {
public:
  std::string path() const { return m_path; }

  any value() const;

  void setValue(const any &value, bool update = true);

  template <typename T>
  bool isValueType()
  {
    return typeid(*m_atomic) == typeid(AtomicControlT<T>);
  }

  bool isValueType( const std::string & type )
  {
    return (m_atomic->systemControl()->getType() == type);
  }

private:
  friend class Runner;
  friend class RunnerThread;

  Control( Runner * runner, const std::string & path, AtomicControl * atomic ):
    m_runner(runner),
    m_path(path),
    m_atomic(atomic)
  {}

  ~Control()
  {
    delete m_atomic;
  }

  void pull() { m_atomic->pull(); }
  void push() { m_atomic->push(); }

  Runner * m_runner;
  std::string m_path;
  AtomicControl * m_atomic;
};

} // namespace RealTime
} // namespace Marsyas

#endif // MARSYAS_THREAD_SYSTEM_INCLUDED
