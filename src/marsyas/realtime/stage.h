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

#ifndef MARSYAS_REALTIME_STAGE_INCLUDED
#define MARSYAS_REALTIME_STAGE_INCLUDED

#include <array>
#include <atomic>

namespace Marsyas {
namespace RealTime {

using std::array;
using std::atomic_int;
using std::atomic;

/**
  @ingroup NotmarCore
  @brief Thread-safe lock-free exchange of large data.
  @author Jakob Leben <jakob.leben@gmail.com>

  This class facilitates lock-free exchange of large data between a
  single producer thread and a single consumer thread, in a functionaly
  similar manner to an atomic variable.
. It is ensured that the producer can always write new data (never
  needs to wait), and that the consumer always sees the latest data, but
  some data may be lost if it is not consumed before more data is produced.

  The class holds an internal array of 3 instances of the template parameter
  type. A reference to one of them is provided to the consumer and another one to
  the producer. The third data space is used for exchange - it is a kind of
  staging area: when the producer is finished writing to its space, it uses
  push() to exchange that space with the staging space, and when the consumer
  is finished using its space it uses pop() to exchange it with the staging
  space.

  By only exchanging references to data, data types of any complexity
  and size can be passed between threads in a lock-free manner, at the cost
  of keeping 3 times the size of the individual data space.
 */
template <typename T>
class stage
{
  struct item {
    bool valid;
    T data;
    item(): valid(false) {}
  };

  array<item, 3> m_items;

  atomic<int> m_stage_index;
  int m_back_index;
  int m_front_index;

public:
  stage( const T & initial_value = T() ):
    m_stage_index(0),
    m_back_index(1),
    m_front_index(2)
  {
    clear(initial_value);
  }

  /**
   * @brief NOT THREAD-SAFE.
   */
  void clear( const T & initial_value = T() )
  {
    for (int i = 0; i < 3; ++i)
    {
      m_items[i].data = initial_value;
      m_items[i].valid = false;
    }
  }

  T & back()
  {
    return m_items[m_back_index].data;
  }

  void push()
  {
    m_items[m_back_index].valid = true;
    m_back_index = m_stage_index.exchange(m_back_index, std::memory_order_release);
  }

  bool has_front() const
  {
    return m_items[m_front_index].valid;
  }

  T & front()
  {
    if (!has_front())
      throw std::out_of_range("stage: Tried to access unavailable front.");
    return m_items[m_front_index].data;
  }

  void pop()
  {
    m_items[m_front_index].valid = false;
    m_front_index = m_stage_index.exchange(m_front_index, std::memory_order_acquire);
  }
};

}
} // namespace Marsyas::RealTime

#endif // MARSYAS_REALTIME_STAGE_INCLUDED
