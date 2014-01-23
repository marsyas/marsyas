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

#ifndef MARSYAS_REALTIME_OSC_DISPATCHER_HPP
#define MARSYAS_REALTIME_OSC_DISPATCHER_HPP

#include <marsyas/system/MarSystem.h>
#include <marsyas/realtime/packet_queue.h>
#include <marsyas/export.h>
#include <marsyas/common_header.h>

#include <memory>
#include <vector>

namespace Marsyas {
namespace RealTime {

struct OscProvider;

/**
 * @brief Real-time-safe dispatcher of received OSC messages
 * @author Jakob Leben <jakob.leben@gmail.com>
 *
 * Receives OSC messages over a packet_queue, for each message
 * finds a control within a MarSystem of which path matches the
 * OSC address, and sets the control to the first OSC value.
 */

class marsyas_EXPORT OscReceiver
{
  MarSystem *m_system;
  std::vector<OscProvider*> m_providers;

public:
  OscReceiver( MarSystem * system ):
    m_system(system)
  {}

  void addProvider( OscProvider * provider );

  void removeProvider( OscProvider * provider );

  void processPacket( const char * data, size_t size );

  // Process all providers:
  void run();
};

struct OscProviderDestination
{
  void provide( const char *data, size_t size )
  {
    m_receiver->processPacket(data, size);
  }

private:
  friend class OscReceiver;
  OscProviderDestination( OscReceiver * receiver ): m_receiver(receiver) {}
  OscReceiver *m_receiver;
};

struct OscProvider
{
  virtual void provide( OscProviderDestination & ) = 0;
};

class marsyas_EXPORT OscQueueProvider : public OscProvider
{
public:
  OscQueueProvider(packet_queue *queue):
    m_queue(queue)
  {}
  virtual ~OscQueueProvider() {}
  virtual void provide( OscProviderDestination & destination );

private:
  static const size_t m_buffer_size = 4096;
  MARSYAS_ALIGN(8) char m_buffer[m_buffer_size];

  packet_queue *m_queue;
};

}
}

#endif // MARSYAS_REALTIME_OSC_DISPATCHER_HPP
