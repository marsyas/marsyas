/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.uvic.ca>
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

/** 
    \class Annotator
    \brief Annotate feature stream with additional row of labels
    
*/



#include "Annotator.h"
using namespace std;



Annotator::Annotator(string name)
{
  type_ = "Annotator";
  name_ = name;
  addControls();
}


Annotator::~Annotator()
{
}


MarSystem* 
Annotator::clone() const 
{
  return new Annotator(*this);
}

void 
Annotator::addControls()
{
  addDefaultControls();
  addctrl("natural/label", 0);
  addctrl("string/labels","");
  setctrlState("string/labels",true);
  labels_str_ = "";
  labels_index_ = 0;  
}


void
Annotator::update()
{
  MRSDIAG("Annotator.cpp - Annotator:update");
  
  setctrl("natural/onSamples", getctrl("natural/inSamples"));
  setctrl("natural/onObservations", getctrl("natural/inObservations").toNatural()+1);
  setctrl("real/osrate", getctrl("real/israte"));

  setctrl("string/onObsNames", getctrl("string/inObsNames"));
  
  if( labels_str_.compare( getctrl("string/labels").toString() ) != 0 )
  {     
     labels_str_ = getctrl("string/labels").toString();
     labels_.clear();
     
     while( labels_str_.length() != 0 )
     {
        natural i = labels_str_.find(",");
        labels_.push_back( strtol( labels_str_.substr(0, i).c_str() , NULL , 10 ) );
        labels_str_ = labels_str_.substr( i+1 , labels_str_.length()-i-1 );
     }
     
     labels_index_ = 0;
  }
  
  
  defaultUpdate();
}


void 
Annotator::process(realvec& in, realvec& out)
{
  checkFlow(in,out);

  natural label = getctrl("natural/label").toNatural();  
  
  for (o=0; o < inObservations_; o++)
    for (t = 0; t < inSamples_; t++)
      {
	out(o,t) =  in(o,t);
      }

       
       for (t=0; t < inSamples_; t++) 
       {
          if( labels_.size() == 0 ){
             out(onObservations_-1, t) = (real)label;
          }
          else{
             out(onObservations_-1, t) = (real)labels_[labels_index_];
             labels_index_ = (labels_index_+1) % labels_.size();
             
          }
       }       
  
      
  
}







	
	
