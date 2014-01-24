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

#include "osc_transmitter.h"
#include "osc_receiver.h"

#include <marsyas/realtime/runner.h>
#include <marsyas/realtime/atomic_control.h>
#include <marsyas/realtime/packet_queue.h>
#include <marsyas/common_header.h>

#include "../common_source.h"

#include <iostream>
#include <algorithm>
#include <functional>
#include <memory>
#include <map>
#include <vector>
#include <cstring>
#include <cerrno>

#if defined(MARSYAS_MACOSX) || defined(MARSYAS_LINUX) || defined(MARSYAS_MINGW)
#  include <pthread.h>
#elif defined(MARSYAS_WIN32)
#  include <windows.h>
#endif

using namespace std;

namespace Marsyas {
namespace RealTime {

class RunnerThread
{
public:
  RunnerThread( MarSystem * system, Runner::Shared * shared,
                bool realtime_priority, unsigned int ticks ):
    m_system(system),
    m_shared(shared),
    m_ticks(ticks > 0 ? ticks : -1),
    m_stop(false),
    m_thread(&Marsyas::RealTime::RunnerThread::run, this)
  {
#if defined(MARSYAS_MACOSX) || defined(MARSYAS_LINUX) || defined(MARSYAS_MINGW)
    int policy;
    sched_param param;
    pthread_getschedparam( m_thread.native_handle(), &policy, &param );

    policy = realtime_priority ? SCHED_RR : SCHED_OTHER;
    int min_priority = sched_get_priority_min( policy );
    int max_priority = sched_get_priority_max( policy );
    int relative_priority = (int) ((max_priority - min_priority) * 0.6);
    int priority = min_priority + relative_priority;
    param.sched_priority = priority;

    if (pthread_setschedparam( m_thread.native_handle(), policy, &param ))
    {
      MRSWARN("RunnerThread: Failed to set thread scheduling policy and priority: "
              << std::strerror(errno));
    }
#elif defined(MARSYAS_WIN32)
    if (!SetThreadPriority( m_thread.native_handle(), THREAD_PRIORITY_HIGHEST ))
    {
      MRSWARN("RunnerThread: Failed to set thread priority.");
    }
#else
    MRSWARN("RunnerThread: Increasing thread priority on this platform is not implemented yet.");
#endif
  }

  void stop()
  {
    m_stop = true;
  }

  void join()
  {
    m_thread.join();
  }

private:
  void run();
  void process_requests();

  Marsyas::MarSystem *m_system;

  Runner::Shared * m_shared;

  int m_ticks;
  std::atomic<bool> m_stop;
  std::thread m_thread;
};

static any get_control_value( const MarControlPtr & control )
{
  std::string control_type = control->getType();
  if(control_type == "mrs_bool")
    return any( control->to<bool>() );
  else if(control_type == "mrs_real")
    return any( control->to<mrs_real>() );
  else if(control_type == "mrs_natural")
    return any( control->to<mrs_natural>() );
  else if(control_type == "mrs_string")
    return any( control->to<mrs_string>() );
  else if(control_type == "mrs_realvec")
    return any( control->to<mrs_realvec>() );
  else {
    MRSERR(
      "Marsyas::Thread::System:: Can not get control value - unsupported type: "
      << control_type.c_str()
    );
    return any();
  }
}

Runner::Runner(Marsyas::MarSystem * system):
  m_system(system),
  m_realtime_priority(false),
  m_osc_receiver(system),
  m_osc_transmitter(system),
  m_thread(0),
  m_shared(new Shared(&m_osc_receiver))
{
}

Runner::~Runner()
{
  stop();

  delete m_shared;
}

void Runner::start(unsigned int ticks)
{
  if (!m_thread)
  {
    refit_realvec_controls();

    m_thread = new RunnerThread(m_system, m_shared, m_realtime_priority, ticks);
  }
}

void Runner::stop()
{
  if (m_thread)
  {
    m_thread->stop();
    m_thread->join();

    delete m_thread;
    m_thread = 0;
  }
}

void Runner::wait()
{
  if (m_thread)
  {
    m_thread->join();

    delete m_thread;
    m_thread = 0;
  }
}

void Runner::addController( OscProvider * controller )
{
  if (isRunning())
  {
    MRSERR("Runner: can not add OSC controller while running.");
    return;
  }

  m_osc_receiver.addProvider(controller);
}

void Runner::removeController( OscProvider * controller )
{
  if (isRunning())
  {
    MRSERR("Runner: can not remove OSC controller while running.");
    return;
  }

  m_osc_receiver.removeProvider(controller);
}

bool Runner::subscribe( const std::string & path, OscSubscriber *subscriber )
{
  if (isRunning())
  {
    MRSERR("Runner: can not add OSC subscriptions while running.");
    return false;
  }

  return m_osc_transmitter.subscribe( path, subscriber );
}

void Runner::unsubscribe( const std::string & path,  OscSubscriber *subscriber )
{
  if (isRunning())
  {
    MRSERR("Runner: can not remove OSC subscriptions while running.");
    return;
  }

  m_osc_transmitter.unsubscribe( path, subscriber );
}

Control * Runner::control( const std::string & path )
{
  std::map<std::string, Control*>::iterator it = m_shared->controls.find(path);
  if (it != m_shared->controls.end())
    return it->second;
  else
    return create_control( path );
}

Control * Runner::create_control( const std::string & control_path )
{
  if (isRunning()) {
    MRSERR("Marsyas::Thread::System:: can not start tracking controls while running.");
    return 0;
  }

  Marsyas::MarControlPtr sys_control = m_system->getControl( control_path );
  if (sys_control.isInvalid()) {
    MRSERR("Marsyas::Thread::System:: Can not track control - invalid path: " << control_path);
    return 0;
  }

  AtomicControl *atomic_control;

  std::string sys_control_type = sys_control->getType();
  if(sys_control_type == "mrs_bool")
    atomic_control = new AtomicControlT<bool>(sys_control);
  else if(sys_control_type == "mrs_real")
    atomic_control = new AtomicControlT<mrs_real>(sys_control);
  else if(sys_control_type == "mrs_natural")
    atomic_control =  new AtomicControlT<mrs_natural>(sys_control);
  else if(sys_control_type == "mrs_string")
    atomic_control = new AtomicControlT<mrs_string>(sys_control);
  else if(sys_control_type == "mrs_realvec")
    atomic_control = new AtomicControlT<mrs_realvec>(sys_control);
  else {
    MRSERR(
      "Marsyas::Thread::System:: Can not track control - unsupported type: "
      << sys_control_type.c_str()
    );
    return 0;
  }

  Control * control = new Control(this, control_path, atomic_control);

  m_shared->controls.insert(std::pair<std::string, Control*>(control_path, control));

  return control;
}

void Runner::refit_realvec_controls()
{
for (const auto & mapping : m_shared->controls)
  {
    Control *control = mapping.second;
    control->resizeToFit();
  }
}

///////////////////////////

static void init_audio_recursive( MarSystem * system )
{
  MarControlPtr init_audio_control = system->getControl("mrs_bool/initAudio");
  if (!init_audio_control.isInvalid())
    init_audio_control->setValue(true);
  for ( MarSystem * child : system->children() )
    init_audio_recursive( child );
}

void RunnerThread::run()
{
  //cout << "MarSystemThread: running" << endl;

  init_audio_recursive(m_system);

  m_system->updControl("mrs_bool/active", true);

  MarControlPtr done_control = m_system->getControl("mrs_bool/done");
  function<bool()> not_system_done;
  if (done_control.isInvalid())
    not_system_done = [](){return true;};
  else
    not_system_done = [&done_control](){return !done_control->to<mrs_bool>();};

  int ticks_done = 0;
  int ticks_left = m_ticks;

  while ( !m_stop &&
          ticks_left &&
          not_system_done() )
  {
    //cout << "tick" << endl;

    m_shared->controller->run();

    m_system->tick();

    for (const auto & mapping : m_shared->controls)
      mapping.second->push();

    if (ticks_left > 0)
      --ticks_left;
    ++ticks_done;
  }

#if 0
  if (done_control->to<mrs_bool>())
  {
    cout << "done in " << ticks_done << " ticks " << endl;
  }
  else if (ticks_left == 0)
  {
    cout << "timeout!" << endl;
  }
#endif

  m_system->updControl("mrs_bool/active", false);

  // make sure not to miss latest updates:
  m_shared->controller->run();
}


////////////////////////

any Control::value() const
{
  if (m_runner->isRunning())
    return m_atomic->value();
  else
    return get_control_value( m_atomic->systemControl() );
}

} // namespace RealTime
} // namespace Marsyas
