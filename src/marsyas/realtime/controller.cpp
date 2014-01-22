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

#include "controller.h"

#include <oscpack/osc/OscOutboundPacketStream.h>

namespace Marsyas {
namespace RealTime {

Controller::Controller( size_t queue_size ):
  OscQueueProvider(&m_queue),
  m_queue(queue_size)
{}


template <typename T>
void write_osc_packet( char *buffer, size_t capacity,
                       packet_queue & queue,
                       const char * path, const T & value )
{
  osc::OutboundPacketStream packet( buffer, capacity );
  try
  {
    packet << osc::BeginMessage(path);
    packet << value;
    packet << osc::EndMessage;
  }
  catch ( std::exception & e )
  {
    MRSWARN("OSC sender: " << e.what());
    return;
  }
  queue.push( packet.Data(), packet.Size() );
}

void Controller::set( const char * path, bool value )
{
  write_osc_packet(m_buffer, m_buffer_size, m_queue, path, value);
}

void Controller::set( const char * path, int value )
{
  write_osc_packet(m_buffer, m_buffer_size, m_queue, path, value);
}

void Controller::set( const char * path, float value )
{
  write_osc_packet(m_buffer, m_buffer_size, m_queue, path, value);
}

void Controller::set( const char * path, double value )
{
  write_osc_packet(m_buffer, m_buffer_size, m_queue, path, value);
}

void Controller::set( const char * path, const char * value )
{
  write_osc_packet(m_buffer, m_buffer_size, m_queue, path, value);
}

}
}
