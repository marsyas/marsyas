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

#include "AimBoxes.h"
#include "../common_source.h"


using std::ostringstream;
using std::vector;

using namespace Marsyas;

AimBoxes::AimBoxes(mrs_string name):MarSystem("AimBoxes",name)
{
  is_initialized = false;
  initialized_israte = 0.0;
  initialized_inobservations = 0;
  initialized_insamples = 0;
  initialized_box_size_spectral = 0;
  initialized_box_size_temporal = 0;

  is_reset = false;
  addControls();
}

AimBoxes::AimBoxes(const AimBoxes& a): MarSystem(a)
{
  is_initialized = false;
  initialized_israte = 0.0;
  initialized_inobservations = 0;
  initialized_insamples = 0;
  initialized_box_size_spectral = 0;
  initialized_box_size_temporal = 0;

  is_reset = false;

  ctrl_box_size_spectral_= getctrl("mrs_natural/box_size_spectral");
  ctrl_box_size_temporal_= getctrl("mrs_natural/box_size_temporal");
}

AimBoxes::~AimBoxes()
{
}

MarSystem*
AimBoxes::clone() const
{
  return new AimBoxes(*this);
}

void
AimBoxes::addControls()
{
  addControl("mrs_natural/box_size_spectral", 16 , ctrl_box_size_spectral_);
  addControl("mrs_natural/box_size_temporal", 32 , ctrl_box_size_temporal_);
}

void
AimBoxes::myUpdate(MarControlPtr sender)
{

  // sness - We calculate box_count_ and feature_size_ inside
  // InitializeInternal, so in this MarSystem we first of all call
  // InitializeInternal, and then update.  We might want to use the
  // same pattern in the other Aim modules

  //
  // Does the MarSystem need initialization?
  //
  if (initialized_israte != ctrl_israte_->to<mrs_real>() ||
      initialized_inobservations != ctrl_inObservations_->to<mrs_natural>() ||
      initialized_insamples != ctrl_inSamples_->to<mrs_natural>() ||
      initialized_box_size_spectral != ctrl_box_size_spectral_->to<mrs_natural>() ||
      initialized_box_size_temporal != ctrl_box_size_temporal_->to<mrs_natural>()) {
    is_initialized = false;
  }

  if (!is_initialized) {
    InitializeInternal();
    is_initialized = true;
    initialized_israte = ctrl_israte_->to<mrs_real>();
    initialized_inobservations = ctrl_inObservations_->to<mrs_natural>();
    initialized_insamples = ctrl_inSamples_->to<mrs_natural>();
    initialized_box_size_spectral = ctrl_box_size_spectral_->to<mrs_natural>();
    initialized_box_size_temporal = ctrl_box_size_temporal_->to<mrs_natural>();
  }

  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("AimBoxes.cpp - AimBoxes:myUpdate");
  // ctrl_onSamples_->setValue(box_count_, NOUPDATE);
  // ctrl_onObservations_->setValue(feature_size_, NOUPDATE);
  ctrl_onSamples_->setValue(feature_size_, NOUPDATE);
  ctrl_onObservations_->setValue(box_count_, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
  ctrl_onObsNames_->setValue("AimBoxes_" + ctrl_inObsNames_->to<mrs_string>() , NOUPDATE);

  // //
  // // Does the MarSystem need a reset?
  // //
  // if (reset_inobservations != ctrl_inObservations_->to<mrs_natural>()) {
  //   is_reset = false;
  // }

  // if (!is_reset) {
  //   ResetInternal();
  //   is_reset = true;
  //   reset_inobservations = ctrl_inObservations_->to<mrs_natural>();
  // }

}

void
AimBoxes::InitializeInternal() {
  // mrs_real sample_rate = ctrl_israte_->to<mrs_real>();
  mrs_natural buffer_length = ctrl_inSamples_->to<mrs_natural>();
  mrs_natural channel_count = ctrl_inObservations_->to<mrs_natural>();
  mrs_natural channels_height = ctrl_box_size_spectral_->to<mrs_natural>();
  mrs_natural temporal_width = ctrl_box_size_temporal_->to<mrs_natural>();

  // If these aren't initialized yet, then the MarSystem hasn't
  // updated, so just return.
  if ((channels_height == 0) || (temporal_width == 0)) {
    return;
  }

  box_limits_channels_.clear();
  box_limits_time_.clear();

  while (channels_height < channel_count / 2) {
    int top = channel_count - 1;
    while (top - channels_height >= 0) {
      box_limits_channels_.push_back(std::make_pair(top,
                                     top - channels_height));
      top -= channels_height / 2;
    }
    channels_height *= 2;
  }

  while (temporal_width < buffer_length) {
    box_limits_time_.push_back(temporal_width);
    temporal_width *= 2;
  }

  box_count_ = (int) (box_limits_time_.size() * box_limits_channels_.size());
  feature_size_ = ctrl_box_size_spectral_->to<mrs_natural>() + ctrl_box_size_temporal_->to<mrs_natural>();

}

void
AimBoxes::ResetInternal() {
}

void
AimBoxes::myProcess(realvec& in, realvec& out)
{
  mrs_natural box_size_temporal = ctrl_box_size_temporal_->to<mrs_natural>();
  mrs_natural box_size_spectral = ctrl_box_size_spectral_->to<mrs_natural>();

  int box_index = 0;
  for (int c = 0; c < static_cast<int>(box_limits_channels_.size()); ++c) {
    for (int s = 0; s < static_cast<int>(box_limits_time_.size()); ++s) {
      int pixel_size_channels = (box_limits_channels_[c].first
                                 - box_limits_channels_[c].second)
                                / box_size_spectral;
      int pixel_size_samples = box_limits_time_[s] / box_size_temporal;
      vector<vector<float> > box;
      vector<float> line;
      line.resize(box_size_temporal, 0.0f);
      box.resize(box_size_spectral, line);
      for (int i = 0; i < box_size_spectral; ++i) {
        for (int j = 0; j < box_size_temporal; ++j) {
          float pixel_value = 0.0f;
          for (int k = i * pixel_size_channels; k < (i + 1) * pixel_size_channels; ++k) {
            for (int l = j * pixel_size_samples; l < (j + 1) * pixel_size_samples; ++l) {
              pixel_value += (float) in(k + box_limits_channels_[c].second, l);
              // pixel_value += input.sample(k, l);
            }
          }
          pixel_value /= (float) (pixel_size_channels * pixel_size_samples);
          box[i][j] = pixel_value;
        }
      }
      int feature_index = 0;
      for (int i = 0; i < box_size_spectral; ++i) {
        float feature_value = 0.0f;
        for (int j = 0; j < box_size_temporal; ++j) {
          feature_value += box[i][j];
        }
        feature_value /= (float)box_size_temporal;
        // output_.set_sample(box_index, feature_index, feature_value);
        out(box_index, feature_index) = feature_value;
        // output_.set_sample(box_index, feature_index, feature_value);
        ++feature_index;
      }
      for (int j = 0; j < box_size_temporal; ++j) {
        float feature_value = 0.0f;
        for (int i = 0; i < box_size_spectral; ++i) {
          feature_value += box[i][j];
        }
        feature_value /= (float) box_size_spectral;
        out(box_index,feature_index) = feature_value;
        // output_.set_sample(box_index, feature_index, feature_value);
        ++feature_index;
      }
      ++box_index;
    }
  }

}
