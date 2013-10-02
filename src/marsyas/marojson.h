/*
** Copyright (C) 1998-2005 George Tzanetakis <gtzan@cs.uvic.ca>
** 2010 Paul Reimer <pdreimer@engr.uvic.ca>
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

#ifndef MARSYAS_OUTPUT_JSON_H
#define MARSYAS_OUTPUT_JSON_H

#include <string>
#include <vector>
#include <marsyas/marostring.h>


namespace Marsyas
{
/**
	\class marojson
	\ingroup Notmar
	\brief generate json representing a MarSystem and its controls
	\author Paul Reimer pdreimer@engr.uvic.ca
*/

class marsyas_EXPORT marojson : public marostring
{
private:

public:
  marojson();
  virtual ~marojson();

  virtual void begin_marsystem(bool isComposite, std::string type, std::string name);
  virtual void end_marsystem(bool isComposite, std::string type, std::string name);

  virtual void begin_controls(int num_controls);
  virtual void begin_control(std::string type, std::string name, std::string value, bool has_state);
  virtual void begin_control_links_in(int num_links);
  virtual void put_control_link_in(std::string abspath, std::string type, std::string name);
  virtual void end_control_links_in(int num_links);
  virtual void begin_control_links_out(int num_links);
  virtual void put_control_link_out(std::string abspath, std::string type, std::string name);
  virtual void end_control_links_out(int num_links);
  virtual void end_control(std::string type, std::string name, std::string value, bool has_state);
  virtual void end_controls(int num_controls);

  virtual void begin_children(int num_children);
  virtual void end_children(int num_children);

  virtual std::string str();

  bool prettyPrint;
  std::string& smart_indent(std::string& str,
                            const std::string& start, const std::string& end);

  std::string& replace_between(std::string& str,
                               const std::string &oldsubstr, const std::string &newsubstr,
                               std::string::size_type from_pos, std::string::size_type to_pos);

  std::string& replace_all(std::string& str,
                           const std::string& oldsubstr, const std::string& newsubstr);
};

}//namespace Marsyas

#endif
