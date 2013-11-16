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

#include <marsyas/TimeLine.h>
#include <marsyas/common_source.h>
#include <cstddef>

using namespace std;
using namespace Marsyas;

TimeLine::TimeLine()
{
  srate_ = 22050.0;
  psrate_ = 0.0;
  lineSize_ = 0;
  size_ = 0;
  filename_ = "";
  numRegions_ = 0;
}

TimeLine::~TimeLine()
{
}

void
TimeLine::clear()
{
  filename_ = "";
  srate_ = 22050.0;
  psrate_ = 0.0;

  lineSize_ = 0;
  size_ = 0;
  regions_.clear();
  numRegions_ = 0;
}

mrs_bool
TimeLine::setSampleRate(mrs_real srate)
{
  srate_ = srate;

  if ((srate_ != 22050.0)&&(srate_ != psrate_)) // not the default
  {
    // readjust
    for (mrs_natural i=0; i < numRegions_; ++i)
    {
      regions_[i].start = (mrs_natural) (regions_[i].start * (srate_ / 22050.0));
      regions_[i].end   = (mrs_natural) (regions_[i].end * (srate_ / 22050.0));
    }
    psrate_ = srate;
    return true;
  } else {
    return false;
  }

}


void
TimeLine::regular(mrs_natural spacing, mrs_natural size, mrs_natural lineSize)
{
  if (size_ != 0)
  {
    MRSERR("TimeLine::regular() - TimeLine has data already!");
    return;
  }

  size_ = size;
  mrs_natural reg_index = 0;
  mrs_natural i;
  lineSize_ = lineSize;
  if ((size_ % spacing) != 0)
    numRegions_ = (size_ / spacing) + 1;
  else
    numRegions_ = (size_ / spacing);

  for (i=0; i < numRegions_; ++i)
  {
    TimeRegion region;
    regions_.push_back(region);
  }

  for (i=0; i<size_; ++i)
  {
    if ((i % spacing) == 0)
    {
      if (reg_index > 0)
        regions_[reg_index-1].end = i-1;
      regions_[reg_index].start = i;
      regions_[reg_index].classId = 0;
      reg_index++;
    }
  }
  regions_[numRegions_-1].end = size_;
  regions_[reg_index-1].end = size_;
}

void
TimeLine::segment(realvec segmentation, mrs_natural lineSize)
{
  mrs_natural i;
  mrs_natural peakCount=0;

  if (size_ != 0)
  {
    MRSERR("TimeLine::scan() - TimeLine has data already!");
    return;
  }

  size_ = segmentation.getSize();
  for (i=0; i<size_; ++i)
  {
    if (segmentation(i) == 1)
      peakCount++;
  }

  numRegions_ = peakCount-1; //[?]
  lineSize_ = lineSize;

  for (i=0; i < numRegions_; ++i)
  {
    TimeRegion region;
    regions_.push_back(region);
  }

  mrs_natural reg_index = 0;
  for (i=0; i<size_; ++i)
  {
    if (segmentation(i) == 1) //[?]
    {
      if (reg_index > 0)
        regions_[reg_index-1].end = i;
      if (reg_index == peakCount -1)
        break;
      regions_[reg_index].start = i;
      regions_[reg_index].classId = 0;
      reg_index++;
    }
  }
}

mrs_natural
TimeLine::numClasses() const
{
  vector<mrs_natural> classes;
  bool found = false;

  for(mrs_natural i = 0; i < numRegions_; ++i)
  {
    found = false;
    //check if this region's class Id was already counted
    for(mrs_natural c = 0; c < (mrs_natural)classes.size(); ++c)
    {
      if(classes[c] == regions_[i].classId)
      {
        found = true;
        break;
      }
    }
    //if this is a new classId, count it
    if(!found)
      classes.push_back(regions_[i].classId);
  }

  return (mrs_natural)classes.size();
}

vector<mrs_string>
TimeLine::getRegionNames() const
{
  vector<mrs_string> classNames;
  bool found = false;

  for(mrs_natural i = 0; i < numRegions_; ++i)
  {
    found = false;
    //check if this region's class name was already considered
    for(mrs_natural c = 0; c < (mrs_natural)classNames.size(); ++c)
    {
      if(classNames[c] == regions_[i].name)
      {
        found = true;
        break;
      }
    }
    //if this is a new className, store it
    if(!found)
      classNames.push_back(regions_[i].name);
  }

  sort(classNames.begin(), classNames.end());


  return classNames;
}

mrs_natural
TimeLine::regionStart(mrs_natural regionNum) const
{
  if (regionNum < numRegions_)
    return regions_[regionNum].start;
  else return -1;
}

mrs_string
TimeLine::regionName(mrs_natural regionNum) const
{
  if (regionNum < numRegions_)
    return regions_[regionNum].name;
  else return "";
}

void
TimeLine::setRegionName(mrs_natural regionNum, mrs_string name)
{
  if (regionNum < numRegions_)
    regions_[regionNum].name = name;
}

void
TimeLine::setRegionClass(mrs_natural regionNum, mrs_natural classId)
{
  if (regionNum < numRegions_)
  {
    regions_[regionNum].classId = classId;
  }
}

mrs_natural
TimeLine::regionEnd(mrs_natural regionNum) const
{
  if (regionNum < numRegions_)
    return regions_[regionNum].end;
  else return -1;
}

void
TimeLine::smooth(mrs_natural smoothSize) //[?]
{
  TimeRegion region;
  TimeRegion pregion;
  TimeRegion nregion;

  for (int i=1; i < numRegions_-1; ++i)
  {
    region = regions_[i];
    pregion = regions_[i-1];
    nregion = regions_[i+1];

    if ((region.end - region.start < smoothSize) && (region.classId == 1))
    {
      // if ((pregion.end - pregion.start) > (nregion.end - nregion.start))
      // {
      removeRegion(i);
      i = i-1;
      // }
      // else
      // remove(i+1);
    }
  }

  for (mrs_natural i=1; i < numRegions_; ++i)
  {
    region = regions_[i];
    pregion = regions_[i-1];

    if (region.classId == pregion.classId)
    {
      removeRegion(i);
      i = i-1;
    }
  }
}

void
TimeLine::removeRegion(mrs_natural regionNum)
{
  if (regionNum >= 1)
  {
    regions_[regionNum-1].end = regions_[regionNum].end;
    regions_.erase(regions_.begin() + regionNum);
    numRegions_--;
  }
}

mrs_real
TimeLine::regionClass(mrs_natural regionNum) const
{


  if (regionNum < numRegions_)
    return regions_[regionNum].classId;
  else
    return 0;
}

mrs_natural
TimeLine::sampleClass(mrs_natural index) const
{
  TimeRegion region;
  for (mrs_natural i=0; i < numRegions_; ++i)
  {
    region = regions_[i];
    if ((region.start <= index) && (index < region.end))
    {
      return region.classId;
    }
  }
  return 0;
}

bool
TimeLine::load(mrs_string filename, mrs_string lexicon_labels)
{

  ifstream in;
  filename_ = filename;

  if(filename == "")
    return false;

  in.open(filename.c_str());
  if(!in.is_open())
  {
    MRSWARN("TimeLine::load() -  Problem opening file " << filename_);
    return false;
  }

  FileName f(filename);
  vector<mrs_string> labels;



  // Load lexicon dictionary if available
  mrs_string lexicon_label;
  mrs_string remainder;
  size_t nLabels;

  nLabels = std::count(lexicon_labels.begin(), lexicon_labels.end(), ',');

  if (lexicon_labels != ",")
  {
    for (size_t i=0; i < nLabels; i++)
    {
      lexicon_label = lexicon_labels.substr(0, lexicon_labels.find(","));
      labels.push_back(lexicon_label);
      sort(labels.begin(), labels.end());
      remainder = lexicon_labels.substr(lexicon_labels.find(",") + 1, lexicon_labels.length());
      lexicon_labels = remainder;
    }
  }
  else
    nLabels = 0;


  if (f.ext() == "txt") // audacity label format
  {
    numRegions_ = 0;
    mrs_real start, end;
    mrs_string label;
    regions_.clear();
    while (!in.eof())
    {
      in >> start >> end >> label;


      TimeRegion region;
      region.start = (mrs_natural) (start * srate_);
      region.end = (mrs_natural) (end * srate_);
      region.classId = 1;
      region.name = label;
      mrs_bool label_found = false;

      for (unsigned int i=0; i < labels.size(); i++)
      {
        if (label == labels[i])
        {
          label_found = true;
          region.classId = i;
        }

      }
      if (!label_found)
      {

        if (lexicon_labels == ",")
        {
          labels.push_back(label);
          sort(labels.begin(), labels.end());
        }
      }
      regions_.push_back(region);
      numRegions_ ++;
    }




    // relabel classIds so that they correspond to sorted labels
    for (mrs_natural i=0; i < numRegions_; ++i)
    {
      mrs_string label = regions_[i].name;
      vector<mrs_string>::iterator it = find(labels.begin(), labels.end(), label);
      if (it == labels.end())
        regions_[i].classId = (mrs_natural)-1;
      mrs_natural l = distance(labels.begin(), it);
      regions_[i].classId = l;
    }


    // last region is a duplicate due to empty last line
    // kind of a hack but works
    numRegions_ --;
    regions_.pop_back();


    lineSize_ = 1;
    size_ = (mrs_natural) (end * srate_);

    in.close();
    return true;
  }
  else     // marsyas .mtl format
  {
    in >> numRegions_; // read numRegions
    MRSDIAG("TimeLine::load() - Number of regions is " << numRegions_);

    in >> lineSize_; //read lineSize
    MRSDIAG("TimeLine::load() - lineSize size is " << lineSize_);

    in >> size_; //read size
    MRSDIAG("TimeLine::load() - Size is " << size_);

    regions_.clear();
    for (mrs_natural i=0; i < numRegions_; ++i)
    {
      TimeRegion region;
      regions_.push_back(region);
    }

    for (mrs_natural i=0; i<numRegions_; ++i)
    {
      mrs_natural token;
      mrs_string stoken1, stoken2;
      in >> token;
      regions_[i].start = token;
      in >> token;
      regions_[i].classId = token;
      in >> token;
      regions_[i].end = token;
      in >> stoken1;// >> stoken2; //used for .cue files in IEEE TASLP paper...
      //regions_[i].name = stoken1 +" "+stoken2; //used for .cue files in IEEE TASLP paper...
      regions_[i].name = stoken1;
    }

    in.close();

    return true;

  }

}

void
TimeLine::info() const
{
  mrs_natural i;
  MRSMSG("Number of regions = " << numRegions_ << endl);
  MRSMSG("Line size  = " << lineSize_ << endl);
  MRSMSG("TimeLine size (# line size blocks ) = " << size_ << endl);

  for (i=0; i < numRegions_; ++i)
  {
    MRSMSG("--------------------------------------------" << endl);
    MRSMSG("Region " << i << " start    = " << regions_[i].start << endl);
    MRSMSG("Region " << i << " class id = " << regions_[i].classId << endl);
    MRSMSG("Region " << i << " name     = " << regions_[i].name << endl);
    MRSMSG("Region " << i << " end      = " << regions_[i].end   << endl);
  }
}

void
TimeLine::printnew(FILE *fp)
{
  mrs_natural i;
  fprintf(fp, "%d\n", (int)numRegions_);
  fprintf(fp, "%d\n", (int)lineSize_);
  fprintf(fp, "%d\n", (int)size_);

  for (i=0; i<numRegions_; ++i)
  {
    // convert to milliseconds
    float smsec;
    float emsec;
    smsec = (float) ((regions_[i].start * lineSize_ * 1000) / srate_);
    // fprintf(fp, "%d ", regions_[i].start);
    fprintf(fp, "%6.0f ", smsec);

    fprintf(fp, "%d ", (int)regions_[i].classId);
    emsec = (float) ((regions_[i].end * lineSize_ * 1000) / srate_);
    // fprintf(fp, "%d ", regions_[i].end);
    fprintf(fp, "%6.0f\n", emsec);

    fprintf(fp, "%s\n", regions_[i].name.c_str());
  }
}

void
TimeLine::write(mrs_string filename)
{
  ofstream os(filename.c_str());
  os << (*this) << endl;
}

namespace Marsyas {
ostream&
operator<<(ostream& o, const TimeLine& tline)
{
  o << tline.numRegions_ << endl;
  o << tline.lineSize_ << endl;
  o << tline.size_ << endl;

  for (mrs_natural i=0; i<tline.numRegions_; ++i)
  {
    o << tline.regions_[i].start ;
    o << " " << tline.regions_[i].classId;
    o << " " << tline.regions_[i].end << endl;
    o << "Region " << i+1 << endl;
  }
  return o;
}
}

void
TimeLine::print(FILE *fp)
{
  mrs_natural i;
  fprintf(fp, "%d\n", (int) numRegions_);
  fprintf(fp, "%d\n", (int)lineSize_);
  fprintf(fp, "%d\n", (int)size_);

  for (i=0; i<numRegions_; ++i)
  {
    fprintf(fp, "%d ", (int)regions_[i].start);
    fprintf(fp, "%d ", (int)regions_[i].classId);
    fprintf(fp, "%d\n", (int)regions_[i].end);
    fprintf(fp, "Region %d\n", (int)i+1);
  }
}

/*
void
TimeLine::print_mp3(FILE *fp)
{
verbose("TimeLine::print(FILE *fp)");
int i;
float ratio = 576.0 / 512.0;

fprintf(fp, "%d\n", num_of_regions_);
fprintf(fp, "%d\n", skip_size_);
fprintf(fp, "%d\n", (int)(size_ * ratio));

for (i=0; i<num_of_regions_; ++i)
{
fprintf(fp, "%d ", (int)(region_[i].start *ratio));
fprintf(fp, "%d ", region_[i].class_id);
fprintf(fp, "%d\n", (int)(region_[i].end *ratio));
fprintf(fp, "Region %d\n", i+1);
}

}
*/

/*
void
TimeLine::print_mmf(FILE *fp)
{
int i;
float ratio = 576.0 / 512.0;

fprintf(fp,"# Feature map information file written by MARSYAS\n");
fprintf(fp,"Segment\n");
fprintf(fp,"#Number of features and number of different classes\n");
fprintf(fp,"%d %d\n", num_of_regions_, 0);
fprintf(fp, "#Names of features\n");
for (i=0; i<num_of_regions_; ++i)
fprintf(fp,"f%d ", i);
fprintf(fp,"\n\n");
fprintf(fp,"#Name of classes\n");
fprintf(fp,"\n\n");

fprintf(fp,"#Number of feature float_vectors \n");
fprintf(fp,"1\n");
fprintf(fp,"#Feature map data\n");
fprintf(fp,"\n");


for (i=0; i<num_of_regions_; ++i)
{
fprintf(fp, "%f ", region_[i].end * ratio - region_[i].start *ratio );
}
fprintf(fp," 0\n\n");
}
*/

void
TimeLine::receive(Communicator* com)
{
  static char *buf = new char[256];
  mrs_natural i;
  mrs_string message;

  com->receive_message(buf);
  // message = buf;
  numRegions_ = atoi(buf);

  com->receive_message(buf);

  lineSize_ = atoi(buf);

  com->receive_message(buf);
  size_ = atoi(buf);

  for (i=0; i < numRegions_; ++i)
  {
    com->receive_message(buf);
  }
}

void
TimeLine::send(Communicator* com)
{
  static char *buf = new char[256];
  mrs_natural i;
  mrs_string message;

  sprintf(buf, "%d\n", (int)numRegions_);
  message = buf;
  com->send_message(message);

  sprintf(buf, "%d\n", (int)lineSize_);
  message = buf;
  com->send_message(message);

  sprintf(buf, "%d\n", (int)size_);
  message = buf;
  com->send_message(message);

  for (i=0; i<numRegions_; ++i)
  {
    sprintf(buf, "%d ", (int)regions_[i].start);
    message = buf;
    com->send_message(message);

    sprintf(buf, "%d ", (int)regions_[i].classId);
    message = buf;
    com->send_message(message);

    /*sprintf(buf, "%f ", region_[i].color.getR());
    message = buf;
    com->send_message(message);

    sprintf(buf, "%f ", region_[i].color.getG());
    message = buf;
    com->send_message(message);

    sprintf(buf, "%f ", region_[i].color.getB());
    message = buf;
    com->send_message(message);

    */

    sprintf(buf, "%d\n", (int)regions_[i].end);
    message = buf;
    com->send_message(message);

    sprintf(buf, "Region %d\n", (int)i);
    message = buf;
    com->send_message(message);
  }
}

/*
void
TimeLine::color(Feature_map &map)
{
int i;
float_vector f1 = map.get_feat(9);
float_vector f2 = map.get_feat(0);
float_vector f3 = map.get_feat(3);
float r;
float g;
float b;

Gen_Plotter plotter;
//f1.norm(0.5, 0.2);
//f2.norm(0.5, 0.2);
//f3.norm(0.5, 0.2);

f1.norm(0.00074, 0.0017, 0.5, 0.2);
f2.norm(40.5, 20.5, 0.5, 0.2);
f3.norm(5.63, 2.71, 0.5, 0.2);

//plotter.plot_wait(f1);
//plotter.plot_wait(f2);
//plotter.plot_wait(f3);

int start;
int end;
for (i=0; i<num_of_regions_; ++i)
{
start = region_[i].start;
end = region_[i].end;
r = fabs(f1.mean(start,end));
g = fabs(f2.mean(start,end));
b = fabs(f3.mean(start,end));

//r = 0.5;
//g = 0.5;
//b = 0.5;

// Clip to colors
if (r > 1.0) r = 1.0;
if (g > 1.0) g = 1.0;
if (b > 1.0) b = 1.0;
if (r < 0.0) r = 0.0;
if (g < 0.0) g = 0.0;
if (b < 0.0) b = 0.0;

region_[i].color.setRGB(r,g,b);
}

}
*/
