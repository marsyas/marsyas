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

#ifndef MARSYAS_REALTIME_CONTROLLER_INCLUDED
#define MARSYAS_REALTIME_CONTROLLER_INCLUDED

#include <marsyas/realtime/osc_receiver.h>
#include <marsyas/realtime/packet_queue.h>
#include <marsyas/common_header.h>
#include <marsyas/export.h>

namespace Marsyas {
namespace RealTime {

class marsyas_EXPORT Controller : public OscQueueProvider
{
public:
  Controller( size_t queue_size = 1000 * 128 );

  void set( const char * path, bool value );
  void set( const char * path, int value );
  void set( const char * path, float value );
  void set( const char * path, double value );
  void set( const char * path, const char * text );
  void clearQueue() { m_queue.clear(); }

private:
  packet_queue m_queue;
  static const size_t m_buffer_size = 1024;
  MARSYAS_ALIGN(8) char m_buffer[m_buffer_size];
};

}
}

#endif // MARSYAS_REALTIME_CONTROLLER_INCLUDED
