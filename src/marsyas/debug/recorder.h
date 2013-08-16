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

#include "record.h"
#include <MarSystem.h>

#include <vector>

namespace Marsyas { namespace Debug {

class Recorder
{
public:
  Recorder(MarSystem *system):
    m_system(system)
  {
    recursive_add_observer(system);
  }

  ~Recorder()
  {
    for (Observer *observer : m_observers)
    {
      delete observer;
    }
  }

  const Record & record() { return m_record; }

  void clear() { m_record.clear(); }

private:
  struct Observer : public MarSystemObserver
  {
    MarSystem *system;
    std::string path;
    Recorder *recorder;
    Record::Entry entry;

    Observer( MarSystem *system, Recorder *recorder ):
      system(system),
      path(system->getAbsPath()),
      recorder(recorder)
    {
      system->addObserver(this);
    }

    ~Observer()
    {
      system->removeObserver(this);
    }

    void preProcess( const realvec &in )
    {
      entry.input = in;
    }

    void postProcess( const realvec &out )
    {
      entry.output = out;
      recorder->m_record.insert(path, entry);
    }
  };

  friend struct Observer;

  void recursive_add_observer(MarSystem *system)
  {
    Observer *observer = new Observer(system, this);
    m_observers.push_back(observer);

    std::vector<MarSystem*> children = system->getChildren();
    for (MarSystem *child : children)
      recursive_add_observer(child);
  }

private:
  MarSystem *m_system;
  Record m_record;
  std::vector<Observer*> m_observers;
};

}} // namespace Marsyas::Debug

#endif // MARSYAS_DEBUG_RECORDER_INCLUDED
