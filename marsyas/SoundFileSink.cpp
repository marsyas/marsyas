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


/**
   \class SoundFileSink
   \ingroup IO
   \brief Interface for sound file sinks
   
   Abstract base class for any type of sound file input.
Specific IO classes for various formats like AuFileSink 
are children of this class. 

   Controls:
   - \b mrs_natural/channel	: dunno.  CCC channel vs. nChannels ?  eh?!?!
   - \b mrs_natural/nChannels	: number of channels (mono, stereo, etc)
   - \b mrs_string/filename	: name of the audio file to write

*/

#include "SoundFileSink.h"

using namespace std;
using namespace Marsyas;

#define UnsignedToFloat(u) (((double)((long)(u - 2147483647L - 1))) + 2147483648.0)
#define FloatToUnsigned(f)      ((unsigned long)(((long)(f - 2147483648.0)) + 2147483647L) + 1)
	


SoundFileSink::SoundFileSink(string name):MarSystem("SoundFileSink",name)
{
  //type_ = "SoundFileSink";
  //name_ = name;
  
	sdata_ = NULL;
  cdata_ = NULL;
  sfp_ = NULL;
  dest_ = NULL;

	addControls();
}


SoundFileSink::SoundFileSink(const SoundFileSink& a):MarSystem(a)
{
// 	type_ = a.type_;
// 	name_ = a.name_;
// 	ncontrols_ = a.ncontrols_; 		
// 
// 	inSamples_ = a.inSamples_;
// 	inObservations_ = a.inObservations_;
// 	onSamples_ = a.onSamples_;
// 	onObservations_ = a.onObservations_;
// 	dbg_ = a.dbg_;
// 	mute_ = a.mute_;

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
  addctrl("mrs_natural/channel", (mrs_natural)0);
  addctrl("mrs_natural/nChannels",(mrs_natural)1);
  addctrl("mrs_string/filename", "defaultfile");
  setctrlState("mrs_string/filename", true);
}


void
SoundFileSink::putHeader()
{
  string filename = getctrl("mrs_string/filename")->toString();
  dest_->putHeader(filename);
}

/*
 * C O N V E R T   F R O M   I E E E   E X T E N D E D
 */

/*
 * Copyright (C) 1988-1991 Apple Computer, Inc.
 * All rights reserved.
 *
 * Machine-independent I/O routines for IEEE floating-point numbers.
 *
 * NaN's and infinities are converted to HUGE_VAL or HUGE, which
 * happens to be infinity on IEEE machines.  Unfortunately, it is
 * impossible to preserve NaN's in a machine-independent way.
 * Infinities are, however, preserved on IEEE machines.
 *
 * These routines have been tested on the following machines:
 *    Apple Macintosh, MPW 3.1 C compiler
 *    Apple Macintosh, THINK C compiler
 *    Silicon Graphics IRIS, MIPS compiler
 *    Cray X/MP and Y/MP
 *    Digital Equipment VAX
 *
 *
 * Implemented by Malcolm Slaney and Ken Turkowski.
 *
 * Malcolm Slaney contributions during 1988-1990 include big- and little-
 * endian file I/O, conversion to and from Motorola's extended 80-bit
 * floating-point format, and conversions to and from IEEE single-
 * precision floating-point format.
 *
 * In 1991, Ken Turkowski implemented the conversions to and from
 * IEEE double-precision format, added more precision to the extended
 * conversions, and accommodated conversions involving +/- infinity,
 * NaN's, and denormalized numbers.
 */

#ifndef HUGE_VAL
# define HUGE_VAL HUGE
#endif /*HUGE_VAL*/

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
	  
	  setctrl("mrs_natural/nChannels", dest_->getctrl("mrs_natural/nChannels"));
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



/* 
void
SoundFileSink::ConvertToIeeeExtended(double num,char *bytes)
{
    int    sign;
    int expon;
    double fMant, fsMant;
    unsigned long hiMant, loMant;

    if (num < 0) {
        sign = 0x8000;
        num *= -1;
    } else {
        sign = 0;
    }

    if (num == 0) {
        expon = 0; hiMant = 0; loMant = 0;
    }
    else {
        fMant = frexp(num, &expon);
        if ((expon > 16384) || !(fMant < 1)) {    Infinity or NaN 
            expon = sign|0x7FFF; hiMant = 0; loMant = 0;  infinity 
        }
        else {    Finite 
            expon += 16382;
            if (expon < 0) {     denormalized 
                fMant = ldexp(fMant, expon);
                expon = 0;
            }
            expon |= sign;
            fMant = ldexp(fMant, 32);          
            fsMant = floor(fMant); 
            hiMant = FloatToUnsigned(fsMant);
            fMant = ldexp(fMant - fsMant, 32); 
            fsMant = floor(fMant); 
            loMant = FloatToUnsigned(fsMant);
        }
    }
    
    bytes[0] = expon >> 8;
    bytes[1] = expon;
    bytes[2] = hiMant >> 24;
    bytes[3] = hiMant >> 16;
    bytes[4] = hiMant >> 8;
    bytes[5] = hiMant;
    bytes[6] = loMant >> 24;
    bytes[7] = loMant >> 16;
    bytes[8] = loMant >> 8;
    bytes[9] = loMant;
}
*/ 

/****************************************************************
 * Extended precision IEEE floating-point conversion routine.
 ****************************************************************/


/* 
double 
SoundFileSink::ieee_ext2double(unsigned char *bytes)
{
  double    f;
  int    expon;
  unsigned long hiMant, loMant;

  expon = ((bytes[0] & 0x7F) << 8) | (bytes[1] & 0xFF);
  hiMant    =    ((unsigned long)(bytes[2] & 0xFF) << 24)
    |    ((unsigned long)(bytes[3] & 0xFF) << 16)
    |    ((unsigned long)(bytes[4] & 0xFF) << 8)
    |    ((unsigned long)(bytes[5] & 0xFF));
  loMant    =    ((unsigned long)(bytes[6] & 0xFF) << 24)
    |    ((unsigned long)(bytes[7] & 0xFF) << 16)
    |    ((unsigned long)(bytes[8] & 0xFF) << 8)
    |    ((unsigned long)(bytes[9] & 0xFF));
	    
  if (expon == 0 && hiMant == 0 && loMant == 0) {
    f = 0;
  }
  else {
    if (expon == 0x7FFF) {    Infinity or NaN 
    f = HUGE_VAL;
    }
    else {
      expon -= 16383;
      f  = ldexp(UnsignedToFloat(hiMant), expon-=31);
      f += ldexp(UnsignedToFloat(loMant), expon-=32);
    }
  }

  if (bytes[0] & 0x80)
    return -f;
  else
    return f;
}
*/ 


/* void
SoundFileSink::write_ieee_extended(FILE *fp, double num)
{
  char buf[10];
  ConvertToIeeeExtended(num, buf);
  fwrite(buf, 1, 10, fp);
}
*/ 



/* read_ieee_extended:
   Read in an ieee-extended float number of 10 bits
   */

/* double 
SoundFileSink::read_ieee_extended()
{
  unsigned char buf[10];
  if (fread(buf, 1, 10, sfp_) != 10)
    return -1;
  return ieee_ext2double(buf);
}
*/ 

void 
SoundFileSink::putFloat(mrs_natural c, realvec& win)
{
  MRSWARN("SoundFileSink::putFloat not implemented");
}




/* void 
SoundFileSink::putLinear16(mrs_natural c, realvec& slice)
{
  mrs_natural nChannels = getctrl("mrs_natural/nChannels")->toNatural();
  mrs_natural nSamples = getctrl("mrs_natural/inSamples")->toNatural();

  for (t=0; t < nSamples; t++)
    {
      sdata_[t*nChannels + c] = htons((short)(slice(0,t) * PCM_MAXSHRT));
    }
  
  

  if (c == nChannels -1) 
    {
      if ((mrs_natural)fwrite(sdata_, sizeof(short), nChannels * nSamples, sfp_) != nChannels * nSamples)
	{
	MRSERR("Problem: could not write window to file " + filename_);
	}
    }
}
*/ 


/* 
void 
SoundFileSink::putLinear16Swap(mrs_natural c, realvec& slice)
{

  
  mrs_natural nChannels = getctrl("mrs_natural/nChannels")->toNatural();
  mrs_natural nSamples = getctrl("mrs_natural/inSamples")->toNatural();
  
  for (t=0; t < nSamples; t++)
    {
      sdata_[t*nChannels + c] = (short)(slice(0,t) * PCM_MAXSHRT);
    }

  if (c == nChannels -1) 
    {
      if ((mrs_natural)fwrite(sdata_, sizeof(short), nChannels * nSamples, sfp_) != nChannels * nSamples)
	{
	MRSERR("Problem: could not write window to file ");
	}
    }
}
*/ 







/* 
ostream& 
operator<< (ostream& o, const SoundFileSink& s)
{
  o << "# MARSYAS SoundFileSink" << endl;
  o << "# FileName      = " << s.filename_ << endl;
  o << "# FileType      = " << s.filetype_ << endl;
  o << "# Channels      = " << s.channels_ << endl;
  o << "# Sampling Rate = " << s.srate_ << endl;
  o << "# Size(samples) = " << s.size_ << endl << endl;
  o << endl;
}
*/ 




	
