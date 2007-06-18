/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.cmu.edu>
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
	\ingroup none
   \brief ViconFileSource for exported VICON .txt files
   
   ViconFileSource reader for exported VICON .txt files. 
*/

#include "ViconFileSource.h"

using namespace std;
using namespace Marsyas;

ViconFileSource::ViconFileSource(string name):MarSystem("ViconFileSource",name)
{
  //type_ = "ViconFileSource";
  //name_ = name;
  
  vfp_ = 0;

	addControls();
}


ViconFileSource::~ViconFileSource()
{
  if (vfp_ != NULL)
    fclose(vfp_);
}

void 
ViconFileSource::addControls()
{
  addctrl("mrs_bool/notEmpty", true);  
  addctrl("mrs_natural/size", 0);
  addctrl("mrs_string/markers", " ");
  addctrl("mrs_string/filename", "dviconfile");
  setctrlState("mrs_string/filename", true);
}



MarSystem* 
ViconFileSource::clone() const
{
  return new ViconFileSource(*this);
}



void 
ViconFileSource::getHeader(string filename)
{
  // Need to read Vicon File Header to get number and name of markers
  vfp_ = fopen(filename.c_str(), "r");
  if (vfp_)
    {
      // read first line from file
      char buffer[4096];
      fgets(buffer, 4096, vfp_);
      stringstream line(buffer);
      char entry[256];
      fileObs_ = 0;
      while (line.getline(entry, 256, ','))
	{
	  
	  fileObs_++;
	}
      setctrl("mrs_natural/onObservations", fileObs_);
      setctrl("mrs_string/markers", buffer);
    }
} 


void
ViconFileSource::myUpdate(MarControlPtr sender)
{
  inObservations_ = getctrl("mrs_natural/inObservations")->toNatural();
  israte_ = getctrl("mrs_real/israte")->toReal();


  if (filename_ != getctrl("mrs_string/filename")->toString())
    {
      filename_ = getctrl("mrs_string/filename")->toString();
      getHeader(filename_);
      
    }
  
  setctrl("mrs_natural/onSamples", inSamples_);
  setctrl("mrs_natural/onObservations", fileObs_);
  setctrl("mrs_real/osrate", israte_);
    
    
}
 
void
ViconFileSource::myProcess(realvec& in, realvec& out)
{
  //checkFlow(in,out);

  
  for (t = 0; t < inSamples_; t++)
    {
      bool notValidLine = true;
      char buffer[4096];      
      while (notValidLine) 
	{
	  char *res;
	  res = fgets(buffer, 4096, vfp_);
	  if (res == NULL) 
	    {
	      setctrl("mrs_bool/notEmpty",false);
	      return;
	    }
	  
	  stringstream line(buffer);
	  stringstream pline(buffer);
	  char entry[256];
	  notValidLine = false;
	  for (o=0; o < onObservations_; o++)
	    { 
	      line.getline(entry, 256, ',');
	      if (!strcmp(entry,"")) 
		{
		  for (mrs_natural j=0; j < o; j++)
		    out(j,t) = 0.0;
		  notValidLine = true;
		}
	      else 
			out(o,t) = (mrs_real)atof(entry);

	      if (notValidLine) break;
	    }
	}
    }
}




  
  


