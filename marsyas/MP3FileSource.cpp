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


#include "MP3FileSource.h"

using namespace std;
using namespace Marsyas;

#define INPUT_BUFFER_SIZE (5*8192)

MP3FileSource::MP3FileSource(string name):AbsSoundFileSource("MP3FileSource", name)
{
  //type_ = "MP3FileSource";
  //name_ = name;

  ri_ = preservoirSize_ = 0;
  ptr_ = NULL;

  fileSize_ = 0;
  fd = 0;  
  fp = NULL;
  offset = 0;
  pos_ = 0;
  size_ = 0;
  currentPos_ = 0;
  
  
  addControls();
}

MP3FileSource::~MP3FileSource()
{
#ifdef MARSYAS_MAD  
  madStructFinish();
#endif
  closeFile(); 
}

MP3FileSource::MP3FileSource(const MP3FileSource& a):AbsSoundFileSource(a)
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

  ptr_ = NULL;
  fp = NULL;
  
  pos_ = 0;
  size_ = 0;
  currentPos_ = 0;
  ctrl_currentlyPlaying_ = getctrl("mrs_string/currentlyPlaying");
}

MarSystem* 
MP3FileSource::clone() const
{
  return new MP3FileSource(*this);
}


void 
MP3FileSource::addControls()
{
  addctrl("mrs_natural/bitRate", 160000);
  addctrl("mrs_bool/init", false);
  setctrlState("mrs_bool/init", true);
  addctrl("mrs_bool/notEmpty", true);
  addctrl("mrs_natural/loopPos", (mrs_natural)0);
  setctrlState("mrs_natural/loopPos", true);
  addctrl("mrs_natural/pos", (mrs_natural)0);
  setctrlState("mrs_natural/pos", true);
  addctrl("mrs_string/filename", "daufile");
  setctrlState("mrs_string/filename", true);
  addctrl("mrs_natural/size", (mrs_natural)0);
  addctrl("mrs_string/filetype", "mp3");
  addctrl("mrs_real/repetitions", 1.0);
  setctrlState("mrs_real/repetitions", true);
  addctrl("mrs_real/duration", -1.0);
  setctrlState("mrs_real/duration", true);

  addctrl("mrs_bool/advance", false);
  setctrlState("mrs_bool/advance", true);

  addctrl("mrs_bool/shuffle", false);
  setctrlState("mrs_bool/shuffle", true);

  addctrl("mrs_natural/cindex", 0);
  setctrlState("mrs_natural/cindex", true);

  addctrl("mrs_string/allfilenames", ",");
  addctrl("mrs_natural/numFiles", 1);
	
  addctrl("mrs_string/currentlyPlaying", "daufile", ctrl_currentlyPlaying_);
}

/** 
 * Function: getHeader
 * Description: Opens the MP3 file and collects all the necessary
 * 		information to update the MarSystem. 
 */
void 
MP3FileSource::getHeader(string filename) 
{

#ifdef MARSYAS_MAD  
  // if we have a file open already, close it
  closeFile();
  
  
  fp = fopen(filename.c_str(), "rb");
  fseek(fp, 0L, SEEK_END);
  myStat.st_size = ftell(fp);
  fseek(fp, 0L, SEEK_SET);



  if (myStat.st_size == 0 ) {
    MRSWARN("Error reading file: " + filename);
    setctrl("mrs_natural/onObservations", 1);
    setctrl("mrs_real/israte", 22050.0);
    setctrl("mrs_natural/size", 0);
    notEmpty_ = 0;
    setctrl("mrs_bool/notEmpty", false);	  
    return;
  }
  

  
  
  // libmad seems to read sometimes beyond the file size 
  // added 2048 as padding for these cases - most likely 
  // mp3 that are not properly formatted 
  ptr_ = new unsigned char[myStat.st_size+2048]; 

  
  int numRead = fread(ptr_, sizeof(unsigned char), myStat.st_size, fp);
  
  if (numRead != myStat.st_size) 
    {
      MRSWARN("Error reading: " + filename + " to memory.");
      setctrl("mrs_natural/onObservations", 1);
      setctrl("mrs_real/israte", 22050.0);
      setctrl("mrs_natural/size", 0);
      notEmpty_ = 0;
      setctrl("mrs_bool/notEmpty", false);	  
      return;
    }
  
  fileSize_ = myStat.st_size;
  
  
  // initialize mad structs and fill the stream
  madStructInitialize();
  fillStream();	
 

  // if there is nothing in the stream...
  notEmpty_ = getctrl("mrs_bool/notEmpty")->toBool(); 
  if (!notEmpty_) {
    pos_ = 0;
    return;
  }
  
  // decode some frames until we find the samplerate and bitrate
  while ( frame.header.samplerate == 0 ||  frame.header.bitrate == 0 ) 
    {
      pos_ += bufferSize_;
      currentPos_ = pos_;
      
      if ( mad_frame_decode(&frame, &stream) ) 
	{
	
	  if(MAD_RECOVERABLE(stream.error)) 
	    {
				
	      if(stream.error != MAD_ERROR_LOSTSYNC) {
		MRSWARN("MP3FileSource: recoverable frame level error");
	      }
	      
	      // get some more samples...
	      fillStream();
	      if (!notEmpty_) {
		pos_ = 0;
		return;
	      }
	      
	    } 
	  else if(stream.error==MAD_ERROR_BUFLEN) 
	    {

	      fillStream();
	      if (!notEmpty_) {
		pos_ = 0;
		return;
	      }
	      
	    } 
	  else 
	    {
	      MRSERR("MP3FileSource: unrecoverable frame level error, quitting.");
	      
	    }
	  
	  frameCount_++;
	}
    
  }
	
  frameSamples_ = 32 * MAD_NSBSAMPLES(&frame.header);
  bufferSize_ = frameSamples_; // mad frame size
  mrs_natural bitRate = frame.header.bitrate;
  mrs_real sampleRate = frame.header.samplerate;

  
  // only works for a constant bitrate, duration is (bits in file / bitrate)
  mrs_real duration_ = 2 * (fileSize_ * 8) / bitRate;
  advance_ = getctrl("mrs_bool/advance")->toBool();
  cindex_ = getctrl("mrs_natural/cindex")->toNatural();
  
  
  size_ = (mrs_natural) ((duration_ * sampleRate) / MAD_NCHANNELS(&frame.header));

  
  csize_ = size_ * MAD_NCHANNELS(&frame.header);
  totalFrames_ = (mrs_natural)((sampleRate * duration_) / frameSamples_);
  
  
  // update some controls 
  updctrl("mrs_real/duration", duration_);
  updctrl("mrs_natural/onObservations", MAD_NCHANNELS(&frame.header)); 
  updctrl("mrs_real/israte", sampleRate);
  updctrl("mrs_natural/size", size_ / 2);
  updctrl("mrs_natural/bitRate", bitRate);

  ctrl_currentlyPlaying_->setValue(filename, NOUPDATE);
  

  offset = 0;
  pos_ = samplesOut_ = frameCount_ = 0;
  currentPos_ = 0;
  notEmpty_ = 1;
  
#endif
}	


/**
 * Function: update
 *
 * Description: Performs the usual MarSystem update jobs. Additionally, 
 * 		it can update the position of the index in the file if
 * 		the user seeks forward or backward.  Note that this 
 * 		is currently going to be slow as we have to refill the 
 * 		mad buffer each time somebody seeks in the file. 
 *
 */
void 
MP3FileSource::myUpdate(MarControlPtr sender)
{
  MRSDIAG("MP3FileSource::myUpdate");
  
  MarSystem::myUpdate(sender);
 
  israte_ = ctrl_israte_->toReal();
  inSamples_ = ctrl_inSamples_->toNatural();
  pos_ = getctrl("mrs_natural/pos")->toNatural();
  mrs_natural nChannels = ctrl_onObservations_->toNatural();
  
  // if the user has seeked somewhere in the file
  if ( (currentPos_ != pos_) && (pos_ < size_)) 
    {
      
      // compute a new file offset using the frame target
      mrs_real ratio = (mrs_real)pos_/size_;
      
#ifdef MARSYAS_MAD     
      madStructInitialize();
#endif 
      
      mrs_natural targetOffset = (mrs_natural) (fileSize_ * (mrs_real)ratio);
      
      // if we are rewinding, we call fillStream with -1
      if (targetOffset==0) {
	fillStream(-1);
      } else {
	fillStream(targetOffset);
      }
      currentPos_ = pos_;
    }
  
  filename_ = getctrl("mrs_string/filename")->toString();    
  duration_ = getctrl("mrs_real/duration")->toReal();
  advance_ = getctrl("mrs_bool/advance")->toBool();
  //rewindpos_ = pos_;
  
  repetitions_ = getctrl("mrs_real/repetitions")->toReal();
  
  if (duration_ != -1.0)
    {
      csize_ = (mrs_natural)(duration_ * israte_);
    }
	
	inSamples_ = ctrl_inSamples_->toNatural();
  	
  if (inSamples_ < bufferSize_/2) {
    reservoirSize_ = 2 * nChannels * bufferSize_;
  } else { 
    reservoirSize_ = 2 * nChannels * inSamples_;
  }
  if (reservoirSize_ > preservoirSize_) {
    reservoir_.stretch(nChannels,reservoirSize_);
  }
  preservoirSize_ = reservoirSize_;

}



/**
 * Function: getLinear16
 * 
 * Description: This function does all the work. It is called by process,
 * 		and its job is to fill an output vector of the correct
 * 		size to push through the system.  The MAD decoder can
 * 		only retrieve buffer sizes of 1152, so therefore we have
 * 		to balance and maintain a reservoir.  Thus, we only get
 * 		more samples from MAD when the reservoir is below a 
 * 		threshold.  (As in AudioSource, etc). 
 *
 */
mrs_natural 
MP3FileSource::getLinear16(realvec& slice) {

		

#ifdef MARSYAS_MAD  
  register double peak = 1.0/32767; // normalize 24-bit sample
  register mad_fixed_t left_ch, right_ch;
  register mrs_real sample;
	 
  // decode a frame if necessary 
  while (ri_ < inSamples_) {
    
    fillStream();
    
    if (!notEmpty_) {
      pos_ = 0;
      return pos_;
    }
    
    
    if (mad_frame_decode(&frame, &stream )) 
      {
	if(MAD_RECOVERABLE(stream.error)) 
	  {
	    
	    if(stream.error != MAD_ERROR_LOSTSYNC) {
	      MRSWARN("MP3FileSource: recoverable frame level error");
	    }
	    
	    fillStream();
	    if (!notEmpty_) {
	      pos_ = 0;
	      return pos_;
	    }
	    
	  } 
	else 
	  if(stream.error==MAD_ERROR_BUFLEN) 
	    {
	      
	      fillStream(); 
	      if (!notEmpty_) {
		pos_ = 0;
		return pos_;
	      }
	      
	    } 
	
	  else 
	    {
	      MRSERR("MP3FileSource: unrecoverable frame level error, quitting.");
	    }
	
	frameCount_++;
      }
    

    mad_synth_frame(&synth, &frame);  
			
    // fill the reservoir...
    for (t=0; t < bufferSize_; t++) {
			
      left_ch = synth.pcm.samples[0][t];
      
      
      sample = (mrs_real) scale(left_ch);	
      sample *= peak;
      reservoir_(0, ri_) = sample;
      
      
      // for 2 channel audio we can add the channels 
      // and divide by two
      if(MAD_NCHANNELS(&frame.header)==2) {
	right_ch = synth.pcm.samples[1][t];
	sample = (mrs_real) scale(right_ch);
	sample *= peak;
	reservoir_(1, ri_) = sample;
      }
      
      ri_++;
    }
    
  } // reservoir fill
  
  
  // spit out the first inSamples_ in our reservoir 
  for (t=0; t < inSamples_; t++) {
    slice(0,t) = reservoir_(0,t);
    if (MAD_NCHANNELS(&frame.header)==2) 
      {
	slice(1,t) = reservoir_(1,t);
      }
  }
	
  // keep track of where we are
  pos_ += inSamples_; // (inSamples_ * getctrl("mrs_natural/nChannels")->toNatural());
  currentPos_ = pos_;	
	
	
  // move the data we ticked to the front of the reservoir
  for (t=inSamples_; t < ri_; t++) {
    reservoir_(0,t-inSamples_) = reservoir_(0,t);
    if (MAD_NCHANNELS(&frame.header)==2) 
      reservoir_(1,t-inSamples_) = reservoir_(1,t);      
  }
  
  // update our reservroi index
  ri_ = ri_ - inSamples_;	

  return pos_;
#else
  return 0;
  
#endif 

}


/**
 * Function: process
 * Description: Fills an output vector with samples.  In this case,
 * 		getLinear16 does all the work.
 */
void MP3FileSource::myProcess(realvec& in, realvec& out)
{
  //checkFlow(in,out);

  if (notEmpty_) 
    getLinear16(out);
  else
    out.setval(0.0);

  samplesOut_ += onSamples_;
  
  if (notEmpty_) {
  	notEmpty_ = (samplesOut_ < repetitions_ * csize_);
  } else{
	  // if notEmpty_ was false already it got set in fillStream
    // MRSWARN("MP3FileSource: track ended.");
  }
  
}


/*
 * Initialize mad structs
 */
#ifdef MARSYAS_MAD  
void MP3FileSource::madStructInitialize() {

	mad_stream_init(&stream);
	mad_frame_init(&frame);
	mad_synth_init(&synth);
}
#endif

	

/*
 * Release mad structs
 */
#ifdef MARSYAS_MAD  
void MP3FileSource::madStructFinish() {

	mad_stream_finish(&stream);
	mad_frame_finish(&frame);
	mad_synth_finish(&synth);
}
#endif 




/**
 * Function: fillstream()
 *
 * Description: Fill the mad stream with a chunk of audio. This function was
 * 		inspired from IzSounds maddecoder implementation. 
 *
 */
void 
MP3FileSource::fillStream( mrs_natural target ) 
{

  // fill the input buffer
#ifdef MARSYAS_MAD  
  if (stream.buffer == NULL || stream.error == MAD_ERROR_BUFLEN) 
    {
    
      register mrs_natural remaining = 0;
      register mrs_natural chunk = INPUT_BUFFER_SIZE;
      
      // when called with the default parameter, carry on decoding...	  
      if ( stream.next_frame != NULL ) {
	offset = stream.next_frame - ptr_;  
	remaining = fileSize_ - offset;
      } else if ( target != 0 ) {	
	// we have seeked somewhere in the file...
	offset = target;
	remaining = fileSize_ - offset;
      } else if ( target == -1 ) {
	// we rewound the track...
	offset = 0;
	remaining = fileSize_;
      }
      
      // there may not be enough to fill the buffer
      if ( remaining < INPUT_BUFFER_SIZE ) {
	chunk = remaining + MAD_BUFFER_GUARD;
      }
      
      // if we have hit the end...
      if ( offset >= fileSize_ ) {
	notEmpty_ = false;
	// MRSWARN("MP3FileSource: cannot seek to offset");
      } else {
	// fill the mad buffer
	mad_stream_buffer(&stream, ptr_ + offset, chunk);
	stream.error = MAD_ERROR_NONE;
      }
    }
  
#endif
}



/**
 * Function: closeFile()
 *
 * Description: Close the file if its open, release memory, and 
 * 		release mad structs.
 *
 */
void MP3FileSource::closeFile()
{
	
  // close the file and release mad structs
  if (fp == NULL) 
    return;

  fclose(fp);
  fd = 0;
  pos_ = 0;
  currentPos_ = 0;
  size_ = 0;
  

  delete [] ptr_;


#ifdef MARSYAS_MAD  
  madStructFinish();
#endif 
}




/**
 *
 * Function: scale 
 *
 * This function (and documentation) was taken directly from minimad.c.
 * 
 * The following utility routine performs simple rounding, clipping, and
 * scaling of MAD's high-resolution samples down to 16 bits. It does not
 * perform any dithering or noise shaping, which would be recommended to
 * obtain any exceptional audio quality. It is therefore not recommended to
 * use this routine if high-quality output is desired.
 * 
 */

#ifdef MARSYAS_MAD  
inline signed int MP3FileSource::scale(mad_fixed_t sample)
{
  // round 
  sample += (1L << (MAD_F_FRACBITS - 16));

  // clip
  if (sample >= MAD_F_ONE)
    sample = MAD_F_ONE - 1;
  else if (sample < -MAD_F_ONE)
    sample = -MAD_F_ONE;

  // quantize
  return sample >> (MAD_F_FRACBITS + 1 - 16);
}
#endif 
