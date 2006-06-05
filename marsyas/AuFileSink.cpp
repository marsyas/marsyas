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
   \class AuFileSink
   \brief SoundFileSink for .au (.snd) soundfiles
   
   SoundFileSink writer for .au (.snd) sound files.
   (Next, Sun audio format). 
*/


#include "AuFileSink.h"
using namespace std;



AuFileSink::AuFileSink(string name)
{
  type_ = "AuFileSink";
  name_ = name;
  cdata_ = NULL;
  sdata_ = NULL;
  addControls();
}


AuFileSink::~AuFileSink()
{
}

MarSystem* 
AuFileSink::clone() const
{
  return new AuFileSink(*this);
}


void 
AuFileSink::addControls()
{
  addDefaultControls();
  addctrl("natural/nChannels", (natural)1);
  setctrlState("natural/nChannels", true);
  addctrl("natural/channel", (natural)0);
  addctrl("string/filename", "daufile");
  setctrlState("string/filename", true);
}


bool 
AuFileSink::checkExtension(string filename)
{
  FileName fn(filename);
  string auext  = "au";
  string sndext = "snd";
  
  if ((fn.ext() == auext) || (fn.ext() == sndext))
    return true;
  else 
    return false;
}

void 
AuFileSink::update()
{
  MRSDIAG("AudioFileSink::update");
  setctrl("natural/onSamples", getctrl("natural/inSamples"));
  setctrl("natural/onObservations", getctrl("natural/inObservations"));
  setctrl("real/osrate", getctrl("real/israte"));

  natural nChannels = getctrl("natural/nChannels").toNatural();      
  
  delete sdata_;
  delete cdata_;
  
  sdata_ = new short[getctrl("natural/inSamples").toNatural() * nChannels];
  cdata_ = new unsigned char[getctrl("natural/inSamples").toNatural() * nChannels];

  filename_ = getctrl("string/filename").toString();
  
  defaultUpdate();
}

  
void 
AuFileSink::putHeader(string filename)
{
  natural nChannels = (natural)getctrl("natural/nChannels").toNatural();
  
  written_ = 0;
  char *comment = "MARSYAS 2001, George Tzanetakis.\n";
  int commentSize = strlen(comment);
  sfp_ = fopen(filename.c_str(), "wb");
  hdr_.pref[0] = '.';
  hdr_.pref[1] = 's';
  hdr_.pref[2] = 'n';
  hdr_.pref[3] = 'd';


#if defined(__BIG_ENDIAN__)
	  hdr_.hdrLength = 24 + commentSize;
	  hdr_.fileLength = 0;
	  hdr_.mode = SND_FORMAT_LINEAR_16;                           
	  hdr_.srate = (natural)getctrl("real/israte").toReal();
	  hdr_.channels = nChannels;

#else
	  hdr_.hdrLength = ByteSwapLong(24 + commentSize);
	  hdr_.fileLength = ByteSwapLong(0);
	  hdr_.mode = ByteSwapLong(SND_FORMAT_LINEAR_16);                           
	  hdr_.srate = ByteSwapLong((natural)getctrl("real/israte").toReal());
	  hdr_.channels = ByteSwapLong(nChannels);

#endif 

  




  fwrite(&hdr_, 24, 1, sfp_);
  // Write comment part of header 
  fwrite(comment, commentSize, 1, sfp_);      
  sfp_begin_ = ftell(sfp_);  
}


unsigned long 
AuFileSink::ByteSwapLong(unsigned long nLongNumber)
{
  return (((nLongNumber&0x000000FF)<<24)+((nLongNumber&0x0000FF00)<<8)+
	  ((nLongNumber&0x00FF0000)>>8)+((nLongNumber&0xFF000000)>>24));
}
 


unsigned short 
AuFileSink::ByteSwapShort (unsigned short nValue)
{
  return (((nValue>> 8)) | (nValue << 8));
}




void 
AuFileSink::putLinear16(natural c, realvec& slice)
{
  natural nChannels = getctrl("natural/nChannels").toNatural();
  natural nSamples = getctrl("natural/inSamples").toNatural();

  for (t=0; t < nSamples; t++)
    {
#if defined(__BIG_ENDIAN__)
      sdata_[t*nChannels + c] = (short)(slice(0,t) * MAXSHRT);      
#else
      sdata_[t*nChannels + c] = ByteSwapShort((short)(slice(0,t) * MAXSHRT));

#endif 
    }
  
  

  if (c == nChannels -1) 
    {
      if ((natural)fwrite(sdata_, sizeof(short), nChannels * nSamples, sfp_) != nChannels * nSamples)
	{
	  MRSWARN("Problem: could not write window to file" + filename_);
	}
    }
}





void 
AuFileSink::process(realvec& in, realvec& out)
{
  
  checkFlow(in,out);
  
  // copy input to output 
  for (o=0; o < inObservations_; o++)
    for (t=0; t < inSamples_; t++)
      {
	if (in(o,t) > 1.0)
	  MRSWARN("AuFileSink::Value out of range > 1.0");
	if (in(o,t) < -1.0)
	  MRSWARN("AuFileSink::Value out of range < -1.0"); 

	  out(o,t) = in(o,t);
      }
  
  
  natural c = getctrl("natural/channel").toNatural();
  natural nChannels = getctrl("natural/nChannels").toNatural();
  
  setctrl("natural/channel", (c + 1)%nChannels);  
  
  long fileSize;
  fpos_ = ftell(sfp_);
  fseek(sfp_, 8, SEEK_SET);
  written_ += inSamples_;
#if defined(__BIG_ENDIAN__)
  fileSize = (written_ * 2 * nChannels);
#else
  fileSize = ByteSwapLong(written_ * 2 * nChannels);
#endif

  fwrite(&fileSize, 4, 1, sfp_);
  fseek(sfp_, fpos_, SEEK_SET);
  
  putLinear16(c, in);

}








  
  



	

	
