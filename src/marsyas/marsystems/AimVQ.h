/*
** Copyright (C) 1998-2011 George Tzanetakis <gtzan@cs.uvic.ca>
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

#ifndef MARSYAS_AIMVQ_H
#define MARSYAS_AIMVQ_H

#include <marsyas/system/MarSystem.h>

#ifdef MARSYAS_ANN
#include "ANN.h"
#include "kd_tree.h"
#endif

// sness - TODO - Eventually make these realvecs.  However, in the
// existing code there is quite a bit of stuff that involves STL
// methods, so I thought it would be safer to use STL for now.
#include <vector>

namespace Marsyas
{
/**
   \class AimVQ
   \ingroup Analysis
   \brief Vector quantization for dense to sparse features

   Author : Thomas Walters <tom@acousticscale.org>

   Ported to Marsyas by Steven Ness <sness@sness.net>

   The original source code for these functions in AIM-C can be found at:
   http://code.google.com/p/aimc/

*/


class AimVQ: public MarSystem
{
private:
  void myUpdate(MarControlPtr sender);

  // Prepare the module
  void InitializeInternal();

  // Does the MarSystem need initialization?
  bool is_initialized;

  // What changes cause it to need initialization?
  mrs_real initialized_israte;

  // Controls
  MarControlPtr ctrl_kd_tree_bucket_size_;
  MarControlPtr ctrl_kd_tree_error_bound_;
  MarControlPtr ctrl_num_codewords_to_return_;

  float sample_rate_;
  int buffer_length_;
  int channel_count_;

#ifdef MARSYAS_ANN
  vector<ANNkd_tree*> sparse_coder_trees_;
  vector<ANNpointArray> codebooks_;
#endif

  int codebooks_count_;
  int codeword_count_;
  int codeword_length_;

public:
  AimVQ(std::string name);
  AimVQ(const AimVQ& a);

  ~AimVQ();
  MarSystem* clone() const;
  void addControls();

  void myProcess(realvec& in, realvec& out);
};

}//namespace marsyas

#endif
