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

#include <atomic>
#include <cassert>
#include <realvec.h>

namespace Marsyas {

using namespace std;

class realvec_queue_producer;
class realvec_queue_consumer;

/**
   \ingroup NotmarCore
   \brief Lock-free fixed-size queue, implemented as a realvec-based ringbuffer
   \author George Tzanetakis <gtzan@cs.uvic.ca>
   \author Jakob Leben <jakob.leben@gmail.com>
*/

class realvec_queue
{
	typedef atomic<size_t> position_t;

	realvec m_buffer;
	position_t m_read_position;
	position_t m_write_position;


public:
	friend class realvec_queue_producer;
	friend class realvec_queue_consumer;

	realvec_queue():
	m_read_position(0),
	m_write_position(0)
	{}

	realvec_queue(size_t observations, size_t samples):
	m_buffer(observations, samples),
	m_read_position(0),
	m_write_position(0)
	{}

	void resize(size_t observations, size_t samples, bool clear = true)
	{
		if (clear)
			m_buffer.create(observations, samples);
		else
			m_buffer.stretch(observations, samples);

		m_read_position = m_write_position = 0;
	}

	void clear()
	{
		m_read_position = m_write_position = 0;
	}

	size_t observations() { return m_buffer.getRows(); }

	size_t samples() { return m_buffer.getCols(); }

	size_t write_capacity()
	{
		size_t read_pos = m_read_position.load(memory_order_acquire);
		size_t write_pos = m_write_position.load(memory_order_relaxed);
		size_t available;
		if (write_pos >= read_pos)
			available = samples() - (write_pos - read_pos);
		else
			available = read_pos - write_pos;
		available -= 1;
		return available;
	}

	size_t read_capacity()
	{
		size_t read_pos = m_read_position.load(memory_order_relaxed);
		size_t write_pos = m_write_position.load(memory_order_acquire);
		size_t available;
		if (write_pos >= read_pos)
			available = write_pos - read_pos;
		else
			available = samples() - (read_pos - write_pos);
		return available;
	}
};

class realvec_queue_producer
{
	realvec_queue & m_queue;
	size_t m_capacity;
	size_t m_position;

public:
	realvec_queue_producer(realvec_queue & destination, size_t capacity):
	m_queue( destination ),
	m_position( destination.m_write_position.load(memory_order_relaxed) )
	{
		if (destination.write_capacity() < capacity)
			m_capacity = 0;
		else
			m_capacity = capacity;
	}

	~realvec_queue_producer()
	{
		if (m_capacity > 0) {
			size_t position = (m_position + m_capacity) % m_queue.samples();
			m_queue.m_write_position.store(position, memory_order_release);
		}
	}

	size_t capacity() { return m_capacity; }

	bool reserve( size_t capacity )
	{
		if (capacity <= m_capacity)
			return true;
		if (m_queue.write_capacity() < capacity)
			return false;
		m_capacity = capacity;
		return true;
	}

	mrs_real & operator() ( size_t observation, size_t sample )
	{
		assert(sample < m_capacity);
		sample = (m_position + sample) % m_queue.samples();
		return m_queue.m_buffer(observation, sample);
	}
};

class realvec_queue_consumer
{
	realvec_queue & m_queue;
	size_t m_capacity;
	size_t m_position;

public:
	realvec_queue_consumer(realvec_queue & destination, size_t capacity):
	m_queue(destination),
	m_position( destination.m_read_position.load(memory_order_relaxed) )
	{
		if (destination.read_capacity() < capacity)
			m_capacity = 0;
		else
			m_capacity = capacity;
	}

	~realvec_queue_consumer()
	{
		if (m_capacity > 0) {
			size_t position = (m_position + m_capacity) % m_queue.samples();
			m_queue.m_read_position.store(position, memory_order_release);
		}
	}

	size_t capacity() { return m_capacity; }

	bool reserve( size_t capacity )
	{
		if (capacity <= m_capacity)
			return true;
		if (m_queue.read_capacity() < capacity)
			return false;
		m_capacity = capacity;
		return true;
	}

	mrs_real & operator() ( size_t observation, size_t sample )
	{
		assert(sample < m_capacity);
		sample = (m_position + sample) % m_queue.samples();
		return m_queue.m_buffer(observation, sample);
	}
};

} // namespace Marsyas

#endif // MARSYAS_REALVEC_QUEUE_INCLUDED
