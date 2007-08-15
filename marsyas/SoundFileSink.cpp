/*
** Copyright (C) 1998-2005 George Tzanetakis <gtzan@cs.uvic.ca>
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


#include "SoundFileSink.h"

using namespace std;
using namespace Marsyas;

#define UnsignedToFloat(u) (((double)((long)(u - 2147483647L - 1))) + 2147483648.0)
#define FloatToUnsigned(f)      ((unsigned long)(((long)(f - 2147483648.0)) + 2147483647L) + 1)
	


SoundFileSink::SoundFileSink(string name):MarSystem("SoundFileSink",name)
{

  
  sdata_ = NULL;
  cdata_ = NULL;
  sfp_ = NULL;
  dest_ = NULL;

  addControls();
}


SoundFileSink::SoundFileSink(const SoundFileSink& a):MarSystem(a)
{
  sdata_ = NULL;
  cdata_ = NULL;
  sfp_ = NULL;
  dest_ = NULL;
}


MarSystem* 
SoundFileSink::clone() const
{
  return new SoundFileSink(*this);
}

SoundFileSink::~SoundFileSink()
{
  delete dest_;
}

void
SoundFileSink::addControls()
{
  addctrl("mrs_string/filename", "defaultfile");
  setctrlState("mrs_string/filename", true);
}


void
SoundFileSink::putHeader()
{
  string filename = getctrl("mrs_string/filename")->toString();
  dest_->putHeader(filename);
}



bool 
SoundFileSink::checkType()
{
  string filename = getctrl("mrs_string/filename")->toString();
  // check if file exists
  if (filename != "defaultfile")
    {
      sfp_ = fopen(filename.c_str(), "wb");
      if (sfp_ == NULL) 
	{
		string wrn = "SoundFileSink::checkType: Problem opening file ";
	  wrn += filename;
	  MRSWARN(wrn);
	  filename = "defaultfile";
	  return false;
	}
      fclose(sfp_);
    }

  // try to open file with appropriate format 
  string::size_type pos = filename.rfind(".", filename.length());
  string ext;
  if (pos == string::npos) ext = "";
  else 
    ext = filename.substr(pos, filename.length());  
  
  if (ext == ".au")
    {
      delete dest_;
			dest_ = new AuFileSink(getName());
    }
  else if (ext == ".wav")
    {
      delete dest_;
			dest_ = new WavFileSink(getName());
    }
  else 
    {
      if (filename != "defaultfile")
	{
	  string wrn = "Unsupported format for file ";
	  wrn += filename;
	  MRSWARN(wrn);
	  filename_ = "defaultfile";
	  setctrl("mrs_string/filename", "defaultfile");
	  return false;
	}
      else 
	return false;
    }
  
  return true;
}

void 
SoundFileSink::myUpdate(MarControlPtr sender)
{
  
  if (filename_ != getctrl("mrs_string/filename")->toString())
    {
      if (checkType() == true)
	{
		dest_->setctrl("mrs_natural/inSamples", getctrl("mrs_natural/inSamples"));
	  dest_->setctrl("mrs_natural/inObservations", getctrl("mrs_natural/inObservations"));
	  dest_->setctrl("mrs_real/israte", getctrl("mrs_real/israte"));
	  dest_->update();

	  putHeader();
	  filename_ = getctrl("mrs_string/filename")->toString();
	  
	  setctrl("mrs_real/israte", dest_->getctrl("mrs_real/israte"));
	}
      else 
	dest_ = NULL;
    }

  
  if (dest_ != NULL)
    {
      dest_->setctrl("mrs_natural/inSamples", getctrl("mrs_natural/inSamples"));
      dest_->setctrl("mrs_natural/inObservations", getctrl("mrs_natural/inObservations"));
      dest_->setctrl("mrs_real/israte", getctrl("mrs_real/israte"));
      dest_->update();
      
      setctrl("mrs_natural/onSamples", dest_->getctrl("mrs_natural/onSamples"));
      setctrl("mrs_natural/onObservations", dest_->getctrl("mrs_natural/onObservations"));
      setctrl("mrs_real/osrate", dest_->getctrl("mrs_real/israte"));
      
      
    }
}
  

void 
SoundFileSink::myProcess(realvec& in, realvec& out)
{
  if (dest_ != NULL) 
    {
      dest_->process(in,out);
    }
 

}









void 
SoundFileSink::putFloat(mrs_natural c, realvec& win)
{
  MRSWARN("SoundFileSink::putFloat not implemented");
}




	
