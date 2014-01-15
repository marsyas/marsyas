#ifndef MARSYAS_REALTIME_OSC_DISPATCHER_HPP
#define MARSYAS_REALTIME_OSC_DISPATCHER_HPP

#include <marsyas/realtime/packet_queue.h>
#include <marsyas/system/MarSystem.h>

#include <oscpack/ip/IpEndpointName.h>
#include <oscpack/osc/OscPacketListener.h>

#include <memory>

namespace Marsyas {
namespace RealTime {

/**
 * @brief Real-time-safe dispatcher of received OSC messages
 * @author Jakob Leben <jakob.leben@gmail.com>
 *
 * Receives OSC messages over a packet_queue, for each message
 * finds a control within a MarSystem of which path matches the
 * OSC address, and sets the control to the first OSC value.
 */

class OscDispatcher : private osc::OscPacketListener
{
  MarSystem *m_system;
  packet_queue * m_queue;
  IpEndpointName m_endpoint;
  static const size_t m_buffer_size = 4096;
#ifdef _MSC_VER
  __declspec(align(8))
#else
  alignas(8)
#endif
  char m_buffer[m_buffer_size];

public:
  OscDispatcher( MarSystem * system, packet_queue * queue ):
    m_system(system),
    m_queue(queue)
  {}

  // Process all packets on the queue:
  void run();

private:
  void ProcessMessage( const osc::ReceivedMessage& message,
                       const IpEndpointName& );

  MarControlPtr find_control( const char * path );
};

}
}

#endif // MARSYAS_REALTIME_OSC_DISPATCHER_HPP
