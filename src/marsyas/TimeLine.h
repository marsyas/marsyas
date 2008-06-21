/*
** Copyright (C) 1998-2004 George Tzanetakis <gtzan@cs.uvic.ca>
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

#ifndef MARSYAS_TIMELINE_H
#define MARSYAS_TIMELINE_H

#include "realvec.h"
#include "Color.h"

#include <string>
#include "Communicator.h"

#include <vector>

namespace Marsyas
{
/** 
\class TimeLine 
	\ingroup none
\brief TimeLine is a list of TimeRegions. 

TimeLine is a list of TimeRegions. It represents a segmentation in
time of sound.  Each segment is defined by its boundaries and a
class id. A class_name table is used to derive the class names from
the class id's.  A simple float_vector representation is used for
the TimeLine where the class #::ids and the boundaries are
interleaved.  For example the following numbers:

0 1 100 0 258 3 640

mean from time 0 to 100 class is 1
from time 100 to 258 class is 0
etc.

The units used to define the boundaries of 
each segment can be arbitrary integer multiples
of one sample and are defined by lineSize.  

*/


struct TimeRegion
{
  mrs_natural start;
  mrs_natural classId;
  Color color;
  mrs_natural end;
  mrs_string name;
};

class TimeLine
{
protected:
  mrs_natural srate_;
  std::vector<mrs_string> classNames_;
  mrs_string filename_;
	std::vector<TimeRegion> regions_;

public:
	mrs_natural numRegions_; //FIXME
	mrs_natural lineSize_; //FIXME
	mrs_natural size_; //FIXME

  TimeLine();
  ~TimeLine();
  void scan(realvec segmentation);
  void regular(mrs_natural spacing, mrs_natural size);
  /* void color(FeatMatrix &map); */
  mrs_natural numRegions();
  mrs_natural start(mrs_natural regionNum);
  mrs_natural end(mrs_natural regionNum);
  std::string name(mrs_natural regionNum);
  void setName(mrs_natural regionNum, mrs_string name);
  void setClassId(mrs_natural regionNum, mrs_natural classId);
  mrs_natural getRClassId(mrs_natural regionNum);
  void remove(mrs_natural regionNum);
  
  void init(realvec values);
  bool load(mrs_string filename);
  void receive(Communicator* com);
  void send(Communicator* com);
  void info();

  mrs_natural getClassId(mrs_natural index);
	mrs_natural numClasses() const;
	std::vector<mrs_string> getClassNames() const;
 
  //void lengths_info();
  void print(FILE *fp);
  void printnew(FILE *fp);
  void smooth(mrs_natural smoothSize);
  
  void write(mrs_string filename);  
  friend std::ostream& operator<<(std::ostream&, const TimeLine&);  
  //void print_mp3(FILE *fp);
  //void print_mmf(FILE *fp);
  
};

}//namespace Marsyas



#endif



	



	
