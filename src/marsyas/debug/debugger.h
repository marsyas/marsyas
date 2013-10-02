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

#ifndef MARSYAS_DEBUG_DEBUGGER_INCLUDED
#define MARSYAS_DEBUG_DEBUGGER_INCLUDED

#include <marsyas/debug/record.h>
#include <marsyas/system/MarSystem.h>
#include <marsyas/export.h>

namespace Marsyas { namespace Debug {

enum BugFlags {
  no_bug = 0,
  path_missing,
  format_mismatch,
  value_mismatch
};

struct marsyas_EXPORT Bug {
  Bug() : flags(no_bug), max_deviation(0.0), average_deviation(0.0) {}
  BugFlags flags;
  Marsyas::mrs_real max_deviation;
  Marsyas::mrs_real average_deviation;
};

typedef std::map<std::string, Bug> BugReport;

marsyas_EXPORT
void compare( const Record & actual, const Record & reference, BugReport & report );

}} // namespace Marsyas::Debug

#endif // MARSYAS_DEBUG_DEBUGGER_INCLUDED
