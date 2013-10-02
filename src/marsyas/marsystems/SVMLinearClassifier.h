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

#ifndef MARSYAS_SVMCLASSIFIER_H
#define MARSYAS_SVMCLASSIFIER_H

#include <marsyas/system/MarSystem.h>
#include "svm.h"
#include "linear.h"
#include <marsyas/WekaData.h>

namespace Marsyas
{
/**
	\ingroup MachineLearning
	\brief SVM classifier based on libsvm
*/

class SVMClassifier: public MarSystem
{
private:
  MarControlPtr ctrl_sv_coef_ ;
  MarControlPtr ctrl_sv_indices_ ;
  MarControlPtr ctrl_SV_ ;
  MarControlPtr ctrl_rho_ ;
  MarControlPtr ctrl_probA_ ;
  MarControlPtr ctrl_probB_ ;
  MarControlPtr ctrl_label_ ;
  MarControlPtr ctrl_nSV_ ;
  MarControlPtr ctrl_nr_class_ ;
  MarControlPtr ctrl_weight_;
  MarControlPtr ctrl_weight_label_;
  MarControlPtr ctrl_minimums_;
  MarControlPtr ctrl_maximums_;
  MarControlPtr ctrl_mode_;
  MarControlPtr ctrl_nClasses_;
  MarControlPtr ctrl_l_;
  MarControlPtr ctrl_svm_;
  MarControlPtr ctrl_kernel_;
  MarControlPtr ctrl_degree_;
  MarControlPtr ctrl_gamma_;
  MarControlPtr ctrl_coef0_;
  MarControlPtr ctrl_nu_;
  MarControlPtr ctrl_cache_size_;
  MarControlPtr ctrl_C_;
  MarControlPtr ctrl_eps_;
  MarControlPtr ctrl_p_;
  MarControlPtr ctrl_shrinking_;
  MarControlPtr ctrl_probability_;
  MarControlPtr ctrl_nr_weight_;
  MarControlPtr ctrl_classPerms_;

  std::vector<int> classPerms_;


  void addControls();
  void myUpdate(MarControlPtr sender);

  WekaData instances_;
  struct svm_problem svm_prob_;
  struct svm_parameter svm_param_;
  struct svm_model *svm_model_;
  mrs_bool trained_, training_, was_training_;
  mrs_natural kernel_, svm_;

  // to clean-up memory leaks
  mrs_natural num_nodes;
  void ensure_free_svm_model();
  void ensure_free_svm_prob_xy();
  mrs_natural num_svm_prob_x;

public:
  SVMClassifier(std::string name);
  SVMClassifier(const SVMClassifier& a);
  ~SVMClassifier();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}

#endif
