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

#ifndef MARSYAS_REALVEC_QUEUE_INCLUDED
#define MARSYAS_REALVEC_QUEUE_INCLUDED

#include <marsyas/realvec.h>
#include <marsyas/types.h>

#include <cassert>
#include <atomic>
#include <algorithm>

namespace Marsyas {

using namespace std;

class realvec_queue_producer;
class realvec_queue_consumer;

/**
   \ingroup NotmarCore
   \brief Lock-free fixed-size queue, implemented as a realvec-based ringbuffer
   \author Jakob Leben <jakob.leben@gmail.com>

   The relavec_queue is a thread-safe, lock-free, fixed-size queue (FIFO) of multiple channels
   of mrs_real values, for use by a single data producing thread and a single data consuming
   thread.

   "Fixed-size" means that the size (i.e. the amount of samples the queue can contain) can
   only be defined while other threads are not pushing and popping data (i.e. not in a
   thread-safe manner).

   "Single-producer, single-consumer" means that the queue is thread-safe only as long as a
   single thread is pushing to it, and a single thread is popping from it.

   "Lock-free" means that the thread synchronization only uses atomic operations and no mutexes
   or any other kind of synchronization methods subject to thread-priority inversion. This makes
   it suitable for real-time applications.

   This class operates in combination with the realvec_queue_producer and realvec_queue_consumer
   classes: the realvec_queue only contains the data to pass between the producer and the consumer,
   but does not provide an interface to write and read data. Those interfaces are provided by the
   other two classes, respectively.
*/

template <typename T>
T clipped(const T& lower, const T& n, const T& upper) {
  return std::max(lower, std::min(n, upper));
}

class realvec_queue
{
  typedef atomic<mrs_natural> atomic_size_t;

  realvec m_buffer;
  atomic_size_t m_read_position;
  atomic_size_t m_write_position;
  atomic_size_t m_capacity;

public:
  friend class realvec_queue_producer;
  friend class realvec_queue_consumer;

  /**
   * @brief Constructs a queue of size 0.
   */
  realvec_queue():
    m_read_position(0),
    m_write_position(0),
    m_capacity(0)
  {}

  /**
   * @brief Constructs a queue of desired size.
   * @param observations Number of channels.
   * @param samples Number of samples.
   */
  realvec_queue(mrs_natural observations, mrs_natural samples):
    m_buffer(observations, samples),
    m_read_position(0),
    m_write_position(0),
    m_capacity( samples )
  {}

  realvec_queue(mrs_natural observations, mrs_natural samples, mrs_natural capacity):
    m_buffer(observations, samples),
    m_read_position(0),
    m_write_position(0),
    m_capacity( clipped((mrs_natural) 0, capacity, samples) )
  {}

  /**
   * @brief Changes queue size. (NOT THREAD-SAFE)
   * @param observations Number of channels.
   * @param samples Number of samples.
   * @param clear Wheather to initialize all new space to 0, or keep data that overlaps with
   * the old data.
   */
  void resize(mrs_natural observations, mrs_natural samples, bool clear = true)
  {
    resize(observations, samples, samples, clear);
  }

  void resize(mrs_natural observations, mrs_natural samples, mrs_natural capacity, bool clear = true)
  {
    if (clear)
      m_buffer.create(observations, samples);
    else
      m_buffer.stretch(observations, samples);

    m_read_position = m_write_position = 0;
    m_capacity = clipped((mrs_natural)0, capacity, samples);
  }

  /**
   * @brief Equivalent to popping all data off queue. (NOT THREAD-SAFE)
   */
  void clear()
  {
    m_read_position = m_write_position = 0;
  }

  mrs_natural capacity()
  {
    return m_capacity.load( memory_order_relaxed );
  }

  mrs_natural set_capacity( mrs_natural capacity )
  {
    capacity = clipped((mrs_natural)0, capacity, samples());
    m_capacity.store( capacity, memory_order_relaxed );
    return capacity;
  }

  /**
   * @return The amount of channels the queue holds. (THREAD-SAFE)
   */
  mrs_natural observations() { return m_buffer.getRows(); }

  /**
   * @return The maximum amount of samples the queue can hold. (THREAD-SAFE)
   */
  mrs_natural samples() { return m_buffer.getCols(); }

  /**
   * @return The amount of samples that can be pushed into the queue. (THREAD-SAFE)
   */
  mrs_natural write_capacity()
  {
    mrs_natural read_pos = m_read_position.load(memory_order_relaxed);
    mrs_natural write_pos = m_write_position.load(memory_order_relaxed);
    mrs_natural capacity = m_capacity.load(memory_order_relaxed);
    mrs_natural size = samples();

    if (capacity > 0 )
      --capacity; // Should never write the last sample.

    mrs_natural written;
    if (write_pos >= read_pos)
      written = write_pos - read_pos;
    else
      written = size - (read_pos - write_pos);

    if (written >= capacity)
      return 0;
    else
      return capacity - written;
  }

  /**
   * @return The amount of samples that can be popped from the queue. (THREAD-SAFE)
   */
  mrs_natural read_capacity()
  {
    mrs_natural read_pos = m_read_position.load(memory_order_relaxed);
    mrs_natural write_pos = m_write_position.load(memory_order_acquire);
    mrs_natural available;
    if (write_pos >= read_pos)
      available = write_pos - read_pos;
    else
      available = samples() - (read_pos - write_pos);
    return available;
  }
};

/**
   \ingroup NotmarCore
   \brief Interface to write data to realvec_queue.
   \author Jakob Leben <jakob.leben@gmail.com>

   This class allows a producer of data to push onto a realvec_queue.
 */
class realvec_queue_producer
{
  realvec_queue & m_queue;
  mrs_natural m_capacity;
  mrs_natural m_position;

public:
  /**
   * @brief [THREAD-SAFE] Construct producer and reserve queue space for writing.
   * @param destination The queue to write to.
   * @param capacity The requested amount of samples.
   *
   * If the amount of free samples in the queue is smaller than requested,
   * no space is reserved, and capacity() will return 0.
   */
  realvec_queue_producer(realvec_queue & destination, mrs_natural capacity):
    m_queue( destination ),
    m_position( destination.m_write_position.load(memory_order_relaxed) )
  {
    assert(capacity >= 0);
    if (destination.write_capacity() < capacity)
      m_capacity = 0;
    else
      m_capacity = capacity;
  }

  /**
  @brief [THREAD-SAFE] Destroy producer and make the reserved space available
  to the realvec_queue_consumer for reading.
  */
  ~realvec_queue_producer()
  {
    if (m_capacity > 0) {
      mrs_natural position = (m_position + m_capacity) % m_queue.samples();
      m_queue.m_write_position.store(position, memory_order_release);
    }
  }

  /**
   * @brief [THREAD-SAFE] Amount of samples reserved for writing.
   */
  mrs_natural capacity() { return m_capacity; }

  /**
   * @brief [THREAD-SAFE] Reserve more queue space for writing.
   * @param capacity The requested amount of samples.
   * @return Whether the requested amount of samples was successfully reserved.
   *
   * It is only possible to reserve more samples than already reserved.
   * If less or equal amount is requested, this method will return true,
   * but the reserved amount will not change.
   *
   * If the amount of free samples in the queue is smaller than requested,
   * This method will return false, and the amount of reserved space will not change.
   */
  bool reserve( mrs_natural capacity )
  {
    assert(capacity >= 0);
    if (capacity <= m_capacity)
      return true;
    if (m_queue.write_capacity() < capacity)
      return false;
    m_capacity = capacity;
    return true;
  }

  /**
   * @brief [THREAD-SAFE] Access data reserved for writing.
   * @param observation Channel index in range of 0 to `queue.observations()`
   * @param sample Sample index, among reserved samples; in the range
   * of 0 to `capacity()`, where 0 denotes the first reserved sample, and so on...
   * @return A reference to the data at requested indexes.
   */
  mrs_real & operator() ( mrs_natural observation, mrs_natural sample )
  {
    assert(sample >= 0);
    assert(sample < m_capacity);
    sample = (m_position + sample) % m_queue.samples();
    return m_queue.m_buffer(observation, sample);
  }
};

/**
   \ingroup NotmarCore
   \brief Interface to read data from realvec_queue.
   \author Jakob Leben <jakob.leben@gmail.com>

   This class allows a consumer of data to pop off of a realvec_queue.
 */
class realvec_queue_consumer
{
  realvec_queue & m_queue;
  mrs_natural m_capacity;
  mrs_natural m_position;

public:
  /**
   * @brief [THREAD-SAFE] Construct consumer and reserve queue space for reading.
   * @param source The queue to read from.
   * @param capacity The requested amount of samples.
   *
   * If the amount of available samples in the queue is smaller than requested,
   * no space is reserved, and capacity() will return 0.
   */
  realvec_queue_consumer(realvec_queue & source, mrs_natural capacity):
    m_queue(source),
    m_position( source.m_read_position.load(memory_order_relaxed) )
  {
    assert(capacity >= 0);
    if (source.read_capacity() < capacity)
      m_capacity = 0;
    else
      m_capacity = capacity;
  }

  /**
  @brief [THREAD-SAFE] Destroy consumer and make the reserved space available
  to the realvec_queue_producer to reuse for writing.
  */
  ~realvec_queue_consumer()
  {
    if (m_capacity > 0) {
      mrs_natural position = (m_position + m_capacity) % m_queue.samples();
      m_queue.m_read_position.store(position, memory_order_release);
    }
  }

  /**
   * @brief [THREAD-SAFE] Amount of samples reserved for reading.
   */
  mrs_natural capacity() { return m_capacity; }

  /**
   * @brief [THREAD-SAFE] Reserve more queue space for reading.
   * @param capacity The requested amount of samples.
   * @return Whether the requested amount of samples was successfully reserved.
   *
   * It is only possible to reserve more samples than already reserved.
   * If less or equal amount is requested, this method will return true,
   * but the reserved amount will not change.
   *
   * If the amount of available samples in the queue is smaller than requested,
   * This method will return false, and the amount of reserved space will not change.
   */
  bool reserve( mrs_natural capacity )
  {
    assert(capacity >= 0);
    if (capacity <= m_capacity)
      return true;
    if (m_queue.read_capacity() < capacity)
      return false;
    m_capacity = capacity;
    return true;
  }

  /**
   * @brief [THREAD-SAFE] Access data reserved for reading.
   * @param observation Channel index in range of 0 to `queue.observations()`
   * @param sample Sample index, among reserved samples; in the range
   * of 0 to `capacity()`, where 0 denotes the first reserved sample, and so on...
   * @return A reference to the data at requested indexes.
   */
  mrs_real & operator() ( mrs_natural observation, mrs_natural sample )
  {
    assert(sample >= 0);
    assert(sample < m_capacity);
    sample = (m_position + sample) % m_queue.samples();
    return m_queue.m_buffer(observation, sample);
  }
};

} // namespace Marsyas

#endif // MARSYAS_REALVEC_QUEUE_INCLUDED
