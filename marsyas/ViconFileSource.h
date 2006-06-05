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
   \class ViconFileSource
   \brief ViconFileSource for exported VICON .txt files
   
   ViconFileSource reader for exported VICON .txt files.
*/


#ifndef MARSYAS_VICONFILESOURCE_H
#define MARSYAS_VICONFILESOURCE_H


#include "MarSystem.h" 




class ViconFileSource: public MarSystem 
{
private:

  std::string tfname_;
  std::string filename_;
  
  FILE *vfp_;
  long vfp_begin_;
    
  natural size_;  
  natural fileObs_;
  void addControls();
      
public:
  ViconFileSource();
  ViconFileSource(std::string name);
  ~ViconFileSource();
  MarSystem* clone() const;  
  
  void update();
  void getHeader(std::string filename);
  void process(realvec& in, realvec &out);
};


#endif     /* !MARSYAS_VICONFILESOURCE_H */ 

	
