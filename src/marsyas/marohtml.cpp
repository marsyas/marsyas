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


#include <marsyas/marohtml.h>
#include <sstream>
#include <iostream>

using namespace std;
using namespace Marsyas;

marohtml::marohtml() : marostring()
{
}

marohtml::~marohtml()
{
  clear();
}

void
marohtml::begin_marsystem(bool isComposite, std::string type, std::string name)
{
  if(isComposite)
    result_ << "<li>MarSystemComposite" << endl;
  else
    result_ << "<li>MarSystem" << endl;

  result_ << "Type = " << type << endl;
  result_ << "Name = " << name << endl;

  result_ << endl;
}

void
marohtml::end_marsystem(bool isComposite, std::string type, std::string name)
{
  (void) isComposite;
  (void) type;
  (void) name;
  result_ << "</li>" << endl;
}

void
marohtml::begin_controls(int num_controls)
{
  result_ << "<li>MarControls = " << num_controls << endl;
  result_ << "<ul>" << endl;
}

void
marohtml::begin_control(std::string type, std::string name, std::string value, bool has_state)
{
  (void) has_state;
  if (value=="")
    result_ << "<li>" << type << "/" << name << " = " << "MARSYAS_EMPTYSTRING" << "</li>" << endl;
  else
    result_ << "<li>" << type << "/" << name << " = " << value << "</li>" << endl;
}
void
marohtml::begin_control_links_in(int num_links)
{
  result_ << "<li>LinksTo = " << num_links << endl;
  result_ << "<ul>";
}
void
marohtml::put_control_link_in(std::string abspath, std::string type, std::string name)
{
  result_ << "<li>" << abspath << "//" << type << "/" << name << "</li>" << endl;
}

void
marohtml::end_control_links_in(int num_links)
{
  (void) num_links;
  result_ << "</ul></li>" << endl;
}

void
marohtml::begin_control_links_out(int num_links)
{
  result_ << "<li>LinksFrom = " << num_links << endl;
  result_ << "<ul>";
}
void
marohtml::put_control_link_out(std::string abspath, std::string type, std::string name)
{
  result_ << "<li>" << abspath << "//" << type << "/" << name << "</li>" << endl;
}
void
marohtml::end_control_links_out(int num_links)
{
  (void) num_links;
  result_ << "</ul></li>" << endl;
}

void
marohtml::end_control(std::string type, std::string name, std::string value, bool has_state)
{
  (void) type;
  (void) name;
  (void) value;
  (void) has_state;
}
void
marohtml::end_controls(int num_links)
{
  (void) num_links;
  result_ << "</ul>" << endl;
  result_ << "</li>" << endl;
}

void
marohtml::begin_children(int num_children)
{
  result_ << "<li>Components = " << num_children << endl;
  result_ << "<ul>" << endl;
}

void
marohtml::end_children(int num_children)
{
  (void) num_children;
  result_ << "</ul>" << endl;
  result_ << "</li>" << endl;
}

std::string
marohtml::str()
{
  return "<html><head></head><body>" + result_.str() + "</body></html>";
}
