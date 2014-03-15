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

#include "../common_source.h"

#ifdef MARSYAS_MAD
#include "MP3FileSource.h"
#endif

#ifdef MARSYAS_VORBIS
#include "OggFileSource.h"
#endif

#ifdef MARSYAS_GSTREAMER
#include "GStreamerSource.h"
#endif


#include "SoundFileSource.h"
#include <string>
#include <cctype>
#include <algorithm>


//#define MTLB_DBG_LOG


using namespace std;
using namespace Marsyas;

SoundFileSource::SoundFileSource(mrs_string name):
  MarSystem("SoundFileSource",name),
  backend_ (0),
  updateCurrDuration(false)
{
  addControls();
}

SoundFileSource::~SoundFileSource()
{
  delete backend_;
}

MarSystem*
SoundFileSource::clone() const
{
  return new SoundFileSource(*this);
}

SoundFileSource::SoundFileSource(const SoundFileSource& a):
  MarSystem(a),
  backend_ (0),
  updateCurrDuration(false)
{
  ctrl_pos_ = getctrl("mrs_natural/pos");
  ctrl_loop_ = getctrl("mrs_natural/loopPos");
  ctrl_hasData_ = getctrl("mrs_bool/hasData");
  ctrl_lastTickWithData_ = getctrl("mrs_bool/lastTickWithData");
  ctrl_mute_ = getctrl("mrs_bool/mute");
  ctrl_advance_ = getctrl("mrs_natural/advance");
  ctrl_filename_ = getctrl("mrs_string/filename");
  ctrl_currentlyPlaying_ = getctrl("mrs_string/currentlyPlaying");
  ctrl_previouslyPlaying_ = getctrl("mrs_string/previouslyPlaying");
  ctrl_regression_ = getctrl("mrs_bool/regression");
  ctrl_currentLabel_ = getctrl("mrs_real/currentLabel");
  ctrl_previousLabel_ = getctrl("mrs_real/previousLabel");
  ctrl_labelNames_ = getctrl("mrs_string/labelNames");
  ctrl_nLabels_ = getctrl("mrs_natural/nLabels");
  ctrl_currentHasData_ = getctrl("mrs_bool/currentHasData");
  ctrl_currentLastTickWithData_ = getctrl("mrs_bool/currentLastTickWithData");
  ctrl_currentCollectionNewFile_ = getctrl("mrs_bool/currentCollectionNewFile");
  ctrl_startStable_ = getctrl("mrs_bool/startStable");

  //ctrl_rewindToPos_ = getctrl("mrs_natural/moveToSamplePos");
}

void
SoundFileSource::addControls()
{
  addctrl("mrs_bool/hasData", true, ctrl_hasData_);

  addctrl("mrs_bool/lastTickWithData", false, ctrl_lastTickWithData_);

  addctrl("mrs_natural/pos", 0, ctrl_pos_);
  setctrlState("mrs_natural/pos", true);

  addctrl("mrs_natural/loopPos", 0, ctrl_loop_);
  setctrlState("mrs_natural/loopPos", true);

  addctrl("mrs_string/filename", string(), ctrl_filename_);
  setctrlState("mrs_string/filename", true);

  addctrl("mrs_string/allfilenames", ",");
  setctrlState("mrs_string/allfilenames",true);

  addctrl("mrs_natural/numFiles", 0);

  addctrl("mrs_natural/size", 0);

  addctrl("mrs_real/frequency", (mrs_real)0.0);
  setctrlState("mrs_real/frequency",true);

  addctrl("mrs_bool/noteon", false);
  setctrlState("mrs_bool/noteon", true);

  addctrl("mrs_real/repetitions", 1.0);
  setctrlState("mrs_real/repetitions", true);

  addctrl("mrs_real/duration", -1.0);
  setctrlState("mrs_real/duration", true);

  addctrl("mrs_natural/advance", 0, ctrl_advance_);
  setctrlState("mrs_natural/advance", true);

  addctrl("mrs_bool/shuffle", false);
  setctrlState("mrs_bool/shuffle", true);

  addctrl("mrs_natural/cindex", 0);
  setctrlState("mrs_natural/cindex", true);

  addctrl("mrs_string/currentlyPlaying", "daufile", ctrl_currentlyPlaying_);
  addctrl("mrs_string/previouslyPlaying", "daufile", ctrl_previouslyPlaying_);
  addctrl("mrs_bool/regression", false, ctrl_regression_);
  addctrl("mrs_real/currentLabel", 0.0, ctrl_currentLabel_);
  addctrl("mrs_real/previousLabel", 0.0, ctrl_previousLabel_);
  addctrl("mrs_natural/nLabels", 0, ctrl_nLabels_);
  addctrl("mrs_string/labelNames", ",", ctrl_labelNames_);
  ctrl_mute_ = getctrl("mrs_bool/mute");

  addctrl("mrs_real/fullDuration", 0.);
  setctrlState("mrs_real/fullDuration", true);

  addctrl("mrs_natural/moveToSamplePos", 0);
  setctrlState("mrs_natural/moveToSamplePos", true);

  addctrl("mrs_bool/currentHasData", true, ctrl_currentHasData_);
  addctrl("mrs_bool/currentLastTickWithData", false, ctrl_currentLastTickWithData_);
  addctrl("mrs_bool/currentCollectionNewFile", true, ctrl_currentCollectionNewFile_);


  addctrl("mrs_bool/startStable", true, ctrl_startStable_);
  setctrlState("mrs_bool/startStable", true);
}

void
SoundFileSource::myUpdate(MarControlPtr sender)
{
  MRSDIAG("SoundFileSource::myUpdate");

  // Do nothing on output-only controls
  if (sender() == ctrl_hasData_())
  {
    return;
  }

  ctrl_inObsNames_->setValue("audio,", NOUPDATE);

  const string &new_filename = getctrl("mrs_string/filename")->to<mrs_string>();

  if (new_filename != filename_)
  {
    filename_ = new_filename;

    if (updateBackend())
    {
      getHeader();
      filename_ = ctrl_filename_->to<mrs_string>();
      ctrl_currentlyPlaying_->setValue(backend_->getctrl("mrs_string/currentlyPlaying"));
      ctrl_previouslyPlaying_->setValue(backend_->getctrl("mrs_string/previouslyPlaying"));
      ctrl_currentLabel_->setValue(backend_->getctrl("mrs_real/currentLabel"));
      ctrl_previousLabel_->setValue(backend_->getctrl("mrs_real/previousLabel"));

      ctrl_labelNames_->setValue(backend_->getctrl("mrs_string/labelNames"));
      ctrl_nLabels_->setValue(backend_->getctrl("mrs_natural/nLabels"));
      ctrl_onObservations_->setValue(backend_->ctrl_onObservations_, NOUPDATE);

      ostringstream oss;
      for (int ch = 0; ch < ctrl_onObservations_->to<mrs_natural>(); ch++)
      {
        oss << "AudioCh" << ch << ",";
      }
      ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);

      ctrl_israte_->setValue(backend_->ctrl_israte_, NOUPDATE);
      ctrl_osrate_->setValue(backend_->ctrl_osrate_, NOUPDATE);


      if (backend_->getctrl("mrs_natural/size")->to<mrs_natural>() != 0)
      {
        ctrl_hasData_->setValue(true);
        backend_->hasData_ = true;

        ctrl_lastTickWithData_->setValue(false, NOUPDATE);
        backend_->lastTickWithData_ =false;
      }
    }
    else
    {
      ctrl_onObservations_->setValue(1, NOUPDATE);
      ctrl_israte_->setValue((mrs_real)22050.0, NOUPDATE); //[!] why not set to 0 or some invalid value?
      ctrl_hasData_->setValue(false);
      ctrl_lastTickWithData_->setValue(true, NOUPDATE);
    }

  } else if(!sender.isInvalid()) {

    // Allow the user to jump to any given sample position. It appears to
    // work across collections as well.

    mrs_string name = sender->getName();
    if(!name.compare("mrs_natural/moveToSamplePos")) {

      mrs_natural newPos = sender->to<mrs_natural>();
      //cout << "Move to sample: " << newPos << endl;

      // getHeader() call seems to be the only way to
      // initialize the playback position.
      getHeader();

      ctrl_pos_->setValue(newPos, NOUPDATE);

      // from here you could run the common last section,
      // but this seems redundant, so for efficiency reasons we
      // return after making these (crucial) updates:

      backend_->setctrl("mrs_natural/pos", getctrl("mrs_natural/pos"));
      backend_->update();
      updControl("mrs_bool/hasData", backend_->hasData_);
      setctrl("mrs_bool/lastTickWithData", backend_->lastTickWithData_);
      return;
    }
  }




  if (backend_ != NULL)
  {

    //pass configuration to audio source object and update it
    backend_->ctrl_inSamples_->setValue(ctrl_inSamples_, NOUPDATE);
    backend_->ctrl_inObservations_->setValue(ctrl_inObservations_, NOUPDATE);
    backend_->ctrl_regression_->setValue(ctrl_regression_, NOUPDATE);
    backend_->setctrl("mrs_real/repetitions", getctrl("mrs_real/repetitions"));
    backend_->setctrl("mrs_real/duration", getctrl("mrs_real/duration"));
    backend_->setctrl("mrs_natural/advance", getctrl("mrs_natural/advance"));
    // src_->setctrl("mrs_natural/cindex", getctrl("mrs_natural/cindex"));
    backend_->setctrl("mrs_bool/shuffle", getctrl("mrs_bool/shuffle"));
    backend_->setctrl("mrs_bool/hasData", getctrl("mrs_bool/hasData"));
    backend_->setctrl("mrs_bool/lastTickWithData", getctrl("mrs_bool/lastTickWithData"));
    backend_->setctrl("mrs_natural/pos", getctrl("mrs_natural/pos"));
    backend_->pos_ = getctrl("mrs_natural/pos")->to<mrs_natural>();//[!]
    backend_->setctrl("mrs_natural/loopPos", getctrl("mrs_natural/loopPos"));
    backend_->rewindpos_ = getctrl("mrs_natural/loopPos")->to<mrs_natural>();//[!]
    backend_->update();

    //sync local controls with the controls from the audio source object
    ctrl_onSamples_->setValue(backend_->ctrl_onSamples_, NOUPDATE);
    ctrl_onObservations_->setValue(backend_->ctrl_onObservations_, NOUPDATE);
    ctrl_osrate_->setValue(backend_->ctrl_osrate_, NOUPDATE);

    setctrl("mrs_natural/pos", backend_->pos_);//[!]
    setctrl("mrs_natural/loopPos", backend_->rewindpos_);//[!]
    updControl("mrs_bool/hasData", backend_->hasData_);//[!]
    setctrl("mrs_bool/lastTickWithData", backend_->lastTickWithData_);
    setctrl("mrs_natural/size", backend_->getctrl("mrs_natural/size"));
    setctrl("mrs_real/repetitions", backend_->getctrl("mrs_real/repetitions"));
    setctrl("mrs_real/duration", backend_->getctrl("mrs_real/duration"));

    advance_ = ctrl_advance_->to<mrs_natural>();//?!?!!? [!][?]
    setctrl("mrs_natural/advance", backend_->getctrl("mrs_natural/advance"));

    setctrl("mrs_bool/shuffle", backend_->getctrl("mrs_bool/shuffle"));
    setctrl("mrs_natural/cindex", backend_->getctrl("mrs_natural/cindex"));
    setctrl("mrs_string/currentlyPlaying", backend_->getctrl("mrs_string/currentlyPlaying"));
    setctrl("mrs_string/previouslyPlaying", backend_->getctrl("mrs_string/previouslyPlaying"));
    setctrl("mrs_real/currentLabel", backend_->getctrl("mrs_real/currentLabel"));
    setctrl("mrs_real/previousLabel", backend_->getctrl("mrs_real/previousLabel"));
    setctrl("mrs_natural/nLabels", backend_->getctrl("mrs_natural/nLabels"));
    setctrl("mrs_string/labelNames", backend_->getctrl("mrs_string/labelNames"));
    setctrl("mrs_string/allfilenames", backend_->getctrl("mrs_string/allfilenames"));
    setctrl("mrs_natural/numFiles", backend_->getctrl("mrs_natural/numFiles"));

    if (backend_->getctrl("mrs_string/filetype")->to<mrs_string>() == "raw")
    {
      setctrl("mrs_real/frequency", backend_->getctrl("mrs_real/frequency"));
      setctrl("mrs_bool/noteon", backend_->getctrl("mrs_bool/noteon"));
    }
    else if (backend_->getctrl("mrs_string/filetype")->to<mrs_string>() == "mp3")
    {
      updateCurrDuration = true;
      //setctrl("mrs_real/fullDuration", src_->getctrl("mrs_real/fullDuration"));
      //std::cout<<"setting mrs/fullDuration to "<<src_->durFull_<<std::endl;
      //setctrl("mrs_real/fullDuration", src_->durFull_);
    }

  }

}

void SoundFileSource::clearBackend()
{
  delete backend_;
  backend_ = 0;
  file_extension_.clear();
}

bool
SoundFileSource::updateBackend()
{
  if (filename_.empty())
  {
    clearBackend();
    return false;
  }

  // check if file exists
  bool file_exists;
  {
    FILE *file = fopen(filename_.c_str(), "rb");
    file_exists = file != 0;
    if (file)
      fclose(file);
  }
  if (!file_exists)
  {
    clearBackend();
    MRSWARN("SoundFileSource: Failed to open file for reading: " << filename_);
    return false;
  }

  mrs_string extension;
  {
    mrs_string::size_type pos = filename_.rfind(".", filename_.length());
    if (pos != mrs_string::npos)
    {
      extension = filename_.substr(pos);
      // need this to be lowercase for the checks below.
      std::transform(extension.begin(), extension.end(), extension.begin(), (int(*)(int))tolower);
    }
  }

  if (file_extension_ == extension)
    return true;

  file_extension_ = extension;

  clearBackend();

  if (extension == ".au")
  {
    backend_ = new AuFileSource(getName());
  }
  else if (extension == ".wav")
  {
    backend_ = new WavFileSource(getName());
  }
  else if (extension == ".raw")
  {
    backend_ = new RawFileSource(getName());
  }
  else if (extension == ".txt")
  {
    backend_ = new CollectionFileSource(getName());
  }
  else if (extension == ".mf")
  {
    backend_ = new CollectionFileSource(getName());
  }
#ifdef MARSYAS_MAD
  else if (extension == ".mp3")
  {
    backend_ = new MP3FileSource(getName());
  }
#endif
#ifdef MARSYAS_VORBIS
  else if (extension == ".ogg")
  {
    backend_ = new OggFileSource(getName());
  }
#endif
  else
#ifdef MARSYAS_GSTREAMER
    // use GStreamer as a fallback
  {
    MRSDIAG("SoundFileSource is falling back to GStreamerSource\n");
    backend_ = new GStreamerSource(getName());
  }
#else
  {
    MRSWARN("Unknown file extension: " << filename_);
    return false;
  }
#endif

  return true;
}

void
SoundFileSource::getHeader()
{
  assert(!filename_.empty());
  assert(backend_ != 0);

  backend_->getHeader(filename_);
  ctrl_pos_->setValue(0, NOUPDATE);
  ctrl_loop_->setValue(0, NOUPDATE);
}

void
SoundFileSource::myProcess(realvec& in, realvec &out)
{
  if (backend_ != NULL)
  {
    backend_->process(in,out);
    // if (ctrl_mute_->isTrue())
    // out.setval(0.0);

#ifdef MARSYAS_MATLAB
#ifdef MTLB_DBG_LOG
    MATLAB_PUT(out, "out");
    MATLAB_EVAL("figure(41),subplot(212),plot(out'),axis('tight'),grid on, title('out')");
#endif
#endif

    /* setctrl("mrs_natural/pos", src_->pos_); //[!]
    setctrl("mrs_natural/loopPos", src_->rewindpos_);//[!]
    setctrl("mrs_bool/hasData", src_->hasData_);//[!]
    */
    // replaced by gtzan
    ctrl_pos_->setValue(backend_->getctrl("mrs_natural/pos")->to<mrs_natural>(), NOUPDATE);
    ctrl_loop_->setValue(backend_->rewindpos_, NOUPDATE);
    ctrl_hasData_->setValue(backend_->hasData_);
    ctrl_lastTickWithData_->setValue(backend_->lastTickWithData_, NOUPDATE);
    ctrl_currentlyPlaying_->setValue(backend_->getctrl("mrs_string/currentlyPlaying"));
    ctrl_previouslyPlaying_->setValue(backend_->getctrl("mrs_string/previouslyPlaying"));

    ctrl_currentLabel_->setValue(backend_->getctrl("mrs_real/currentLabel"));
    ctrl_previousLabel_->setValue(backend_->getctrl("mrs_real/previousLabel"));
    ctrl_labelNames_->setValue(backend_->getctrl("mrs_string/labelNames"));
    ctrl_nLabels_->setValue(backend_->getctrl("mrs_natural/nLabels"));

    if (updateCurrDuration)
    {
      setctrl("mrs_real/fullDuration", backend_->durFull_);
    }


    if (backend_->getType() == "CollectionFileSource") {
      CollectionFileSource *coll = (CollectionFileSource*)backend_;
      ctrl_currentHasData_->setValue(coll->iHasData_,
                                     NOUPDATE);

      ctrl_currentLastTickWithData_->setValue(coll->iLastTickWithData_, NOUPDATE);
      ctrl_currentCollectionNewFile_->setValue(coll->iNewFile_,
          NOUPDATE);
      if ( !(coll->iHasData_) ||
           (coll->iNewFile_)) {
        ctrl_startStable_->setValue((mrs_bool)true,
                                    NOUPDATE);
      } else {
        ctrl_startStable_->setValue((mrs_bool)false,
                                    NOUPDATE);
      }
    } else {
      ctrl_currentHasData_->setValue(backend_->hasData_);
      ctrl_currentLastTickWithData_->setValue(backend_->lastTickWithData_);
    }

    // MRSMSG("currentLastTickWithData_" << ctrl_currentLastTickWithData_->to<mrs_bool>())
    // MRSMSG("currentHasData" << ctrl_currentHasData_->to<mrs_bool>())
  }




  //used for toy_with_onsets.m (DO NOT DELETE! - COMMENT INSTEAD)
  //MATLAB_PUT(out, "SoundFileSource_out");
  //MATLAB_EVAL("srcAudio = [srcAudio, SoundFileSource_out];");


}
