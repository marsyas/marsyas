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

#include <marsyas/debug/debugger.h>

namespace Marsyas { namespace Debug {

void compare( const Record & actual, const Record & reference, BugReport & report )
{
  using Marsyas::mrs_realvec;
  using Marsyas::mrs_real;
  using Marsyas::mrs_natural;

  for (const auto & ref_entry : reference.entries())
  {
    Bug bug;

    const std::string & path = ref_entry.first;

    const auto & actual_entry_it = actual.entries().find(path);
    if (actual_entry_it == actual.entries().end())
    {
      bug.flags = path_missing;
      report.insert( std::make_pair(path, bug) );
      continue;
    }

    const mrs_realvec & ref_data = ref_entry.second.output;
    const mrs_realvec & act_data = actual_entry_it->second.output;

    if (ref_data.getRows() != act_data.getRows() ||
        ref_data.getCols() != act_data.getCols())
    {
      bug.flags = format_mismatch;
      report.insert( std::make_pair(path, bug) );
      continue;
    }

    mrs_real max_dev = 0.0, avg_dev = 0.0;
    for (int i = 0; i < ref_data.getSize(); ++i)
    {
      if (ref_data(i) != act_data(i))
      {
        mrs_real dev = std::abs( act_data(i) - ref_data(i) );
        if (dev > max_dev)
          max_dev = dev;
        avg_dev += dev;
      }
    }

    if (ref_data.getSize())
      avg_dev /= ref_data.getSize();

    if (max_dev != 0.0)
    {
      bug.flags = value_mismatch;
      bug.average_deviation = avg_dev;
      bug.max_deviation = max_dev;
      report.insert( std::make_pair(path, bug) );
      continue;
    }
  }
}

}} // namespace Marsyas::Debug
