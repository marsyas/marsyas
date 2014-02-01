/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include <marsyas/system/MarControlValue.h>
#include <marsyas/system/MarControl.h>
#include <marsyas/system/MarControlManager.h>
#include "../common_source.h"

#include <algorithm>


using std::ostringstream;
using namespace Marsyas;


/************************************************************************/
/* MarControlValue implementation                                       */
/************************************************************************/

void
MarControlValue::setDebugValue()
{
#ifdef MARSYAS_TRACECONTROLS
  ostringstream oss;
  serialize(oss);
  value_debug_ = oss.str();
#endif
}

void MarControlValue::updateMarSystemFor( MarControl *control )
{
  control->callMarSystemUpdate();
}

mrs_string
MarControlValue::getType() const
{
  return type_;
}

mrs_string
MarControlValue::getRegisteredType()
{
  return MarControlManager::getManager()->getRegisteredType(this->getTypeID());
}
