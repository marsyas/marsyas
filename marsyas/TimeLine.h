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



/** 
    \class TimeLine 
    \brief TimeLine is a list of TimeRegions. 
    
   TimeLine is a list of TimeRegions. It represents a segmentation in
   time of sound.  Each segment is defined by its boundaries and a
   class id. A class_name table is used to derive the class names from
   the class id's.  A simple float_vector representation is used for
   the TimeLine where the class #ids and the boundaries are
   interleaved.  For example the following numbers:

   0 1 100 0 258 3 640
   
   mean from time 0 to 100 class is 1
   from time 100 to 258 class is 0
   etc.
   
   The units used to define the boundaries of 
   each segment can be arbitrary integer multiples
   of one sample and are defined by lineSize.  

*/



#ifndef MARSYAS_TIMELINE_H
#define MARSYAS_TIMELINE_H

#include "realvec.h"
#include "Color.h"


#include <string>
#include "Communicator.h"

#include <vector>


struct TimeRegion
{
  unsigned int start;
  unsigned int classId;
  Color color;
  unsigned int end;
  std::string name;
};


#define SEPCHARS " \t\n"

class TimeLine
{
protected:

  int srate_;
  std::vector<std::string> classNames_;

  std::string filename_;
public:
  std::vector<TimeRegion> regions_;
  int numRegions_;
  int lineSize_;

  int size_;
  TimeLine();
  ~TimeLine();
  void scan(realvec segmentation);
  void regular(unsigned int spacing, unsigned int size);
  /* void color(FeatMatrix &map); */
  int numRegions();
  int start(int regionNum);
  int end(int regionNum);
  std::string name(int regionNum);
  void setName(int regionNum, std::string name);
  void setClassId(int regionNum, int classId);
  int getRClassId(int regionNum);
  void remove(int regionNum);
  
  void init(realvec values);
  void load(std::string filename);
  void receive(Communicator* com);
  void send(Communicator* com);
  void info();

  unsigned int getClassId(unsigned int index);
  

  //void lengths_info();
  void print(FILE *fp);
  void printnew(FILE *fp);
  void smooth(unsigned int smoothSize);
  
  void write(std::string filename);  
  friend std::ostream& operator<<(std::ostream&, const TimeLine&);  
  //void print_mp3(FILE *fp);
  //void print_mmf(FILE *fp);
  
};



#endif



	



	
