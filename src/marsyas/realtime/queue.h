/*
** Copyright (C) 2013-2014 George Tzanetakis <gtzan@cs.uvic.ca>
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
#include <cstddef>
#include <cassert>

namespace Marsyas {
namespace RealTime {

using std::atomic;
using std::vector;
using std::size_t;

/**
 * @brief Generic, lock-free, single-producer-single-consumer queue.
 * @author Jakob Leben (jakob.leben@gmail.com)
 */

template<typename T> class queue_producer;
template<typename T> class queue_consumer;

template<typename T>
class queue
{
  typedef atomic<size_t> atomic_size_t;
  typedef vector<T> vector_t;

  vector_t m_vector;
  atomic_size_t m_write_index;
  atomic_size_t m_read_index;

public:
  friend class queue_producer<T>;
  friend class queue_consumer<T>;

  queue( size_t size ):
    m_vector(size),
    m_write_index(0),
    m_read_index(0)
  {}

  void clear()
  {
    m_read_index = m_write_index = 0;
  }

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

  bool push( const T * source, size_t count )
  {
    if (write_capacity() < count)
      return false;

    size_t write_index = m_write_index.load( std::memory_order_relaxed );
    size_t source_index = 0;
    size_t queue_size = m_vector.size();

    while( source_index < count && write_index < queue_size )
    {
      m_vector[write_index] = source[source_index];
      ++source_index;
      ++write_index;
    }

    if ( write_index == queue_size )
      write_index = 0;

    while( source_index < count )
    {
      m_vector[write_index] = source[source_index];
      ++source_index;
      ++write_index;
    }

    m_write_index.store( write_index, std::memory_order_release );

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

  bool pop( T * destination, size_t count )
  {
    if (read_capacity() < count)
      return false;

    size_t read_index = m_read_index.load( std::memory_order_relaxed );
    size_t dest_index = 0;
    size_t queue_size = m_vector.size();

    while ( dest_index < count && read_index < queue_size )
    {
      destination[dest_index] = m_vector[read_index];
      ++dest_index;
      ++read_index;
    }

    if (read_index == queue_size)
      read_index = 0;

    while ( dest_index < count )
    {
      destination[dest_index] = m_vector[read_index];
      ++dest_index;
      ++read_index;
    }

    m_read_index.store( read_index, std::memory_order_relaxed );

    return true;
  }

  size_t capacity()
  {
    return m_vector.size();
  }

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


template <typename T>
class queue_producer
{
  queue<T> & m_queue;
  size_t m_capacity;
  size_t m_position;

public:
  queue_producer(queue<T> & destination, size_t capacity):
    m_queue( destination ),
    m_position( destination.m_write_index.load(std::memory_order_relaxed) )
  {
    if (destination.write_capacity() < capacity)
      m_capacity = 0;
    else
      m_capacity = capacity;
  }

  ~queue_producer()
  {
    if (m_capacity > 0)
    {
      size_t new_write_index = (m_position + m_capacity) % m_queue.capacity();
      m_queue.m_write_index.store(new_write_index, std::memory_order_release);
    }
  }

  size_t capacity() { return m_capacity; }

  bool reserve( size_t capacity )
  {
    if (m_queue.write_capacity() < capacity)
      return false;
    m_capacity = capacity;
    return true;
  }

  T & operator [] ( size_t position )
  {
    assert(position < m_capacity);
    size_t queue_position = (m_position + position) % m_queue.capacity();
    return m_queue.m_vector[queue_position];
  }

  void write ( size_t position, const T * source, size_t count )
  {
    assert(position + count <= m_capacity);

    size_t queue_capacity = m_queue.capacity();
    size_t write_index = m_position + position;
    size_t source_index = 0;

    while( source_index < count && write_index < queue_capacity )
    {
      m_queue.m_vector[write_index] = source[source_index];
      ++source_index;
      ++write_index;
    }

    write_index = write_index % queue_capacity;

    while( source_index < count )
    {
      m_queue.m_vector[write_index] = source[source_index];
      ++source_index;
      ++write_index;
    }
  }

  void abort()
  {
    reserve(0);
  }
};

template<typename T>
class queue_consumer
{
  queue<T> & m_queue;
  size_t m_capacity;
  size_t m_position;

public:

  queue_consumer(queue<T> & source, size_t capacity):
    m_queue(source),
    m_position( source.m_read_index.load(std::memory_order_relaxed) )
  {
    if (source.read_capacity() < capacity)
      m_capacity = 0;
    else
      m_capacity = capacity;
  }

  ~queue_consumer()
  {
    if (m_capacity > 0) {
      size_t new_read_index = (m_position + m_capacity) % m_queue.capacity();
      m_queue.m_read_index.store(new_read_index, std::memory_order_release);
    }
  }

  size_t capacity() { return m_capacity; }

  bool reserve( size_t capacity )
  {
    if (m_queue.read_capacity() < capacity)
      return false;
    m_capacity = capacity;
    return true;
  }

  T & operator[] ( size_t position )
  {
    assert(position < m_capacity);
    size_t queue_position = (m_position + position) % m_queue.capacity();
    return m_queue.m_vector[queue_position];
  }

  void read ( size_t position, T * destination, size_t count )
  {
    assert(position + count <= m_capacity);

    size_t queue_capacity = m_queue.capacity();
    size_t read_index = m_position + position;
    size_t dest_index = 0;

    while ( dest_index < count && read_index < queue_capacity )
    {
      destination[dest_index] = m_queue.m_vector[read_index];
      ++dest_index;
      ++read_index;
    }

    read_index = read_index % queue_capacity;

    while ( dest_index < count )
    {
      destination[dest_index] = m_queue.m_vector[read_index];
      ++dest_index;
      ++read_index;
    }
  }

  void abort()
  {
    reserve(0);
  }
};

}
}

#endif
