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

#ifndef MARSYAS_THREAD_ATOMIC_CONTROL_INCLUDED
#define MARSYAS_THREAD_ATOMIC_CONTROL_INCLUDED

#include <marsyas/realtime/any.h>
#include <marsyas/realtime/stage.h>

#include <marsyas/common_header.h>
#include <marsyas/realvec.h>
#include <marsyas/system/MarControl.h>

#include <atomic>
#include <string>
#include <cstring>

namespace Marsyas {
namespace RealTime {

using std::atomic;

using Marsyas::MarControl;
using Marsyas::MarControlPtr;

/**
 * @brief Sets and gets value of MarControl via an atomic variable.
 * @author Jakob Leben (jakob.leben@gmail.com)
 */
class AtomicControl
{
public:
  AtomicControl( const MarControlPtr & control ):
    m_control(control)
  {}

  virtual ~AtomicControl() {}

  /**
  Get value of intermediate atomic variable
  */
  virtual any value() = 0;

  /**
  Set intermediate atomic variable to control value.
  */
  virtual void push() = 0;

  MarControlPtr & systemControl() { return m_control; }

protected:
  MarControlPtr m_control;
};

template<typename T>
class AtomicControlT : public AtomicControl
{
public:
  AtomicControlT( const MarControlPtr & control ):
    AtomicControl(control),
    m_value(T())
  {}

  any value()
  {
    return any(m_value.load());
  }

  void push()
  {
    m_value = m_control->to<T>();
  }

private:
  atomic<T> m_value;
};

template<>
class AtomicControlT<mrs_string> : public AtomicControl
{
public:
  AtomicControlT( const MarControlPtr & control ):
    AtomicControl(control)
  {}

  any value()
  {
    MRSERR("AtomicControlT<mrs_string>::value(): not yet implemented!");
    return any(mrs_string());
  }

  void push()
  {
    // FIXME: not real-time safe!
    // m_value = m_control->to<mrs_string>();
  }
};

template<>
class AtomicControlT<mrs_realvec> : public AtomicControl
{
public:
  AtomicControlT( const MarControlPtr & control ):
    AtomicControl(control)
  {
    resizeToFit();
  }

  void resizeToFit()
  {
    const mrs_realvec & vector = m_control->to<mrs_realvec>();
    m_rows = vector.getRows();
    m_columns = vector.getCols();
    m_value.create( m_rows, m_columns );
    m_stage.clear( m_value );
  }

  any value()
  {
    const_cast<stage<mrs_realvec>&>(m_stage).pop();
    if (m_stage.has_front())
      m_value = m_stage.front();
    return any(m_value);
  }

  void push()
  {
    const mrs_realvec & value = m_control->to<mrs_realvec>();
    if (value.getRows() == m_rows && value.getCols() == m_columns)
    {
      mrs_realvec & stage_value = m_stage.back();
      std::memcpy( stage_value.getData(),
                   value.getData(),
                   sizeof(mrs_real) * value.getSize() );
      m_stage.push();
    }
    else
    {
      MRSERR("AtomicControlT<mrs_realvec>::push(): realvec format mismatch!");
    }
  }

private:
  mrs_natural m_rows;
  mrs_natural m_columns;
  mrs_realvec m_value;
  stage<mrs_realvec> m_stage;
};


} // namespace RealTime
} // namespace Marsyas

#endif // MARSYAS_THREAD_ATOMIC_CONTROL_INCLUDED
