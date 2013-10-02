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

#ifndef MARSYAS_MARCONTROLMANAGER_H
#define MARSYAS_MARCONTROLMANAGER_H

#include <marsyas/system/MarControl.h>

#include <map>
#include <vector>

namespace Marsyas
{
/**
\class MarControlManager
\brief MarControlManager handle controls creation
\author lfpt@inescporto.pt, lmartins@inescporto.pt
**/

class MarSystem;

class MarControlManager
{
protected:
  static MarControlManager *instance_;

  std::map<std::string, MarControlPtr> registry_;
  std::map<std::string, std::string> typeRegistry_;
  MarControlManager();

public:
  ~MarControlManager();

  static inline MarControlManager* getManager();

  void registerPrototype(std::string type, MarControlPtr);
  MarControlPtr create(std::string type);
  MarControlPtr createFromStream(std::string type, std::istream& in);

  bool isRegistered (std::string name);
  std::string getRegisteredType(std::string typeIdName);

  std::vector <std::string> registeredPrototypes();
};

inline
MarControlManager* MarControlManager::getManager()
{
  if (!instance_)
  {
    instance_ = new MarControlManager();
  }
  return instance_;
}

}

#endif /* MARSYAS_MARCONTROLMANAGER_H	*/

