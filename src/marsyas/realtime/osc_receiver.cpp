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

#include "osc_receiver.h"

#include <oscpack/ip/IpEndpointName.h>
#include <oscpack/osc/OscPacketListener.h>
#include <oscpack/osc/OscReceivedElements.h>

#include <string>
#include <algorithm>

using namespace std;

namespace Marsyas {
namespace RealTime {

void process_packet( MarSystem * root_system, const char * data, size_t size );
void process_bundle( MarSystem * root_system, const osc::ReceivedBundle& bundle );
void process_message( MarSystem * root_system, const osc::ReceivedMessage& message );

void OscReceiver::addProvider( OscProvider * provider )
{
  auto provider_pos = find(m_providers.begin(), m_providers.end(), provider);
  if (provider_pos == m_providers.end())
    m_providers.push_back(provider);
}

void OscReceiver::removeProvider( OscProvider * provider )
{
  auto provider_pos = find(m_providers.begin(), m_providers.end(), provider);
  if (provider_pos != m_providers.end())
    m_providers.erase(provider_pos);
}

void OscReceiver::processPacket(const char *data, size_t size)
{
  process_packet(m_system, data, size);
}

void OscReceiver::run()
{
  OscProviderDestination me(this);

  for ( OscProvider *provider : m_providers )
  {
    provider->provide(me);
  }
}

MarSystem *find_child_system(MarSystem * parent, const char * name, size_t count)
{
  const std::vector<MarSystem*> & children = parent->children();
  for( MarSystem * child : children )
  {
    const string & child_name = child->getName();
    if (child_name.size() == count &&
        equal(name, name + count, child_name.data()))
    {
      return child;
    }
  }

  return 0;
}

MarControlPtr find_local_control(MarSystem * system, const char *name, size_t count)
{
  const auto & controls = system->controls();
  for ( const auto & mapping : controls )
  {
    const MarControlPtr & control = mapping.second;
    const string & id = control->id();
    if (id.size() == count &&
        equal(name, name + count, id.data()))
    {
      return control;
    }
  }

  return MarControlPtr();
}

MarControlPtr find_control( MarSystem *root_system, const char * path )
{
  size_t path_len = strlen(path);
  const char * path_end = path + path_len;
  const char * component = path;

  MarSystem * system = root_system;

  while(system)
  {
    const char * separator = std::find(component, path_end, '/');
    size_t component_len = separator - component;
    if (separator != path_end)
    {
      system = find_child_system(system, component, component_len );
    }
    else
    {
      return find_local_control(system, component, component_len);
    }
    component = separator + 1;
  }

  return MarControlPtr();
}

void process_packet( MarSystem * root_system, const char * data, size_t size )
{
  osc::ReceivedPacket packet( data, size );
  if( packet.IsBundle() )
      process_bundle( root_system, osc::ReceivedBundle(packet) );
  else
      process_message( root_system, osc::ReceivedMessage(packet) );
}

void process_bundle( MarSystem * root_system, const osc::ReceivedBundle& bundle )
{
  // ignore bundle time tag for now
  for( osc::ReceivedBundle::const_iterator i = bundle.ElementsBegin();
       i != bundle.ElementsEnd(); ++i )
  {
    if( i->IsBundle() )
      process_bundle( root_system, osc::ReceivedBundle(*i) );
    else
      process_message( root_system, osc::ReceivedMessage(*i) );
  }
}

void process_message( MarSystem * root_system, const osc::ReceivedMessage& message )
{
  const char * path = message.AddressPattern();
  if (path[0] == '/') ++path;

  // FIXME: Constructing std::string is not real-time-safe.
  MarControlPtr control = find_control(root_system, path);
  if (control.isInvalid())
  {
    MRSWARN("OSC receiver: no control for path: " << path);
    return;
  }

  try
  {
    osc::ReceivedMessage::const_iterator it = message.ArgumentsBegin();
    if (it == message.ArgumentsEnd())
      throw std::runtime_error("OSC receiver: Message has no arguments.");

    char tag = it->TypeTag();
    switch(tag)
    {
    case osc::TRUE_TYPE_TAG:
    case osc::FALSE_TYPE_TAG:
      control->setValue(it->AsBoolUnchecked());
      break;
    case osc::INT32_TYPE_TAG:
      control->setValue(it->AsInt32Unchecked());
      break;
    case osc::FLOAT_TYPE_TAG:
      control->setValue((mrs_real) it->AsFloatUnchecked());
      break;
    case osc::DOUBLE_TYPE_TAG:
      control->setValue((mrs_real) it->AsDoubleUnchecked());
      break;
    case osc::STRING_TYPE_TAG:
      control->setValue(it->AsStringUnchecked());
      break;
    default:
      throw std::runtime_error("OSC receiver: Unsupported message argument type.");
    }
  }
  catch ( std::exception & e )
  {
    MRSWARN("OSC receiver: error while parsing message: " << e.what());
  }
}

void OscQueueProvider::provide( OscProviderDestination & destination )
{
  size_t packet_size;

  while( (packet_size = m_queue->pop(m_buffer, m_buffer_size)) )
  {
    if (packet_size > m_buffer_size)
    {
      MRSWARN("OSC receiver: dropped too large OSC packet.");
      continue;
    }
    destination.provide(m_buffer, packet_size);
  }
}

}
}
