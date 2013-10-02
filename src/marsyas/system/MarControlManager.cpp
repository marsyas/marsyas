/*
** Copyright (C) 1998-2005 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include <marsyas/system/MarControlManager.h>
#include <marsyas/system/MarControlValue.h>

using namespace Marsyas;
using std::ostringstream;

MarControlManager* MarControlManager::instance_ = NULL;

MarControlManager::MarControlManager()
{
  /* Will be used when controls are created only using MarControlManager
  registerPrototype("mrs_natural", new MarControl(0L));
  registerPrototype("mrs_real", new MarControl(0.0));
  registerPrototype("mrs_string", new MarControl("empty"));
  registerPrototype("mrs_bool", new MarControl(false));
  registerPrototype("mrs_realvec", new MarControl(realvec()));*/
}

MarControlManager::~MarControlManager()
{
  registry_.clear();
  if (instance_) delete instance_;
}

void
MarControlManager::registerPrototype(mrs_string type, MarControlPtr control)
{
  registry_[type] = control;
  typeRegistry_[control->value_->getTypeID()] = type;
}

MarControlPtr
MarControlManager::create(mrs_string type)
{
  if (registry_.find(type) != registry_.end())
  {
    MarControl *mc = registry_[type]->clone();
    return mc;
  }

  else
  {
    MRSWARN("MarControlManager::getPrototype: No prototype found for " + type);
    return MarControlPtr();
  }
}

MarControlPtr
MarControlManager::createFromStream(std::string type, std::istream& in)
{
  MarControlPtr ctrl = create(type);
  if (!ctrl.isInvalid())
  {
    ctrl->value_->createFromStream(in);
  }
  else
  {
    MRSWARN("MarControl::createFromStream Trying to create unknown control type.");
  }
  return ctrl;
}

bool MarControlManager::isRegistered (mrs_string name)
{
  return (registry_.find(name) != registry_.end());
}

std::string MarControlManager::getRegisteredType(std::string typeIdName)
{
  if (registry_.find(typeIdName) != registry_.end())
  {
    return typeRegistry_[typeIdName];
  }
  MRSWARN("MarControlManager::getRegisteredType Unknown type is being queried.");
  return "mrs_unknown";
}
