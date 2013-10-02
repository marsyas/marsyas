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

#ifndef MARSYAS_DEBUG_RECORD_INCLUDED
#define MARSYAS_DEBUG_RECORD_INCLUDED

#include <marsyas/realvec.h>
#include <marsyas/export.h>

#include <map>
#include <string>

namespace Marsyas { namespace Debug {

class marsyas_EXPORT Record
{
public:
  struct Entry
  {
    Entry() {}
    Entry( const realvec & input, const realvec & output,
           double cpu_time, double real_time ):
      input(input),
      output(output),
      cpu_time(cpu_time),
      real_time(real_time)
    {}

    realvec input;
    realvec output;
    double cpu_time;
    double real_time;
  };

  typedef std::map<std::string, Entry> EntryMap;

  const EntryMap & entries() const { return m_entries; }

  void insert( const std::string & path, const Entry & entry )
  {
    m_entries[path] = entry;
  }

  const Entry * entry( const std::string & path ) const
  {
    const auto & entry = m_entries.find(path);
    if (entry != m_entries.end())
      return &entry->second;
    else
      return 0;
  }

  void clear() { m_entries.clear(); }

private:
  EntryMap m_entries;
};

}} // namespace Marsyas::Debug

#endif // MARSYAS_DEBUG_RECORD_INCLUDED
