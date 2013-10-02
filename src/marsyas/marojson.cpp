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


#include <marsyas/marojson.h>
#include <sstream>
#include <iostream>

//using namespace std;
using namespace Marsyas;

marojson::marojson() : marostring()
{
  this->prettyPrint = true;
}

marojson::~marojson()
{
  clear();
}

void
marojson::begin_marsystem(bool isComposite, std::string type, std::string name)
{
  result_ << "'" << name << "':{";

  result_ << "'isComposite':";
  if(isComposite)
    result_ << "true";
  else
    result_ << "false";

  result_ << ",";

  result_ << "'type':'" << type << "',";
}

void
marojson::end_marsystem(bool isComposite, std::string type, std::string name)
{
  (void) isComposite;
  (void) type;
  (void) name;
  result_ << "},";
}

void
marojson::begin_controls(int num_controls)
{
  if (num_controls > 0)
    result_ << "'controls':[";
}

void
marojson::begin_control(std::string type, std::string name, std::string value, bool has_state)
{
  (void) has_state;
  result_ << "{";
  result_ << "'type':'" << type << "',";
  result_ << "'name':'" << name << "',";

  result_ << "'value':";

  if (type == "mrs_realvec" || type == "mrs_string")
    result_ << "'";

  if (value=="")
    result_ << "MARSYAS_EMPTYSTRING";
  else
    result_ << value;

  if (type == "mrs_realvec" || type == "mrs_string")
    result_ << "'";

  result_ << "',";
}
void
marojson::begin_control_links_in(int num_links)
{
  if (num_links > 0)
    result_ << "'linksTo':[";
}
void
marojson::put_control_link_in(std::string abspath, std::string type, std::string name)
{
  result_ << "{";
  result_ << "'abspath':'" << abspath << "',";
  result_ << "'type':'" << type << "',";
  result_ << "'name':'" << name << "',";
  result_ << "},";
}

void
marojson::end_control_links_in(int num_links)
{
  if (num_links > 0)
    result_ << "],";
}

void
marojson::begin_control_links_out(int num_links)
{
  if (num_links > 0)
    result_ << "'linksFrom':[";
}
void
marojson::put_control_link_out(std::string abspath, std::string type, std::string name)
{
  result_ << "{";
  result_ << "'abspath':'" << abspath << "',";
  result_ << "'type':'" << type << "',";
  result_ << "'name':'" << name << "',";
  result_ << "},";
}
void
marojson::end_control_links_out(int num_links)
{
  if (num_links > 0)
    result_ << "],";
}

void
marojson::end_control(std::string type, std::string name, std::string value, bool has_state)
{
  (void) type;
  (void) name;
  (void) value;
  (void) has_state;
  result_ << "},";
}
void
marojson::end_controls(int num_links)
{
  if (num_links > 0)
    result_ << "],";
}

void
marojson::begin_children(int num_children)
{
  if (num_children > 0)
    result_ << "'components':[";
}

void
marojson::end_children(int num_children)
{
  if (num_children > 0)
    result_ << "],";
}

std::string
marojson::str()
{
  std::string json = result_.str();
  replace_all(json, ",}", "}");
  replace_all(json, ",]", "]");

  if (prettyPrint)
  {
    replace_all(json, ":", ": ");

//		smart_indent(json, "{[", "]}");

    replace_all(json, "{", "{\r\n");
    replace_all(json, "}", "\r\n}");
    replace_all(json, "[", "[\r\n");
    replace_all(json, "]", "\r\n]");
    replace_all(json, ",", ",\r\n");
  }

  return "{" + json + "}";
}

std::string&
marojson::replace_between(std::string& str,
                          const std::string &oldsubstr, const std::string &newsubstr,
                          std::string::size_type from_pos, std::string::size_type to_pos)
{
  std::string::size_type position;

  position = str.find(oldsubstr, from_pos);
  while (position != std::string::npos && position < to_pos)
  {
    str.replace(position, oldsubstr.size(), newsubstr);
    position = str.find(oldsubstr, position + newsubstr.size());
  }

  return str;
}

std::string&
marojson::replace_all(std::string& str, const std::string &oldsubstr, const std::string &newsubstr)
{
  return replace_between(str, oldsubstr, newsubstr, 0, str.size() - 1);
}

std::string&
marojson::smart_indent(std::string& str, const std::string &start, const std::string &end)
{
  (void) end;
  std::string::size_type position;

  position = str.find(start);
  while (position != std::string::npos)
  {
    str.replace(position, start.size(), start);
    position = str.find(start, position + start.size());
  }

  return str;
}
