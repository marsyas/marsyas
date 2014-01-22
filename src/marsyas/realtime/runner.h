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

#include <marsyas/realtime/atomic_control.h>
#include <marsyas/realtime/any.h>
#include <marsyas/realtime/packet_queue.h>
#include <marsyas/realtime/osc_receiver.h>
#include <marsyas/realtime/osc_transmitter.h>
#include <marsyas/system/MarSystem.h>

#include <thread>
#include <atomic>
#include <mutex>
#include <list>
#include <map>

namespace Marsyas {
namespace RealTime {

class Control;
class RunnerThread;
class OscTransmitter;
class OscReceiver;

/**
 * @brief Interaction with MarSystem running in real time.
 * @author Jakob Leben (jakob.leben@gmail.com)
 */

class marsyas_EXPORT Runner
{
public:

  Runner( MarSystem * system );
  ~Runner();

  MarSystem * system() { return m_system; }

  void setRtPriorityEnabled( bool on ) { m_realtime_priority = on; }

  void start(unsigned int ticks = 0);
  void stop();
  void wait();
  bool isRunning() const
  {
    return m_thread != 0;
  }

  void addController( OscProvider * );
  void removeController( OscProvider * );
  bool subscribe( const std::string & path, OscSubscriber * );
  void unsubscribe( const std::string & path,  OscSubscriber * );

  Control * control( const std::string & path );

private:
  friend class Control;
  friend class RunnerThread;

  Control * create_control( const std::string & path );

  void refit_realvec_controls();

private:
  MarSystem * m_system;

  bool m_realtime_priority;

  OscReceiver m_osc_receiver;
  OscTransmitter m_osc_transmitter;

  RunnerThread *m_thread;

  struct Shared
  {
    Shared(OscReceiver *osc_rcv): controller(osc_rcv) {}
    OscReceiver * controller;
    std::map<std::string, Control*> controls;
  };
  Shared * m_shared;
};

class marsyas_EXPORT Control
{
public:
  std::string path() const { return m_path; }

  any value() const;

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

  void resizeToFit()
  {
    if (isValueType<mrs_realvec>())
    {
      static_cast<AtomicControlT<mrs_realvec>*>(m_atomic)->resizeToFit();
    }
  }

  void push() { m_atomic->push(); }

  Runner * m_runner;
  std::string m_path;
  AtomicControl * m_atomic;
};

} // namespace RealTime
} // namespace Marsyas

#endif // MARSYAS_THREAD_SYSTEM_INCLUDED
