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

#include "udp_receiver.h"
#include "packet_queue.h"
#include <marsyas/common_source.h>

#include <oscpack/ip/UdpSocket.h>
#include <oscpack/ip/PacketListener.h>

#include <memory>
#include <cassert>
#include <iostream>

using namespace std;

namespace Marsyas {
namespace RealTime {

class UdpReceiver::Implementation : public PacketListener
{
  packet_queue *m_queue;
  SocketReceiveMultiplexer m_mux;

public:
  Implementation( packet_queue *queue ):
    m_queue(queue)
  {
  }

  void run( const std::string & address, int port )
  {
    std::unique_ptr<UdpSocket> socket;
    try {
      socket.reset(new UdpSocket);
      socket->Bind( IpEndpointName( address.c_str(), port ) );
    }
    catch (std::exception & e)
    {
      MRSERR("UDP receiver: Failed to set up UDP socket. Port already in use?");
      return;
    }

    m_mux.AttachSocketListener( socket.get(), this );
    m_mux.Run();
    m_mux.DetachSocketListener( socket.get(), this );
  }

  void stop()
  {
    m_mux.AsynchronousBreak();
  }

private:

  void ProcessPacket( const char *data, int size,
                      const IpEndpointName& )
  {
    m_queue->push(data, size);
  }
};

UdpReceiver::UdpReceiver( const std::string & address, int port,
                          size_t queue_size ):
  OscQueueProvider(&m_queue),
  m_address(address),
  m_port(port),
  m_queue(queue_size),
  m_implementation(nullptr)
{
}

UdpReceiver::~UdpReceiver()
{
  if (running())
    stop();
}

void UdpReceiver::start()
{
  if (m_implementation)
  {
    MRSERR("UdpReceiver: Can not start: already running.");
    return;
  }
  m_implementation = new Implementation(&m_queue);
  m_thread = thread(&Implementation::run, m_implementation, m_address, m_port);
}

void UdpReceiver::stop()
{
  if (!m_implementation)
  {
    MRSERR("UdpReceiver: Can not stop: not running.");
    return;
  }
  m_implementation->stop();
  m_thread.join();
  delete m_implementation;
  m_implementation = nullptr;
}

}
}
