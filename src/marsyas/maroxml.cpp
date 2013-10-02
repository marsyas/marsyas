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


#include <marsyas/maroxml.h>
#include <sstream>
#include <iostream>

using namespace std;
using namespace Marsyas;

maroxml::maroxml() : marostring()
{
}

maroxml::~maroxml()
{
  clear();
}

void
maroxml::begin_marsystem(bool isComposite, std::string type, std::string name)
{
  (void) isComposite;
  result_ << "<marsystem>" << endl;
  result_ << "  <type>" << type << "</type>" << endl;
  result_ << "  <name>" << name << "</name>" << endl;
}

void
maroxml::end_marsystem(bool isComposite, std::string type, std::string name)
{
  (void) isComposite;
  (void) type;
  (void) name;
  result_ << "</marsystem>" << endl;
}

void
maroxml::begin_controls(int num_controls)
{
  result_ << "  <controls count=\"" << num_controls << "\" >" << endl;
}

void
maroxml::begin_control(std::string type, std::string name, std::string value, bool has_state)
{
  result_ << "    <control>" << endl;
  result_ << "      <type>" << type << "</type>" << endl;
  result_ << "      <name>" << name << "</name>" << endl;
  result_ << "      <value type=\"" << type << "\">" << value << "</value>" << endl;
  result_ << "      <state>" << has_state << "</state>" << endl;
}

void
maroxml::begin_control_links_in(int num_links)
{
  if(num_links>0)
    result_ << "      <inlinks count=\"" << num_links << "\">" << endl;
}
void
maroxml::put_control_link_in(std::string abspath, std::string type, std::string name)
{
  result_ << "        <link path=\"" << abspath << "\" type=\"" << type << "\" name=\"" << name << "\" />" << endl;
}

void
maroxml::end_control_links_in(int num_links)
{
  if(num_links>0)
    result_ << "      </inlinks>" << endl;
}

void
maroxml::begin_control_links_out(int num_links)
{
  if(num_links>0)
    result_ << "      <outlinks count=\"" << num_links << "\">" << endl;
}
void
maroxml::put_control_link_out(std::string abspath, std::string type, std::string name)
{
  result_ << "        <link path=\"" << abspath << "\" type=\"" << type << "\" name=\"" << name << "\" />" << endl;
}
void
maroxml::end_control_links_out(int num_links)
{
  if(num_links>0)
    result_ << "      </outlinks>" << endl;
}

void
maroxml::end_control(std::string type, std::string name, std::string value, bool has_state)
{
  (void) type;
  (void) name;
  (void) value;
  (void) has_state;
  result_ << "    </control>" << endl;
}

void
maroxml::end_controls(int num_controls)
{
  (void) num_controls;
  result_ << "  </controls>" << endl;
}

void
maroxml::begin_children(int num_children)
{
  result_ << "  <children count=\"" << num_children << "\" >" << endl;
}

void
maroxml::end_children(int num_children)
{
  (void) num_children;
  result_ << "  </children>" << endl;
}

