/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include <marsyas/common_source.h>
#include "MP3FileSource.h"

using std::cout;
using std::endl;

using std::ostringstream;
using namespace Marsyas;

#define INPUT_BUFFER_SIZE (5*8192)

MP3FileSource::MP3FileSource(mrs_string name):AbsSoundFileSource("MP3FileSource", name)
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


  bufferSize_ = 576;	// must be initialized, otherwise it may be a very big value and
  // may further cause segment fault when allocating space for input buffer
  frameSamples_ = 0;
  totalFrames_ = 0;
  frameCount_ = 0;

  // variables for buffer balancing
  reservoirSize_ = 0;

  advance_ = 0;
  cindex_ = 0;

  duration_ = 0;
  csize_ = 0;
  samplesOut_ = 0;
  repetitions_ = 0;



  addControls();
}

MP3FileSource::~MP3FileSource()
{
  madStructFinish();
  closeFile();
}

MP3FileSource::MP3FileSource(const MP3FileSource& a):AbsSoundFileSource(a)
{
  ptr_ = NULL;
  fp = NULL;

  pos_ = 0;
  size_ = 0;
  currentPos_ = 0;

// must be initialized, otherwise it may be a very big value and
  bufferSize_ = 576;
// may further cause segment fault when allocating space for input buffer
  frameSamples_ = 0;
  totalFrames_ = 0;
  frameCount_ = 0;

// variables for buffer balancing
  reservoirSize_ = 0;

  advance_ = 0;
  cindex_ = 0;

  duration_ = 0;
  csize_ = 0;
  samplesOut_ = 0;
  repetitions_ = 0;

  ctrl_pos_ = getctrl("mrs_natural/pos");
  ctrl_currentlyPlaying_ = getctrl("mrs_string/currentlyPlaying");
  ctrl_previouslyPlaying_ = getctrl("mrs_string/previouslyPlaying");
  ctrl_regression_ = getctrl("mrs_bool/regression");
  ctrl_currentLabel_ = getctrl("mrs_real/currentLabel");
  ctrl_previousLabel_ = getctrl("mrs_real/previousLabel");
  ctrl_labelNames_ = getctrl("mrs_string/labelNames");
  ctrl_nLabels_ = getctrl("mrs_natural/nLabels");
  ctrl_currentHasData_ = getctrl("mrs_bool/currentHasData");
  ctrl_currentLastTickWithData_ = getctrl("mrs_bool/currentLastTickWithData");
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
  addctrl("mrs_bool/hasData", true);
  addctrl("mrs_bool/lastTickWithData", false);
  addctrl("mrs_natural/loopPos", (mrs_natural)0);
  setctrlState("mrs_natural/loopPos", true);
  addctrl("mrs_natural/pos", (mrs_natural)0, ctrl_pos_);
  setctrlState("mrs_natural/pos", true);
  addctrl("mrs_string/filename", "daufile");
  setctrlState("mrs_string/filename", true);
  addctrl("mrs_natural/size", (mrs_natural)0);
  addctrl("mrs_string/filetype", "mp3");
  addctrl("mrs_real/repetitions", 1.0);
  setctrlState("mrs_real/repetitions", true);
  addctrl("mrs_real/duration", -1.0);
  setctrlState("mrs_real/duration", true);


  addctrl("mrs_natural/advance", 0);
  setctrlState("mrs_natural/advance", true);

  addctrl("mrs_bool/shuffle", false);
  setctrlState("mrs_bool/shuffle", true);

  addctrl("mrs_natural/cindex", 0);
  setctrlState("mrs_natural/cindex", true);

  addctrl("mrs_string/allfilenames", ",");
  addctrl("mrs_natural/numFiles", 1);

  addctrl("mrs_string/currentlyPlaying", "daufile", ctrl_currentlyPlaying_);
  addctrl("mrs_string/previouslyPlaying", "daufile", ctrl_previouslyPlaying_);

  addctrl("mrs_bool/regression", false, ctrl_regression_);
  addctrl("mrs_real/currentLabel", 0.0, ctrl_currentLabel_);
  addctrl("mrs_real/previousLabel", 0.0, ctrl_previousLabel_);
  addctrl("mrs_string/labelNames",",", ctrl_labelNames_);
  addctrl("mrs_natural/nLabels", 0, ctrl_nLabels_);

  addctrl("mrs_bool/currentHasData", true, ctrl_currentHasData_);
  addctrl("mrs_bool/currentLastTickWithData", false, ctrl_currentLastTickWithData_);
}




void
MP3FileSource::PrintFrameInfo(struct mad_header *Header)
{
  const char	*Layer,
         *Mode,
         *Emphasis;

  /* Convert the layer number to it's printed representation. */
  switch(Header->layer)
  {
  case MAD_LAYER_I:
    Layer="I";
    break;
  case MAD_LAYER_II:
    Layer="II";
    break;
  case MAD_LAYER_III:
    Layer="III";
    break;
  default:
    Layer="(unexpected layer value)";
    break;
  }

  /* Convert the audio mode to it's printed representation. */
  switch(Header->mode)
  {
  case MAD_MODE_SINGLE_CHANNEL:
    Mode="single channel";
    break;
  case MAD_MODE_DUAL_CHANNEL:
    Mode="dual channel";
    break;
  case MAD_MODE_JOINT_STEREO:
    Mode="joint (MS/intensity) stereo";
    break;
  case MAD_MODE_STEREO:
    Mode="normal LR stereo";
    break;
  default:
    Mode="(unexpected mode value)";
    break;
  }

  /* Convert the emphasis to it's printed representation. Note that
   * the MAD_EMPHASIS_RESERVED enumeration value appeared in libmad
   * version 0.15.0b.
   */
  switch(Header->emphasis)
  {
  case MAD_EMPHASIS_NONE:
    Emphasis="no";
    break;
  case MAD_EMPHASIS_50_15_US:
    Emphasis="50/15 us";
    break;
  case MAD_EMPHASIS_CCITT_J_17:
    Emphasis="CCITT J.17";
    break;
#if (MAD_VERSION_MAJOR>=1) ||							\
	((MAD_VERSION_MAJOR==0) && (MAD_VERSION_MINOR>=15))
  case MAD_EMPHASIS_RESERVED:
    Emphasis="reserved(!)";
    break;
#endif
  default:
    Emphasis="(unexpected emphasis value)";
    break;
  }

  printf("%lu kb/s audio MPEG layer %s stream %s CRC, "
         "%s with %s emphasis at %d Hz sample rate\n",
         Header->bitrate,Layer,
         Header->flags&MAD_FLAG_PROTECTION?"with":"without",
         Mode,Emphasis,Header->samplerate);
}





/**
 * Function: getHeader
 * Description: Opens the MP3 file and collects all the necessary
 * 		information to update the MarSystem.
 */
void
MP3FileSource::getHeader(mrs_string filename)
{
  // cout << "getHeader " << filename << endl;

  debug_filename = filename;

  durFull_ = 0.;
  // if we have a file open already, close it
  closeFile();
  update();

  reservoir_.setval(0.0);

  fp = fopen(filename.c_str(), "rb");
  fseek(fp, 0L, SEEK_END);
  myStat.st_size = ftell(fp);
  fseek(fp, 0L, SEEK_SET);



  if (myStat.st_size == 0 ) {
    MRSWARN("Error reading file: " + filename);
    setctrl("mrs_natural/onObservations", 2);
    setctrl("mrs_real/israte", 22050.0);
    setctrl("mrs_natural/size", 0);
    hasData_ = 0;
    lastTickWithData_ = false;
    setctrl("mrs_bool/hasData", false);
    setctrl("mrs_bool/lastTickWithData", true);
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
    setctrl("mrs_natural/onObservations", 2);
    setctrl("mrs_real/israte", 22050.0);
    setctrl("mrs_natural/size", 0);
    hasData_ = 0;
    lastTickWithData_ = false;
    setctrl("mrs_bool/hasData", false);
    setctrl("mrs_bool/lastTickWithData", true);
    return;
  }

  fileSize_ = myStat.st_size;
  offset = 0;
  samplesOut_ = 0;
  ri_ = 0;


  // initialize mad structs and fill the stream
  madStructInitialize();
  fillStream();


  // if there is nothing in the stream...
  hasData_ = getctrl("mrs_bool/hasData")->to<mrs_bool>();
  if (!hasData_) {
    pos_ = 0;
    return;
  }

  // decode some frames until we find the samplerate and bitrate
  while (1)
  {
    pos_ += bufferSize_;
    currentPos_ = pos_;


    if ( mad_frame_decode(&frame, &stream) )
    {

      if(MAD_RECOVERABLE(stream.error))
      {

        if(stream.error != MAD_ERROR_LOSTSYNC) {
          mrs_string errmsg;
          errmsg += "MP3FileSource: recoverable frame level error: ";
          errmsg += mad_stream_errorstr(&stream);
          MRSDIAG(errmsg);
        }

        // get some more samples...
        fillStream();
        if (!hasData_) {
          pos_ = 0;
          return;
        }

      }
      else if(stream.error==MAD_ERROR_BUFLEN)
      {

        fillStream();
        if (!hasData_) {
          pos_ = 0;
          return;
        }

      }
      else
      {
        MRSERR("MP3FileSource: unrecoverable frame level error, quitting.");
        pos_ = 0;
        return;
      }

      frameCount_++;
    }
    else
      break;


  }

  pos_ = 0;
  currentPos_ = 0;

  // PrintFrameInfo(&frame.header);


  mrs_natural nChannels = MAD_NCHANNELS(&frame.header);
  setctrl("mrs_natural/onObservations", nChannels);


  frameSamples_ = 32 * MAD_NSBSAMPLES(&frame.header);
  bufferSize_ = frameSamples_; // mad frame size
  mrs_natural bitRate = frame.header.bitrate;
  mrs_real sampleRate = frame.header.samplerate;


  // only works for a constant bitrate, duration is (bits in file / bitrate)
  mrs_real duration_ = (fileSize_ * 8) / bitRate;
  advance_ = getctrl("mrs_natural/advance")->to<mrs_natural>();
  cindex_ = getctrl("mrs_natural/cindex")->to<mrs_natural>();

  size_ = (mrs_natural) (duration_ * sampleRate);


  csize_ = size_ ;


  totalFrames_ = (mrs_natural)((sampleRate * duration_) / frameSamples_);


  // update some controls
  setctrl("mrs_real/duration", duration_);
  setctrl("mrs_real/israte", sampleRate);
  setctrl("mrs_natural/size", size_ );
  setctrl("mrs_natural/bitRate", bitRate);

  update();


  ctrl_currentlyPlaying_->setValue(filename, NOUPDATE);
  ctrl_previouslyPlaying_->setValue(filename, NOUPDATE);

  ctrl_currentLabel_->setValue(0.0, NOUPDATE);
  ctrl_previousLabel_->setValue(0.0, NOUPDATE);
  ctrl_nLabels_->setValue(0, NOUPDATE);


  ctrl_labelNames_->setValue(",", NOUPDATE);



  offset = 0;
  pos_ = samplesOut_ = frameCount_ = 0;
  currentPos_ = 0;
  hasData_ = 1;


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
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("MP3FileSource::myUpdate");



  israte_ = ctrl_israte_->to<mrs_real>();
  inSamples_ = ctrl_inSamples_->to<mrs_natural>();
  // pos_ = getctrl("mrs_natural/pos")->to<mrs_natural>();

  mrs_natural nChannels = ctrl_onObservations_->to<mrs_natural>();

  setctrl("mrs_natural/onSamples", inSamples_);
  setctrl("mrs_real/osrate", israte_);




  filename_ = getctrl("mrs_string/filename")->to<mrs_string>();
  duration_ = getctrl("mrs_real/duration")->to<mrs_real>();
  advance_ = getctrl("mrs_natural/advance")->to<mrs_natural>();
  //rewindpos_ = pos_;

  repetitions_ = getctrl("mrs_real/repetitions")->to<mrs_real>();

  if (duration_ != -1.0)
  {
    csize_ = (mrs_natural)(duration_ * israte_);
  }

  inSamples_ = ctrl_inSamples_->to<mrs_natural>();

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
MP3FileSource::getLinear16(realvec& slice)
{


  register double peak = 1.0/32767; // normalize 24-bit sample
  register mad_fixed_t left_ch, right_ch;
  register mrs_real sample;



  // decode a frame if necessary
  while (ri_ < inSamples_) {

    fillStream();

    // if (!hasData_) {
    // pos_ = 0;
    // return pos_;
    // }

    if (mad_frame_decode(&frame, &stream ))
    {
      long bufferSize = ((long)stream.bufend-(long)stream.buffer)*8  - stream.md_len*8;

      if (frame.header.bitrate!=0 && bufferSize>0) durFull_ += (float)bufferSize/(float)frame.header.bitrate;
      // std::cout<<"decoded: bufptr="<<(long)stream.buffer<<" cnt="<<frameCount_<<" bps="<<frame.header.bitrate<<" bufSize="<<bufferSize<<" dur="<<durFull_<<std::endl;
      // std::cout<<"possible buffersize c="<<bufferSize<<" 1="<<((long)stream.next_frame-(long)stream.this_frame)*8<<" 2="<<stream.anc_bitlen<<" 3="<<stream.md_len*8<<std::endl;

      if(MAD_RECOVERABLE(stream.error))
      {

        if(stream.error != MAD_ERROR_LOSTSYNC) {
          mrs_string errmsg;
          errmsg += "MP3FileSource: recoverable frame level error :";
          errmsg += mad_stream_errorstr(&stream);
          MRSDIAG(errmsg);
        }

        fillStream();
        if (!hasData_) {
          pos_ = 0;
          return pos_;
        }

      }
      else if(stream.error==MAD_ERROR_BUFLEN)
      {

        fillStream();
        if (!hasData_) {
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
    for (mrs_natural t=0; t < bufferSize_; t++) {

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
  for (mrs_natural t=0; t < inSamples_; t++) {
    slice(0,t) = reservoir_(0,t);
    if (MAD_NCHANNELS(&frame.header)==2)
    {
      slice(1,t) = reservoir_(1,t);
    }
  }



  // keep track of where we are
  pos_ += inSamples_; // (inSamples_ * getctrl("mrs_natural/nChannels")->to<mrs_natural>());


  currentPos_ = pos_;


  // move the data we ticked to the front of the reservoir
  for (mrs_natural t=inSamples_; t < ri_; t++) {
    reservoir_(0,t-inSamples_) = reservoir_(0,t);
    if (MAD_NCHANNELS(&frame.header)==2)
      reservoir_(1,t-inSamples_) = reservoir_(1,t);
  }

  // update our reservroi index
  ri_ = ri_ - inSamples_;

  return pos_;
}


/**
 * Function: process
 * Description: Fills an output vector with samples.  In this case,
 * 		getLinear16 does all the work.
 */
void MP3FileSource::myProcess(realvec& in, realvec& out)
{
  (void) in;
  //checkFlow(in,out);


  if (hasData_)
    getLinear16(out);
  // else
  // out.setval(0.0);

  //if (!hasData_)
  // out.setval(0.0);



  ctrl_pos_->setValue(pos_, NOUPDATE);


  if (pos_ >= rewindpos_ + csize_)
  {
    if (repetitions_ != 1)
    {
      if (repetitions_ != 1)
        pos_ = rewindpos_;
      // compute a new file offset using the frame target
      mrs_real ratio = (mrs_real)pos_/size_;

      madStructInitialize();

      mrs_natural targetOffset = (mrs_natural) (fileSize_ * (mrs_real)ratio);

      // if we are rewinding, we call fillStream with -1
      if (targetOffset==0) {
        fillStream(-1);
      } else {
        fillStream(targetOffset);
      }
      currentPos_ = pos_;
    }

  }
  samplesOut_ += onSamples_;



  if (repetitions_ != 1)
  {
    hasData_ = (samplesOut_ < repetitions_ * csize_);
    lastTickWithData_ = ((samplesOut_  + onSamples_>= repetitions_ * csize_) && hasData_);
  }
  else
  {
    hasData_ = samplesOut_ < rewindpos_ + csize_;
    lastTickWithData_ = ((samplesOut_ + onSamples_ >= rewindpos_ + csize_) && hasData_);
  }

  if (repetitions_ == -1)
  {
    hasData_ = true;
    lastTickWithData_ = false;
  }




  ctrl_currentHasData_->setValue(hasData_);
  ctrl_currentLastTickWithData_->setValue(lastTickWithData_);


}


/*
 * Initialize mad structs
 */
void MP3FileSource::madStructInitialize() {

  mad_stream_init(&stream);
  mad_frame_init(&frame);
  mad_synth_init(&synth);
}



/*
 * Release mad structs
 */
void MP3FileSource::madStructFinish() {

  mad_stream_finish(&stream);
  mad_frame_finish(&frame);
  mad_synth_finish(&synth);
}




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
  if (stream.buffer == NULL || stream.error == MAD_ERROR_BUFLEN)
  {

    register mrs_natural remaining = 0;
    register mrs_natural chunk = INPUT_BUFFER_SIZE;

    // cout << "offset = " << offset << endl;

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
      hasData_ = false;
      // MRSWARN("MP3FileSource: cannot seek to offset");
    } else {
      // fill the mad buffer
      if (offset == -1)
        offset = 1;

      mad_stream_buffer(&stream, ptr_ + offset, chunk);

      stream.error = MAD_ERROR_NONE;
    }
  }
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
  ctrl_pos_->setValue(0, NOUPDATE);

  delete [] ptr_;


  madStructFinish();
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
