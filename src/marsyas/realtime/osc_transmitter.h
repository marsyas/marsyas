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

#ifndef MARSYAS_REALTIME_OSC_SENDER_HPP
#define MARSYAS_REALTIME_OSC_SENDER_HPP

#include <marsyas/system/MarSystem.h>
#include <marsyas/export.h>
#include <marsyas/common_header.h>

#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <memory>

namespace Marsyas {
namespace RealTime {

struct OscSubscriber
{
  virtual void process( const char * data, int size ) = 0;
};

/**
 * @brief Real-time-safe generator of OSC messages with control change info.
 * @author Jakob Leben <jakob.leben@gmail.com>
 *
 * Monitors changes of controls within a MarSystem. For each change generates
 * an OSC message with new control data and address equal to path of control,
 * then calls subscribers to that particular control to process the OSC message.
 * Only monitors controls and generates OSC messages specifically subscribed
 * to.
 */

class marsyas_EXPORT OscTransmitter : public MarSystem
{
public:
  OscTransmitter( MarSystem * system ):
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
   * @param path Path of control, in the form of "/child-system/child-system/.../control-name"
   * @param address IP host of receiver.
   * @param port IP port of receiver.
   * @return
   */
  bool subscribe( const std::string & path, OscSubscriber * subscriber )
  {
    if (!path.size() || path[0] != '/')
      return false;
    MarControlPtr control = m_system->remoteControl(path);
    return subscribe(control, subscriber);
  }

  bool subscribe( MarControlPtr control, OscSubscriber * subscriber );

  /**
   * @brief Unsubscribe receiver from changes of a particular control.
   * @param path Path of control, in the form of "/child-system/child-system/.../control-name"
   * @param address IP host of receiver.
   * @param port IP port of receiver.
   * @return
   */
  void unsubscribe( const std::string & path, OscSubscriber * subscriber )
  {
    if (!path.size() || path[0] != '/')
      return;
    MarControlPtr control = m_system->remoteControl(path);
    return unsubscribe(control, subscriber);
  }

  void unsubscribe( MarControlPtr control, OscSubscriber * subscriber );

  void myProcess(realvec &, realvec &);
  void myUpdate( MarControlPtr handler );

private:
  struct subscription
  {
    std::string path;
    std::vector<OscSubscriber*> subscribers;
    void add(OscSubscriber * subscriber)
    {
      subscribers.push_back(subscriber);
    }
    void remove(OscSubscriber * subscriber)
    {
      auto subscriber_it = find(subscribers.begin(), subscribers.end(), subscriber);
      if (subscriber_it != subscribers.end())
        subscribers.erase(subscriber_it);
    }
    bool contains(OscSubscriber * subscriber) const
    {
      return std::find(subscribers.begin(), subscribers.end(), subscriber) != subscribers.end();
    }
    bool empty() const { return subscribers.empty(); }
  };

  std::string make_osc_path( MarControlPtr control, char separator = '/' );

  MarSystem * m_system;
  static const size_t m_buffer_size = 4096;
  static const size_t max_key_length = 512;
  MARSYAS_ALIGN(8) char m_buffer[m_buffer_size];
  std::map<MarControl*, subscription> m_subscribers;
};

}
}

#endif // MARSYAS_REALTIME_OSC_SENDER_HPP
