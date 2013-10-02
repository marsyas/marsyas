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

#include "../common_source.h"
#include "AimVQ.h"

#ifdef MARSYAS_ANN
#include "ANN.h"
#include "static_vq_codebook.h"
#endif


using std::ostringstream;
using namespace Marsyas;

AimVQ::AimVQ(mrs_string name):MarSystem("AimVQ",name)
{
  is_initialized = false;
  addControls();
}

AimVQ::AimVQ(const AimVQ& a): MarSystem(a)
{
  is_initialized = false;

  ctrl_kd_tree_bucket_size_ = getctrl("mrs_natural/kd_tree_bucket_size");
  ctrl_kd_tree_error_bound_ = getctrl("mrs_real/kd_tree_error_bound");
  ctrl_num_codewords_to_return_ = getctrl("mrs_natural/num_codewords_to_return");
}

AimVQ::~AimVQ()
{


}

MarSystem*
AimVQ::clone() const
{
  return new AimVQ(*this);
}

void
AimVQ::addControls()
{
  addControl("mrs_natural/kd_tree_bucket_size", 50 , ctrl_kd_tree_bucket_size_);
  addControl("mrs_real/kd_tree_error_bound", 1.0 , ctrl_kd_tree_error_bound_);
  addControl("mrs_natural/num_codewords_to_return", 1 , ctrl_num_codewords_to_return_);
}

void
AimVQ::myUpdate(MarControlPtr sender)
{

  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("AimVQ.cpp - AimVQ:myUpdate");
  ctrl_onSamples_->setValue(1, NOUPDATE);
#ifdef MARSYAS_ANN
  ctrl_onObservations_->setValue(static_array_num_codebooks * static_array_num_codewords, NOUPDATE);
#endif
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);

  ostringstream oss;
  for (int i =0; i < ctrl_onObservations_->to<mrs_natural>(); ++i)
    oss << "attribute,";
  //ctrl_onObsNames_->setValue("AimVQ_" + ctrl_inObsNames_->to<mrs_string>() , NOUPDATE);
  ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);


  //
  // Does the MarSystem need initialization?
  //
  if (!is_initialized) {
    InitializeInternal();
    is_initialized = true;
  }


}

void
AimVQ::InitializeInternal() {
#ifdef MARSYAS_ANN
  codebooks_count_ = static_array_num_codebooks;  // Your value here
  codeword_count_ = static_array_num_codewords;  // Your value here
  codeword_length_ = static_array_codeword_length;  // Your value here

  // Fill the ANN points arrays with the codebooks
  for (int i = 0; i < codebooks_count_; ++i) {
    ANNpointArray points = annAllocPts(codeword_count_,codeword_length_);
    // The points arrays are stored here for later deallocation
    codebooks_.push_back(points);
    int index = 0;
    for (int j = 0; j < codeword_count_; ++j) {
      for (int k = 0; k < codeword_length_; ++k) {
        (*points)[index] = static_vq_array[i][j][k];  // take the value from your array here
        ++index;
      }
    }
    ANNkd_tree* kd_tree = new ANNkd_tree(points,
                                         codeword_count_,
                                         codeword_length_,
                                         ctrl_kd_tree_bucket_size_->to<mrs_natural>());
    sparse_coder_trees_.push_back(kd_tree);
  }
#endif
}


void
AimVQ::myProcess(realvec& in, realvec& out)
{
  // cout << "AimVQ::myProcess" << endl;

  (void) in; // avoid unused parameter warning when MARSYAS_ANN is not enabled


  // Zero out the output first
  for (int i = 0; i < onObservations_; ++i) {
    out(i,0) = 0.0;
  }

#ifdef MARSYAS_ANN
  mrs_natural _num_codewords_to_return = ctrl_num_codewords_to_return_->to<mrs_natural>();
  mrs_real _kd_tree_error_bound = ctrl_kd_tree_error_bound_->to<mrs_real>();

  vector<int> sparse_code;
  int offset = 0;
  realvec obsRow;
  for (int i = 0; i < codebooks_count_; ++i) {
    in.getRow(i,obsRow);
    ANNidxArray indices = new ANNidx[_num_codewords_to_return];
    ANNdistArray distances = new ANNdist[_num_codewords_to_return];
    sparse_coder_trees_[i]->annkSearch(obsRow.getData(),
                                       _num_codewords_to_return,
                                       indices,
                                       distances,
                                       _kd_tree_error_bound);
    for (int j = 0; j < _num_codewords_to_return; ++j) {
      sparse_code.push_back(indices[j] + offset);
    }
    offset += codeword_count_;

    delete indices;
    delete distances;
  }

  for (unsigned int j = 0; j < sparse_code.size(); ++j) {
    out(sparse_code[j],0) = 1.0;
  }
#endif

}
