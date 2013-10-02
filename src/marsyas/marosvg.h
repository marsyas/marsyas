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

#ifndef MARSYAS_OUTPUT_SVG_H
#define MARSYAS_OUTPUT_SVG_H

#include <string>
#include <vector>
#include <map>
#include <marsyas/marostring.h>


namespace Marsyas
{
/**
	\class marosvg
	\ingroup Notmar
	\brief generate svg representing a MarSystem
	\author inb@cs.uvic.ca
*/

class marsyas_EXPORT marosvg : public marostring
{
private:
  std::map<std::string, std::map<std::string,std::string> > style_;
  class Style_
  {
    Style_();
  };
  class SVGObj_
  {
  public:
    int x_, y_, w_, h_;
    std::string t, n_;
    SVGObj_* parent_;
    std::vector<SVGObj_*> children_;

    SVGObj_(SVGObj_* p, std::string t, std::string n);
    virtual ~SVGObj_();

    void addChild(SVGObj_* c);

    void posAdj(int x, int y);
    void sizeAdj();
    std::string str();
  };
  SVGObj_* curr_;

  void output_properties(std::string property);
  void load_properties();

public:
  marosvg();
  virtual ~marosvg();

  virtual void begin_marsystem(bool isComposite, std::string type, std::string name);
  virtual void end_marsystem(bool isComposite, std::string type, std::string name);
  virtual void begin_control(std::string type, std::string name, std::string value, bool has_state);
  virtual void begin_controls(int num_controls);
  virtual void begin_children(int num_children);

  void style(std::string obj, std::string property, std::string value);

  virtual std::string str();
  virtual void clear();
};

}//namespace Marsyas

#endif
