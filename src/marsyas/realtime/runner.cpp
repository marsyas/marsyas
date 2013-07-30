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

#include "runner.h"
#include "atomic_control.h"

#include "common_header.h"
#include "common_source.h"

#include <iostream>
#include <algorithm>
#include <cstring>
#include <cerrno>

#if defined(MARSYAS_MACOSX) || defined(MARSYAS_LINUX)
#  include <pthread.h>
#elif defined(MARSYAS_WIN32) || defined(MARSYAS_MINGW)
#  include <windows.h>
#endif

using namespace std;

namespace Marsyas {
namespace RealTime {

class RunnerThread
{
public:
  RunnerThread( MarSystem * system, Runner::Shared * shared ):
    m_system(system),
    m_shared(shared),
    m_stop(false),
    m_thread(&Marsyas::RealTime::RunnerThread::run, this)
  {
#if defined(MARSYAS_MACOSX) || defined(MARSYAS_LINUX)
    int policy;
    sched_param param;
    pthread_getschedparam( m_thread.native_handle(), &policy, &param );

    policy = SCHED_FIFO;
    int min_priority = sched_get_priority_min( policy );
    int max_priority = sched_get_priority_max( policy );
    int relative_priority = (int) ((max_priority - min_priority) * 0.75);
    int priority = min_priority + relative_priority;
    param.sched_priority = priority;

    if (pthread_setschedparam( m_thread.native_handle(), policy, &param ))
    {
      MRSWARN("RunnerThread: Failed to set thread scheduling policy and priority: "
              << std::strerror(errno));
    }
#elif defined(MARSYAS_WIN32) || defined(MARSYAS_MINGW)
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

  std::atomic<bool> m_stop;
  std::thread m_thread;
};

#if 1
template <typename T>
static void set_control_value( MarControlPtr & control, const any & value, bool update = true )
{
  T typed_value;
  try {
    typed_value = any_cast<T>( value );
  } catch ( bad_any_cast ) {
    return;
  };
  control->setValue(typed_value, update);
}

static void set_control_value( MarControlPtr & control, const any & value, bool update = true )
{
  std::string control_type = control->getType();
  if(control_type == "mrs_bool")
    set_control_value<bool>(control, value, update);
  else if(control_type == "mrs_real")
    set_control_value<mrs_real>(control, value, update);
  else if(control_type == "mrs_natural")
    set_control_value<mrs_natural>(control, value, update);
  else if(control_type == "mrs_string")
    set_control_value<mrs_string>(control, value, update);
  else {
    MRSERR(
          "Marsyas::Thread::System:: Can not set control value - unsupported type: "
          << control_type.c_str()
          );
    return;
  }
}

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
  else {
    MRSERR(
          "Marsyas::Thread::System:: Can not get control value - unsupported type: "
          << control_type.c_str()
          );
    return any();
  }
}
#endif

Runner::Runner(Marsyas::MarSystem * system):
  m_system(system),
  m_thread(0),
  m_set_controls_event(0),
  m_shared(new Shared)
{}

void Runner::start()
{
  if (!m_thread) {
    m_thread = new RunnerThread(m_system, m_shared);
  }
}

void Runner::stop()
{
  if (m_thread) {
    m_thread->stop();
    m_thread->join();

    delete m_thread;
    m_thread = 0;

    delete m_set_controls_event;
    m_set_controls_event = 0;
  }
}

void Runner::wait()
{
  if (m_thread) {
    m_thread->join();

    delete m_thread;
    m_thread = 0;

    delete m_set_controls_event;
    m_set_controls_event = 0;
  }
}

Control * Runner::control( const std::string & path )
{
  std::map<std::string, Control*>::iterator it = m_shared->controls.find(path);
  if (it != m_shared->controls.end())
    return it->second;
  else
    return create_control( path );
}

void Runner::update()
{
  if (isRunning())
    push_staged_control_values();
  else
    m_system->update();
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
    // FIXME: not real-time-safe
    atomic_control = new AtomicControlT<mrs_string>(sys_control);
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

void Runner::enqueue_control_value(const MarControlPtr & control,
                                   const any & value,
                                   bool push)
{
  if (push)
  {
    push_request( new SetControlEvent(control, value) );
  }
  else
  {
    if (!m_set_controls_event)
      m_set_controls_event = new SetControlsEvent;

    m_set_controls_event->control_values[control] = value;
  }
}

void Runner::push_staged_control_values()
{
  if (m_set_controls_event) {
    push_request( m_set_controls_event );
    m_set_controls_event = 0;
  }
}

void Runner::push_request( Event * request )
{
  bool ok = m_shared->request_queue.push(request);
  if (!ok)
    MRSERR("Marsyas::Thread::System: failed to push event! Event queue full?");
  delete_processed_requests();
}

void Runner::delete_processed_requests()
{
  Event *event;
  while( m_shared->request_queue.popProcessed(event) ) {
    delete event;
  }
}

///////////////////////////

void RunnerThread::run()
{
  //cout << "MarSystemThread: running" << endl;

  m_system->updControl("mrs_bool/active", true);

  while(!m_stop)
  {
    //cout << "tick" << endl;
    process_requests();

    m_system->tick();
#if 1
    for (std::pair<std::string, Control*> mapping : m_shared->controls)
      mapping.second->push();
#endif
  }

  m_system->updControl("mrs_bool/active", false);

  // make sure not to miss latest updates:
  process_requests();
}

void RunnerThread::process_requests()
{
  Event *event;
  while( m_shared->request_queue.pop(event) )
  {
    //cout << "MarSystemThread: popped event: type=" << event->type << endl;
    switch (event->type) {
    case SetControl:
    {
      SetControlEvent* request = static_cast<SetControlEvent*>(event);
      //cout << "MarSystemThread: setting control: " << request->control->getName() << endl;
      set_control_value( request->control, request->value );
      break;
    }
    case SetControlValue:
    {
      SetControlValueEvent* request = static_cast<SetControlValueEvent*>(event);
      //cout << "MarSystemThread: setting control value: " << request->path << endl;
      MarControlPtr control = m_system->getControl(request->path);
      if (control.isInvalid()) {
        MRSERR("Marsyas::Thread::System:: Can not set control - invalid path: " << request->path);
      } else
        set_control_value(control, request->value );
      break;
    }
    case SetControls:
    {
      SetControlsEvent *request = static_cast<SetControlsEvent*>(event);
      for( std::pair<const MarControlPtr, any> &mapping : request->control_values )
      {
        const bool do_not_update = false;
        //cout << "MarSystemThread: setting staged control: " << mapping.first->getName() << endl;
        // FIXME: evil const_cast:
        MarControlPtr & settable_control = const_cast<MarControlPtr &>(mapping.first);
        set_control_value(settable_control, mapping.second, do_not_update);
      }
      m_system->update();
      break;
    }
    default:
      MRSERR("Marsyas::Thread::System:: unsupported event type: " << event->type);
    }

    event->setProcessed();
  }
}

////////////////////////

any Control::value() const
{
  if (m_runner->isRunning())
    return m_atomic->value();
  else
    return get_control_value( m_atomic->systemControl() );
}

void Control::setValue(const any &value, bool update)
{
  MarControlPtr & control = m_atomic->systemControl();
  if (m_runner->isRunning())
    m_runner->enqueue_control_value( control, value, update );
  else
    set_control_value( control, value, update );
}


} // namespace RealTime
} // namespace Marsyas
