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

#ifndef MARSYAS_REALTIME_UDP_TRANSMITTER_INCLUDED
#define MARSYAS_REALTIME_UDP_TRANSMITTER_INCLUDED

#include <marsyas/realtime/osc_transmitter.h>
#include <marsyas/export.h>

namespace Marsyas {
namespace RealTime {

/**
 * @brief Receives OSC packets as an OscSubscriber, and sends them over UDP.
 * @author Jakob Leben (jakob.leben@gmail.com)
 */

class marsyas_EXPORT UdpTransmitter : public OscSubscriber
{
public:
  UdpTransmitter( const char *address, int ip );
  virtual ~UdpTransmitter() {}
  bool hasDestination( const char *address, int ip );
  virtual void process( const char * data, int size );

private:
  struct private_data;
  private_data *p;
};

}
}

#endif // MARSYAS_REALTIME_UDP_TRANSMITTER_INCLUDED
