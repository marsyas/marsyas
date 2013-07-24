/*
** Copyright (C) 1998-2013 George Tzanetakis <gtzan@cs.uvic.ca>
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

#ifndef MARSYAS_REALTIME_QUEUE_INCLUDED
#define MARSYAS_REALTIME_QUEUE_INCLUDED

#include <atomic>
#include <vector>

namespace Marsyas {
namespace RealTime {

using std::atomic;
using std::vector;

/**
 * @brief Generic, lock-free, single-producer-single-consumer queue.
 * @author Jakob Leben (jakob.leben@gmail.com)
 */

template<typename T>
class queue
{
  typedef atomic<size_t> atomic_size_t;
  typedef vector<T> vector_t;

  vector_t m_vector;
  atomic_size_t m_write_index;
  atomic_size_t m_read_index;

public:
  queue( size_t size ):
    m_vector(size),
    m_write_index(0),
    m_read_index(0)
  {}

  bool push( const T & source )
  {
    if (write_capacity() < 1)
      return false;

    size_t write_index = m_write_index.load( std::memory_order_relaxed );
    m_vector[write_index] = source;
    m_write_index.store( (write_index + 1) % m_vector.size(),
                         std::memory_order_release );
    return true;
  }

  bool pop( T & destination )
  {
    if (read_capacity() < 1)
      return false;

    size_t read_index = m_read_index.load( std::memory_order_relaxed );
    destination = m_vector[read_index];
    m_read_index.store( (read_index + 1) % m_vector.size(),
                        std::memory_order_relaxed );
    return true;
  }

private:
  size_t write_capacity()
  {
    size_t read_pos = m_read_index.load(std::memory_order_relaxed);
    size_t write_pos = m_write_index.load(std::memory_order_relaxed);
    size_t capacity = m_vector.size();

    size_t available;
    if (write_pos >= read_pos)
      available = capacity - (write_pos - read_pos);
    else
      available = read_pos - write_pos;
    available -= 1;
    return available;
  }

  size_t read_capacity()
  {
    size_t read_pos = m_read_index.load(std::memory_order_relaxed);
    size_t write_pos = m_write_index.load(std::memory_order_acquire);
    size_t capacity = m_vector.size();

    size_t available;
    if (write_pos >= read_pos)
      available = write_pos - read_pos;
    else
      available = capacity - (read_pos - write_pos);
    return available;
  }
};

}
}

#endif
