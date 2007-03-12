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
   \class WekaSink
   \brief Output sink (text) in Weka format

*/

#ifndef MARSYAS_WEKASINK_H
#define MARSYAS_WEKASINK_H

#include "MarSystem.h"

#include <fstream>
#include <sstream>
#include <iomanip>

namespace Marsyas
{

class WekaSink: public MarSystem
{
private:
  void addControls();
  void myUpdate(MarControlPtr sender);
  
  std::string filename_;
  std::ofstream* mos_;

  std::vector<std::string> labelNames_;
  mrs_natural precision_;
  mrs_natural downsample_;
  
  MarControlPtr ctrl_regression_;
  MarControlPtr ctrl_putHeader_;
  MarControlPtr ctrl_precision_;
  MarControlPtr ctrl_downsample_;
  MarControlPtr ctrl_nLabels_;
  MarControlPtr ctrl_labelNames_;
  
 
public:
  WekaSink(std::string name);
  WekaSink(const WekaSink& a);
  
  ~WekaSink();
 
  MarSystem* clone() const;  
  void putHeader(std::string inObsNames);
 
  void myProcess(realvec& in, realvec& out);

  // Jen's hack for MIREX 05 to annotate produced weka file
  std::ofstream* getOfstream(){ return mos_; }
     
};

}//namespace Marsyas


#endif

	

	

	

	

	
