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

#ifndef MARSYAS_OUTPUT_STRING_STREAM_H
#define MARSYAS_OUTPUT_STRING_STREAM_H

#include <marsyas/export.h>

#include <string>
#include <sstream>

namespace Marsyas
{
/**
	\class marostring
	\ingroup Notmar
	\brief abstract class for Marsyas Network string representation generators
	\author inb@cs.uvic.ca
*/

class marsyas_EXPORT marostring
{
protected:
  std::ostringstream result_;

public:
  marostring();
  marostring(const marostring& m);
  virtual ~marostring();

  virtual void begin_marsystem(bool isComposite, std::string type, std::string name)=0;
  virtual void end_marsystem(bool isComposite, std::string type, std::string name);

  virtual void begin_controls(int num_controls)=0;
  virtual void begin_control(std::string type, std::string name, std::string value, bool has_state)=0;
  virtual void begin_control_links_in(int num_links);
  virtual void put_control_link_in(std::string abspath, std::string type, std::string name);
  virtual void end_control_links_in(int num_links);
  virtual void begin_control_links_out(int num_links);
  virtual void put_control_link_out(std::string abspath, std::string type, std::string name);
  virtual void end_control_links_out(int num_links);
  virtual void end_control(std::string type, std::string name, std::string value, bool has_state);
  virtual void end_controls(int num_controls);

  virtual void begin_children(int num_children)=0;
  virtual void end_children(int num_children);

  virtual std::string str();
  virtual void clear();
};

}//namespace Marsyas

#endif
