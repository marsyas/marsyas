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

#include <marsyas/debug/recorder.h>
#include "timer.h"

namespace Marsyas { namespace Debug {

struct Recorder::Observer : public MarSystemObserver
{
  MarSystem *system;
  std::string path;
  Timer timer;
  realvec input;
  realvec output;
  double m_cpu_time;
  double m_real_time;

  Observer( MarSystem *system ):
    system(system),
    path(system->getAbsPath()),
    m_cpu_time(0),
    m_real_time(0)
  {
    system->addObserver(this);
  }

  ~Observer()
  {
    system->removeObserver(this);
  }

  void preProcess( const realvec &in )
  {
    input = in;
    timer.start();
  }

  void postProcess( const realvec &out )
  {
    timer.measure();
    output = out;
    m_cpu_time += timer.cpuTime();
    m_real_time += timer.realTime();
  }

  void reset()
  {
    m_cpu_time = 0;
    m_real_time = 0;
  }

  Record::Entry record()
  {
    return Record::Entry(input, output,
                         m_cpu_time, m_real_time);
  }
};

Recorder::Recorder(MarSystem *system):
  m_system(system)
{
  recursive_add_observer(system);
}

Recorder::~Recorder()
{
  for (Observer *observer : m_observers)
  {
    delete observer;
  }
}

void Recorder::clear()
{
  m_record.clear();

  for (Observer *observer : m_observers)
    observer->reset();
}

void Recorder::recursive_add_observer(MarSystem *system)
{
  Observer *observer = new Observer(system);
  m_observers.push_back(observer);

  std::vector<MarSystem*> children = system->getChildren();
  for (MarSystem *child : children)
    recursive_add_observer(child);
}

void Recorder::commit_observer(Observer *observer)
{
  m_record.insert(observer->path, observer->record());
}


}} // namespace Marsyas::Debug
