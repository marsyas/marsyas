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

#ifndef MARSYAS_REALTIME_PACKET_QUEUE_INCLUDED
#define MARSYAS_REALTIME_PACKET_QUEUE_INCLUDED

#include "queue.h"
#include <iostream>
#include <algorithm>

namespace Marsyas {
namespace RealTime {

using std::size_t;

class packet_queue
{

  queue<char> m_byte_queue;

public:
  packet_queue(size_t capacity):
    m_byte_queue(capacity)
  {}

  void clear() { m_byte_queue.clear(); }

  bool push ( const char *data, size_t count )
  {
    queue_producer<char> producer(m_byte_queue, sizeof(size_t) + count);

    if (!producer.capacity())
      return false;

    producer.write(0, reinterpret_cast<char*>(&count), sizeof(size_t));
    producer.write(sizeof(size_t), data, count);

    return true;
  }

  size_t pop ( char *data, size_t max_count )
  {
    size_t count;

    queue_consumer<char> consumer(m_byte_queue, sizeof(size_t));

    if (!consumer.capacity())
      return 0;

    consumer.read(0, reinterpret_cast<char*>(&count), sizeof(size_t));

    if (!consumer.reserve(sizeof(size_t) + count))
    {
      consumer.abort();
      return 0;
    }

    consumer.read( sizeof(size_t), data, std::min(count, max_count) );

    return count;
  }
};

}
}

#endif // MARSYAS_REALTIME_PACKET_QUEUE_INCLUDED
