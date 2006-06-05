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
   \brief ViconFileSource for exported VICON .txt files
   
   ViconFileSource reader for exported VICON .txt files. 
*/

#include "ViconFileSource.h"
using namespace std;


ViconFileSource::ViconFileSource(string name)
{
  type_ = "ViconFileSource";
  name_ = name;
  
  addControls();
  vfp_ = 0;
  
}


ViconFileSource::~ViconFileSource()
{
  if (vfp_ != NULL)
    fclose(vfp_);
}

void 
ViconFileSource::addControls()
{
  addDefaultControls();
  addctrl("bool/notEmpty", true);  
  addctrl("natural/size", 0);
  addctrl("string/markers", " ");
  addctrl("string/filename", "dviconfile");
  setctrlState("string/filename", true);
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
      setctrl("natural/onObservations", fileObs_);
      setctrl("string/markers", buffer);
    }
} 


void
ViconFileSource::update()
{
  inObservations_ = getctrl("natural/inObservations").toNatural();
  israte_ = getctrl("real/israte").toReal();


  if (filename_ != getctrl("string/filename").toString())
    {
      filename_ = getctrl("string/filename").toString();
      getHeader(filename_);
      
    }
  
  setctrl("natural/onSamples", inSamples_);
  setctrl("natural/onObservations", fileObs_);
  setctrl("real/osrate", israte_);
    
       
  defaultUpdate();
}
 
void
ViconFileSource::process(realvec& in, realvec& out)
{
  checkFlow(in,out);

  
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
	      setctrl("bool/notEmpty",(MarControlValue)false);
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
		  for (natural j=0; j < o; j++)
		    out(j,t) = 0.0;
		  notValidLine = true;
		}
	      else 
		out(o,t) = (real)atof(entry);

	      if (notValidLine) break;
	    }
	}
    }
}




  
  


