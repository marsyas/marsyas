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


#include <marsyas/marostring.h>

using namespace std;
using namespace Marsyas;

marostring::marostring()
{
  clear();
}

marostring::marostring(const marostring& m)
{
  (void) m;
  //result_ << m.result_;
}

marostring::~marostring()
{
}

void
marostring::end_marsystem(bool isComposite, std::string type, std::string name)
{
  (void) isComposite;
  (void) type;
  (void) name;
}

void
marostring::begin_control_links_in(int num_links)
{
  (void) num_links;
}

void
marostring::put_control_link_in(std::string abspath, std::string type, std::string name)
{
  (void) abspath;
  (void) type;
  (void) name;
}

void
marostring::end_control_links_in(int num_links)
{
  (void) num_links;
}

void
marostring::begin_control_links_out(int num_links)
{
  (void) num_links;
}

void
marostring::put_control_link_out(std::string abspath, std::string type, std::string name)
{
  (void) abspath;
  (void) type;
  (void) name;
}

void
marostring::end_control_links_out(int num_links)
{
  (void) num_links;
}

void
marostring::end_control(std::string type, std::string name, std::string value, bool has_state)
{
  (void) type;
  (void) name;
  (void) value;
  (void) has_state;
}

void
marostring::end_controls(int num_controls)
{
  (void) num_controls;
}

void
marostring::end_children(int num_children)
{
  (void) num_children;
}

std::string
marostring::str()
{
  return result_.str();
}

void
marostring::clear()
{
  result_.clear();
}
