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

//#ifndef __MARCONTROL__
//#error Do not include this file directly, include only MarControl.h
//#endif

#include <marsyas/system/MarControl.h>

namespace Marsyas
{
/**
\author Luis F. Teixeira
\author Luis Gustavo Martins (adapted from from MarsyasX to Marsyas0.2 - 24.08.2007)
\date 21/07/2007
*/
class MarControlAccessor
{
private:
  MarControlPtr ctrl_;
  bool update_;
  bool readOnlyAccess_;

public:
  MarControlAccessor(MarControlPtr ctrl, bool update = true, bool readOnlyAccess = false)
  {
    ctrl_ = ctrl;
    update_ = update;
    readOnlyAccess_ = readOnlyAccess;
  }

  ~MarControlAccessor()
  {
#ifdef MARSYAS_TRACECONTROLS
    ctrl_->value_->setDebugValue();
#endif

    if(update_)
      ctrl_->value_->callMarSystemsUpdate();
  }

  void enableUpdates() {update_ = true;};
  void disableUpdates() {update_ = false;};

  template<class T> T& to()
  {
    return const_cast<T&>(ctrl_->to<T>());
  }
};
}

