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

#ifndef MARSYAS_REALTIME_UDP_RECEIVER_INCLUDED
#define MARSYAS_REALTIME_UDP_RECEIVER_INCLUDED

#include <marsyas/realtime/osc_receiver.h>
#include <marsyas/realtime/packet_queue.h>
#include <marsyas/export.h>

#include <string>
#include <thread>

namespace Marsyas {
namespace RealTime {

/**
 * @brief Awaits UDP packets and pushes them onto a packet_queue.
 * @author Jakob Leben (jakob.leben@gmail.com)
 */

class marsyas_EXPORT UdpReceiver : public OscQueueProvider
{
public:
  UdpReceiver( const std::string & address, int port,
               size_t queue_size = 1000 * 128 );
  ~UdpReceiver();

  void start();

  void stop();

  bool running() { return m_implementation != nullptr; }

  // NOTE: Only safe while queue consumer not running!
  void clearQueue() { m_queue.clear(); }

private:
  std::string m_address;
  int m_port;
  packet_queue m_queue;
  std::thread m_thread;

  class Implementation;
  friend class Implementation;
  Implementation *m_implementation;
};

}
}

#endif // MARSYAS_REALTIME_UDP_RECEIVER_INCLUDED
