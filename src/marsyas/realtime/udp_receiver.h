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

#include <string>

namespace Marsyas {
namespace RealTime {

class packet_queue;

/**
 * @brief Awaits UDP packets and pushes them onto a packet_queue.
 * @author Jakob Leben (jakob.leben@gmail.com)
 */

class UdpReceiver
{
public:
  UdpReceiver( const std::string & address, int port,
               packet_queue * queue );

  void run();

  // stop() is safe to call from a single thread
  // other than the thread calling "run()"
  void stop();

private:
  std::string m_address;
  int m_port;

  class Implementation;
  friend class Implementation;
  Implementation *m_implementation;
};

}
}

#endif // MARSYAS_REALTIME_UDP_RECEIVER_INCLUDED
