#ifndef MARSYAS_REALTIME_OSC_SENDER_HPP
#define MARSYAS_REALTIME_OSC_SENDER_HPP

#include <marsyas/system/MarSystem.h>

#include <oscpack/ip/IpEndpointName.h>
#include <oscpack/ip/UdpSocket.h>

#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <memory>

namespace Marsyas {
namespace RealTime {

/**
 * @brief Real-time-safe sender of OSC messages.
 * @author Jakob Leben <jakob.leben@gmail.com>
 *
 * Monitors changes of controls withing a MarSystem and sends that
 * information as OSC messages with OSC address equal to path of control.
 * To send changes of a particular control, an explicit subscription
 * has to be made.
 */

class OscSender : public MarSystem
{
  struct subscription
  {
    std::string path;
    std::vector<IpEndpointName> subscribers;
    void add(IpEndpointName endpoint)
    {
      subscribers.push_back(endpoint);
    }
    void remove(IpEndpointName endpoint)
    {
      auto subscriber_it = find(subscribers.begin(), subscribers.end(), endpoint);
      if (subscriber_it != subscribers.end())
        subscribers.erase(subscriber_it);
    }
    bool contains(IpEndpointName endpoint) const
    {
      return std::find(subscribers.begin(), subscribers.end(), endpoint) != subscribers.end();
    }
    bool empty() const { return subscribers.empty(); }
  };

  MarSystem * m_system;
  UdpSocket m_socket;
  static const size_t m_buffer_size = 4096;
  static const size_t max_key_length = 512;
#ifndef _MSC_VER
  alignas(64)
#endif
  char m_buffer[m_buffer_size];
  std::map<MarControl*, subscription> m_subscribers;

public:
  OscSender( MarSystem * system ):
    MarSystem("OscSender", "OscSender"),
    m_system(system)
  {
  }

  MarSystem *clone() const
  {
    // no-op
    return 0;
  }

  /**
   * @brief Subscribe receiver to changes of a particular control.
   * @param Path of control, in the form of "/child-system/child-system/.../control-name"
   * @param IP host of receiver.
   * @param IP port of receiver.
   * @return
   */
  bool subscribe( const std::string & path, const char * address, int port )
  {
    if (!path.size() || path[0] != '/')
      return false;
    MarControlPtr control = m_system->remoteControl(path.substr(1));
    return subscribe(control, address, port);
  }

  bool subscribe( MarControlPtr control, const char * address, int port );

  /**
   * @brief Unsubscribe receiver from changes of a particular control.
   * @param Path of control, in the form of "/child-system/child-system/.../control-name"
   * @param IP host of receiver.
   * @param IP port of receiver.
   * @return
   */
  void unsubscribe( const std::string & path, const char * address, int port )
  {
    if (!path.size() || path[0] != '/')
      return;
    MarControlPtr control = m_system->remoteControl(path.substr(1));
    return unsubscribe(control, address, port);
  }

  void unsubscribe( MarControlPtr control, const char * address, int port );

  void myProcess(realvec &, realvec &);
  void myUpdate( MarControlPtr handler );

private:
  std::string make_osc_path( MarControlPtr control, char separator = '/' );
};

}
}

#endif // MARSYAS_REALTIME_OSC_SENDER_HPP
