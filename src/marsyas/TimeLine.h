/*
** Copyright (C) 1998-2012 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include <marsyas/FileName.h>
#include <marsyas/realvec.h>
#include "Color.h"

#include <string>
#include <algorithm>
#include "Communicator.h"

#include <vector>

namespace Marsyas
{
/**
\class TimeLine
\ingroup DataStruture
\brief TimeLine is a list of TimeRegions.

TimeLine is a list of TimeRegions. It represents a segmentation in
time of sound.  Each region is defined by its boundaries, a class id and
a name.
The units used to define the boundaries of each segment can be arbitrary
integer multiples of one sample and are defined by lineSize (e.g. if
lineSize is set to 1024, if the start of a region is set to 2, it means
that it starts at sample 2*1024 = 2048).

TimeLines can be saved and loaded from files. The current format is as follows:

Nr. of Regions (mrs_natural)
lineSize (in samples - mrs_natural)
size (in lineSize units - mrs_natural)
region1 start (mrs_natural)
region1 classid (mrs_natura)
region1 end (mrs_natural)
region1 name (mrs_string)
region2 start (mrs_natural)
region2 classid (mrs_natura)
region2 end (mrs_natural)
region2 name (mrs_string)
...
*/


struct marsyas_EXPORT TimeRegion
{
  mrs_natural start;
  mrs_natural classId;
  //Color color;
  mrs_natural end;
  mrs_string name;
};

class marsyas_EXPORT TimeLine
{
protected:
  mrs_string filename_;
  mrs_natural numRegions_;
  mrs_natural lineSize_;
  mrs_natural size_;
  mrs_real srate_;
  mrs_real psrate_;


  std::vector<TimeRegion> regions_;

public:
  TimeLine();
  ~TimeLine();

  void clear();
  void regular(mrs_natural spacing, mrs_natural size, mrs_natural lineSize = MRS_DEFAULT_SLICE_NSAMPLES);
  void segment(realvec segmentation, mrs_natural lineSize = MRS_DEFAULT_SLICE_NSAMPLES);

  mrs_string filename() const {return filename_;};
  mrs_natural size()const {return size_;};
  mrs_natural lineSize()const {return lineSize_;};

  mrs_natural numRegions() const {return numRegions_;};
  std::vector<mrs_string> getRegionNames() const;
  mrs_natural regionStart(mrs_natural regionNum) const;
  mrs_natural regionEnd(mrs_natural regionNum) const;
  mrs_string regionName(mrs_natural regionNum) const;
  mrs_real regionClass(mrs_natural regionNum) const;

  void setRegionName(mrs_natural regionNum, mrs_string name);
  void setRegionClass(mrs_natural regionNum, mrs_natural classId);
  mrs_bool setSampleRate(mrs_real srate);

  void removeRegion(mrs_natural regionNum);

  mrs_natural numClasses() const;
  //mrs_natural sampleRegion(mrs_natural sample) const;
  mrs_natural sampleClass(mrs_natural sample) const;

  void smooth(mrs_natural smoothSize);

  void info() const;
  bool load(mrs_string filename, mrs_string lexicon_labels);
  void write(mrs_string filename);
  friend std::ostream& operator<<(std::ostream&, const TimeLine&);

  void receive(Communicator* com);
  void send(Communicator* com);

  void print(FILE *fp);
  void printnew(FILE *fp);
  //void print_mp3(FILE *fp);
  //void print_mmf(FILE *fp);
  /* void color(FeatMatrix &map); */

};

}//namespace Marsyas



#endif








