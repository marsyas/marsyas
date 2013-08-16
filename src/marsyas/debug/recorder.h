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

#include <map>

namespace Marsyas { namespace Debug {

class Recorder : protected MarSystemObserver
{
  MarSystem *m_system;
  Record m_record;

public:
  Recorder(MarSystem *system):
    m_system(system)
  {
    recursive_set_observer(system);
  }

  ~Recorder()
  {
    recursive_remove_observer(m_system);
  }

  const Record & record() { return m_record; }

  void clear() { m_record.clear(); }

private:
  void processed( MarSystem * system, const realvec &in, const realvec &out)
  {
    (void) in;
    Record::Entry entry;
    entry.input = in;
    entry.output = out;
    m_record.insert(system->getAbsPath(), entry);
  }

  void recursive_set_observer(MarSystem *system)
  {
    system->addObserver(this);

    std::vector<MarSystem*> children = system->getChildren();
    for (MarSystem *child : children)
      recursive_set_observer(child);
  }

  void recursive_remove_observer(MarSystem *system)
  {
    system->removeObserver(this);

    std::vector<MarSystem*> children = system->getChildren();
    for (MarSystem *child : children)
      recursive_remove_observer(child);
  }
};

}} // namespace Marsyas::Debug

#endif // MARSYAS_DEBUG_RECORDER_INCLUDED
