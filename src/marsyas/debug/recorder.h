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

#ifndef MARSYAS_DEBUG_RECORDER_INCLUDED
#define MARSYAS_DEBUG_RECORDER_INCLUDED

#include <marsyas/debug/record.h>
#include <marsyas/system/MarSystem.h>
#include <marsyas/export.h>

#include <vector>

namespace Marsyas { namespace Debug {

class marsyas_EXPORT Recorder
{
public:
  Recorder(MarSystem *system);
  ~Recorder();

  const Record & record()
  {
    return m_record;
  }

  void commit()
  {
    for (Observer *observer : m_observers)
      commit_observer(observer);
  }

  void clear();

private:
  struct Observer;

  void recursive_add_observer(MarSystem *system);
  void commit_observer(Observer *observer);

private:
  MarSystem *m_system;
  Record m_record;
  std::vector<Observer*> m_observers;
};

}} // namespace Marsyas::Debug

#endif // MARSYAS_DEBUG_RECORDER_INCLUDED
