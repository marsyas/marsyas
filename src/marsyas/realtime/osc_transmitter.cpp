/*
** Copyright (C) 2014 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include <marsyas/system/MarSystem.h>
#include <marsyas/common_source.h>

#include <oscpack/osc/OscOutboundPacketStream.h>

#include <stack>
#include <vector>
#include <algorithm>

using namespace std;

namespace Marsyas {
namespace RealTime {

string OscTransmitter::make_osc_path( MarControlPtr control, char separator )
{
  string id("/");
  stack<MarSystem*> system_stack;
  //MarSystem *system_stack[30];
  //int system_stack_size = 0;

  MarSystem *system = control->getMarSystem();
  while( system != m_system )
  {
    system_stack.push(system);
    system = system->getParent();
  }

  while(!system_stack.empty())
  {
    MarSystem *system = system_stack.top();
    system_stack.pop();
    id.append(system->getName());
    id.push_back(separator);
  }

  id.append(control->id());

  return id;
}

bool OscTransmitter::subscribe( MarControlPtr control, OscSubscriber * subscriber )
{
  if (control.isInvalid())
    return false;

  string path = make_osc_path(control);

  string handler_name(path);
  std::replace( handler_name.begin(), handler_name.end(), '/', '.');
  string handler_path = control->getType() + '/' + handler_name;

  MarControlPtr handler = getControl(handler_path);
  if (handler.isInvalid())
  {
    addControl(handler_path, *control, handler);
    handler->setState(true);
    handler->linkTo(control, false);
    m_subscribers[handler()].path = path;
  }

  subscription &slot = m_subscribers[handler()];
  if (!slot.contains(subscriber))
    slot.add(subscriber);

  return true;
}

void OscTransmitter::unsubscribe( MarControlPtr control, OscSubscriber * subscriber )
{
  if (control.isInvalid())
    return;

  string path = make_osc_path(control);

  string handler_name(path);
  std::replace( handler_name.begin(), handler_name.end(), '/', '.');
  string handler_path = control->getType() + '/' + handler_name;

  MarControlPtr handler = getControl(handler_path);
  if (handler.isInvalid())
    return;

  // Find subscribers for control:
  auto subscribers_it = m_subscribers.find(handler());
  if (subscribers_it != m_subscribers.end())
  {
    // Find subscriber:
    subscription & slot = subscribers_it->second;
    slot.remove(subscriber);

    // If no subscriber for control left:
    if (slot.empty())
    {
      // Delete the map entry:
      m_subscribers.erase(subscribers_it);

      // Unlink the handler control:
      handler->unlinkFromAll();
      // TODO: Delete the handler control.
    }
  }
}

void OscTransmitter::myProcess(realvec &, realvec &)
{
  // no-op
}

void OscTransmitter::myUpdate( MarControlPtr handler )
{
  if (handler.isInvalid())
    return;

  auto subscribers_it = m_subscribers.find(handler());
  if (subscribers_it == m_subscribers.end())
    return;

  subscription & slot = subscribers_it->second;
  if (slot.empty())
    return;

  osc::OutboundPacketStream packet( m_buffer, m_buffer_size );
  try
  {
    packet << osc::BeginMessage( slot.path.c_str() );
    if (handler->hasType<bool>())
      packet << handler->to<bool>();
    else if (handler->hasType<mrs_natural>())
      packet << ((int) handler->to<mrs_natural>());
    else if (handler->hasType<mrs_real>())
      packet << ((float) handler->to<mrs_real>());
    else if (handler->hasType<mrs_string>())
      packet << handler->to<mrs_string>().c_str();
    else
      throw std::runtime_error("Unsupported control type.");
    packet << osc::EndMessage;
  }
  catch ( std::exception & e )
  {
    MRSWARN("OSC sender: " << e.what());
  }

  for( OscSubscriber * subscriber : slot.subscribers )
  {
    subscriber->process(packet.Data(), packet.Size());
  }
}

}
}
