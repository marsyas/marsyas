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

#ifndef MARSYAS_THREAD_EVENT_QUEUE_INCLUDED
#define MARSYAS_THREAD_EVENT_QUEUE_INCLUDED

#include <marsyas/system/MarControl.h>
#include <marsyas/realtime/any.h>
#include <marsyas/realtime/queue.h>

#include <atomic>
#include <list>
#include <map>
#include <string>
#include <iostream>

namespace Marsyas {
namespace RealTime {

using std::list;
using std::string;

class EventQueue;

struct Event
{
  Event(int type): type(type), processed(false)
  {}

  virtual ~Event() {}

  const int type;

  void setProcessed()
  {
    processed.store(true, std::memory_order_release);
  }

private:
  friend class EventQueue;

  std::atomic<bool> processed;

};

class EventQueue
{
  typedef queue<Event*> BaseEventQueue;
  typedef list<Event*> EventList;

public:
  EventQueue(size_t size):
    m_queue(size)
  {}

  bool push( Event * event )
  {
    bool ok = m_queue.push( event );
    if (ok)
      m_processed.push_front(event);
    //std::cout << "EventQueue: pushing event"
    //<< " (now " << m_processed.size() << " events)" << std::endl;
    return ok;
  }

  bool pop( Event *& event )
  {
    return m_queue.pop( event );
  }

  bool popProcessed( Event *& processed_event)
  {
    if (m_processed.empty())
      return 0;
    Event * event = m_processed.back();
    bool is_processed = event->processed.load( std::memory_order_acquire );
    if (is_processed) {
      m_processed.pop_back();
      processed_event = event;
      //std::cout << "EventQueue: popping processed event"
      //<< " (" << m_processed.size() << " remaining)" << std::endl;
    }
    return is_processed;
  }

private:
  BaseEventQueue m_queue;
  EventList m_processed;
};

//////////////////////////////

enum EventType
{
  SetControl,
  SetControls,
  SetControlValue,
};

struct SetControlValueEvent : Event
{
  SetControlValueEvent( const string & path, const any & value ):
    Event(SetControlValue),
    path(path),
    value(value)
  {}

  string path;
  any value;
};

struct SetControlEvent : Event
{
  SetControlEvent( MarControlPtr control, const any & value ):
    Event(SetControl),
    control(control),
    value(value)
  {}

  MarControlPtr control;
  any value;
};

struct SetControlsEvent : Event
{
  SetControlsEvent(): Event(SetControls) {}
  std::map<MarControlPtr, any> control_values;
};

} // namespace RealTime
} // namespace Marsyas

#endif // MARSYAS_THREAD_EVENT_QUEUE_INCLUDED
