/*
** Copyright (C) 1998-2011 George Tzanetakis <gtzan@cs.uvic.ca>
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


#include <marsyas/marosvg.h>
#include <sstream>
#include <iostream>

using namespace std;
using namespace Marsyas;

marosvg::SVGObj_::SVGObj_(SVGObj_* p, std::string t, std::string n)
{
  parent_=p; t=t; n_=n;
  if(parent_!=NULL) {
    parent_->addChild(this);
  }
}
marosvg::SVGObj_::~SVGObj_()
{
  while(children_.size()>0) {
    SVGObj_* c = children_.back();
    children_.pop_back();
    delete c;
  }
}
void
marosvg::SVGObj_::addChild(SVGObj_* c)
{
  if(c!=NULL)
    children_.push_back(c);
}

void
marosvg::SVGObj_::sizeAdj()
{
  if (children_.size()>0) {
    if(t=="Series") {
      int max_height=0;
      int w=0;

      for(int i=0; i < (int)children_.size(); ++i) {
        SVGObj_* s=children_[i];
        s->sizeAdj();

        if (s->h_>max_height)
          max_height = s->h_;
        w = w + 20 + s->w_;
      }
      h_ = max_height + 20 + 20;
      w_ = w + 20;
    }
    else if (t=="Fanout" || t=="Parallel") {
      int max_width=0;
      int h=0;

      for(int i=0; i < (int)children_.size(); ++i) {
        SVGObj_* s=children_[i];
        s->sizeAdj();

        h = h + 20 + s->h_;
        if (s->w_ > max_width)
          max_width = s->w_;
      }
      h_ = h + 20;
      w_ = max_width + 20 + 20;
    }
  }
  else {
    h_=40;
    w_=100;
  }
}

void
marosvg::SVGObj_::posAdj(int x, int y)
{
  x_=x; y_=y;
  int midY = (h_ >> 1) + y;
  x+=20;
  if (children_.size()>0) {
    if (t=="Series") {
      for(int i=0; i < (int)children_.size(); ++i) {
        SVGObj_* s=children_[i];
        int sy = midY - (s->h_ >> 1);
        s->posAdj(x,sy);
        x = x + s->w_ + 20;
      }
    }
    else if (t=="Fanout" || t=="Parallel") {
      y += 20;
      for(int i=0; i < (int)children_.size(); ++i) {
        SVGObj_* s=children_[i];
        s->posAdj(x,y);
        y = y + s->h_ + 20;
      }
    }
  }
}

std::string
marosvg::SVGObj_::str()
{
  bool fanout=(t=="Fanout");
  bool parallel=(t=="Parallel");
  bool series=(t=="Series");
  std::ostringstream op;
  op << "<rect ";
  if (fanout) { op << "class=\"fanout\" "; }
  else if (parallel) { op << "class=\"parallel\" "; }
  else if (series) { op << "class=\"series\" "; }
  op << "x=\"" << x_
     << "\" y=\"" << y_
     << "\" width=\"" << w_
     << "\" height=\"" << h_
     << "\" />\n";//style=\"fill:rgb(230,230,255);stroke-width:1; stroke:rgb(0,0,0)\"/>\n";

  op << "<text class=\"marsysid\" x=\"" << (x_+5) << "\" y=\"" << (y_+15) << "\">" << t << "/" << n_ << "</text>\n";

  int my = y_ + (h_ >> 1);
  if (series) {
    op << "<line class=\"wire\" x1=\"" << x_
       << "\" y1=\"" << my
       << "\" x2=\"" << (x_+w_)
       << "\" y2=\"" << my
       << "\" />\n";
  }

  for(int i=0; i < (int)children_.size(); ++i) {
    SVGObj_* c = children_[i];
    int cmy = c->y_ + (c->h_ >> 1);
    if(fanout) {
      op << "<line class=\"wire\" x1=\"" << x_
         << "\" y1=\"" << my
         << "\" x2=\"" << c->x_
         << "\" y2=\"" << cmy
         << "\" />";

      op << "<line class=\"wire\" x1=\"" << c->x_
         << "\" y1=\"" << cmy
         << "\" x2=\"" << (x_+w_)
         << "\" y2=\"" << cmy
         << "\" />\n";
    }
    else if (parallel) {
      op << "<line class=\"wire\" x1=\"" << x_
         << "\" y1=\"" << cmy
         << "\" x2=\"" << (x_+w_)
         << "\" y2=\"" << cmy
         << "\" />\n";
    }
    op << c->str();
  }
  return op.str();
}

marosvg::marosvg() : marostring()
{
  curr_=NULL;
  load_properties();
}

marosvg::~marosvg()
{
  clear();
}

void
marosvg::begin_marsystem(bool isComposite, std::string type, std::string name)
{
  (void)isComposite;  // avoid unused parameter warning

  // make this node the root of the tree for recursive marsystem creation
  curr_ = new SVGObj_(curr_,type,name);
}

void
marosvg::end_marsystem(bool isComposite, std::string type, std::string name)
{
  (void)isComposite;  // avoid warnings about unused parameters
  (void)type;
  (void)name;

  //"<rect x=\"50\" y=\"50\" width=\"300\" height=\"100\" style=\"fill:rgb(0,0,255);stroke-width:1; stroke:rgb(0,0,0)\"/>"
  // return parent to the root of the tree
  if(curr_!=NULL && curr_->parent_!=NULL)
    curr_=curr_->parent_;
}

void
marosvg::begin_control(std::string type, std::string name, std::string value, bool has_state)
{
  (void)type; // avoid unused parameter warning
  (void)name;
  (void)value;
  (void)has_state;

}

void
marosvg::begin_controls(int num_controls)
{
  (void)num_controls;  // avoid unused parameter warning

}

void
marosvg::begin_children(int num_children)
{
  (void)num_children;  // avoid unused parameter warning
}

void
marosvg::load_properties()
{
  style_["marsystem"]["fill"] = "rgb(196,196,196)";
  style_["marsystem"]["stroke"] = "black";
  style_["marsystem"]["stroke-width"] = "1";

  style_["series"]["fill"] = "rgb(255,196,196)";

  style_["parallel"]["fill"] = "rgb(196,255,196)";

  style_["fanout"]["fill"] = "rgb(196,196,255)";

  style_["wire"]["stroke-width"] = "1";
  style_["wire"]["stroke"] = "black";

  style_["marsysid"]["stroke"] = "black";
}

void
marosvg::style(std::string obj, std::string property, std::string value)
{
  style_[obj][property] = value;
}

void
marosvg::output_properties(std::string property)
{
  std::map<std::string,std::string> prop = style_[property];
  std::map<std::string,std::string>::iterator iter;

  for (iter = prop.begin(); iter != prop.end(); ++iter ) {
    result_ << iter->first << ':' << iter->second << ';';
  }
}
std::string
marosvg::str()
{
  if (curr_!=NULL) {
    curr_->sizeAdj();
    curr_->posAdj(0,0);

    int ww = curr_->w_;
    int hh = curr_->h_;
    std::map<std::string,std::string>::iterator iter;

    result_ << "<?xml version=\"1.0\"?>\n"
            << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n"
            << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\"  width=\""<<ww<<"\" height=\""<<hh<<"\">\n"

            << "<style>\n";

    result_ << "  line.wire{"; output_properties("wire"); result_ << "}\n";

    result_ << "  rect{"; output_properties("marsystem"); result_ << "}\n";

    result_ << "  rect.series{"; output_properties("series"); result_ << "}\n";

    result_ << "  rect.parallel{"; output_properties("parallel"); result_ << "}\n";

    result_ << "  rect.fanout{"; output_properties("fanout"); result_ << "}\n";

    result_ << "  text.marsysid{"; output_properties("marsysid"); result_ << "}\n";
    result_ << "</style>\n";

    result_ << curr_->str();
    result_ << "\n</svg>\n";
    return result_.str();
  }
  return "";
}

void
marosvg::clear()
{
  marostring::clear();
  if (curr_!=NULL) {
    delete curr_;
    curr_=NULL;
  }
}
