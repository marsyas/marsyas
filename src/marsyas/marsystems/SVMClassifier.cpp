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

#include "../common_source.h"
#include "SVMClassifier.h"
#define Malloc(type,n) (type *)malloc((n)*sizeof(type))

using namespace std;
using namespace Marsyas;

SVMClassifier::SVMClassifier(mrs_string name) :
  MarSystem("SVMClassifier", name) {
  training_ = true;
  was_training_ = false;
  trained_ = false;
  kernel_ = LINEAR;
  svm_ = C_SVC;
  svm_model_ = NULL;
  num_nodes = 0;
  svm_prob_.y = NULL;
  svm_prob_.x = NULL;

  addControls();
}

SVMClassifier::SVMClassifier(const SVMClassifier& a) :
  MarSystem(a) {
  training_ = true;
  was_training_ = false;
  trained_ = false;
  kernel_ = LINEAR;
  svm_ = C_SVC;
  svm_model_ = NULL;
  num_nodes = 0;
  svm_prob_.y = NULL;
  svm_prob_.x = NULL;

  ctrl_nClasses_ = getctrl("mrs_natural/nClasses");
  ctrl_sv_coef_ = getctrl("mrs_realvec/sv_coef");
  ctrl_sv_indices_ = getctrl("mrs_realvec/sv_indices");
  ctrl_SV_ = getctrl("mrs_realvec/SV");
  ctrl_rho_ = getctrl("mrs_realvec/rho");
  ctrl_probA_ = getctrl("mrs_realvec/probA");
  ctrl_probB_ = getctrl("mrs_realvec/probB");
  ctrl_label_ = getctrl("mrs_realvec/label");
  ctrl_nSV_ = getctrl("mrs_realvec/nSV");
  ctrl_nr_class_ = getctrl("mrs_natural/nr_class");
  ctrl_weight_ = getctrl("mrs_realvec/weight");
  ctrl_weight_label_ = getctrl("mrs_realvec/weight_label");
  ctrl_minimums_ = getctrl("mrs_realvec/minimums");
  ctrl_maximums_ = getctrl("mrs_realvec/maximums");
  ctrl_mode_ = getctrl("mrs_string/mode");
  ctrl_l_ = getctrl("mrs_natural/l");
  ctrl_svm_ = getctrl("mrs_string/svm");
  ctrl_kernel_ = getctrl("mrs_string/kernel");
  ctrl_degree_ = getctrl("mrs_natural/degree");
  ctrl_gamma_ = getctrl("mrs_natural/gamma");
  ctrl_coef0_ = getctrl("mrs_natural/coef0");
  ctrl_nu_ = getctrl("mrs_real/nu");
  ctrl_cache_size_ = getctrl("mrs_natural/cache_size");
  ctrl_C_ = getctrl("mrs_real/C");
  ctrl_eps_ = getctrl("mrs_real/eps");
  ctrl_p_ = getctrl("mrs_real/p");
  ctrl_shrinking_ = getctrl("mrs_bool/shrinking");
  ctrl_probability_ = getctrl("mrs_bool/probability");
  ctrl_nr_weight_ = getctrl("mrs_natural/nr_weight");
  ctrl_classPerms_ = getctrl("mrs_realvec/classPerms");

}

SVMClassifier::~SVMClassifier() {
  ensure_free_svm_model();
  ensure_free_svm_prob_xy();
}

void SVMClassifier::ensure_free_svm_model() {
  if (svm_model_ != NULL)
  {
    svm_free_and_destroy_model(&svm_model_);
  }
}

void SVMClassifier::ensure_free_svm_prob_xy() {
  if (svm_prob_.x != NULL) {
    for (int i=0; i < num_svm_prob_x; ++i) {
      if (svm_prob_.x[i] != NULL) {
        delete [] svm_prob_.x[i];
        svm_prob_.x[i] = NULL;
      }
    }
    delete [] svm_prob_.x;
    svm_prob_.x = NULL;
  }
  if (svm_prob_.y != NULL) {
    delete [] svm_prob_.y;
    svm_prob_.y = NULL;
  }
}

MarSystem* SVMClassifier::clone() const {
  return new SVMClassifier(*this);
}

void SVMClassifier::addControls() {
  addctrl("mrs_string/mode", "train", ctrl_mode_);
  setctrlState("mrs_string/mode", true);

  addctrl("mrs_natural/nClasses", 1, ctrl_nClasses_);
  setctrlState("mrs_natural/nClasses", true);

  addctrl("mrs_realvec/minimums", realvec(), ctrl_minimums_);
  addctrl("mrs_realvec/maximums", realvec(), ctrl_maximums_);
  addctrl("mrs_realvec/sv_coef", realvec(), ctrl_sv_coef_);
  addctrl("mrs_realvec/sv_indices", realvec(), ctrl_sv_indices_);
  addctrl("mrs_realvec/SV", realvec(), ctrl_SV_);
  addctrl("mrs_realvec/rho", realvec(), ctrl_rho_);
  addctrl("mrs_realvec/probA", realvec(), ctrl_probA_);
  addctrl("mrs_realvec/probB", realvec(), ctrl_probB_);
  addctrl("mrs_realvec/label", realvec(), ctrl_label_);
  addctrl("mrs_realvec/nSV", realvec(), ctrl_nSV_);
  addctrl("mrs_natural/nr_class", (mrs_natural)0, ctrl_nr_class_);
  addctrl("mrs_natural/l", (mrs_natural)0, ctrl_l_);
  addctrl("mrs_realvec/weight_label", realvec(), ctrl_weight_label_);
  addctrl("mrs_realvec/weight", realvec(), ctrl_weight_);
  addctrl("mrs_string/svm", "C_SVC", ctrl_svm_);
  setctrlState("mrs_string/svm", true);
  addctrl("mrs_string/kernel", "LINEAR", ctrl_kernel_);;
  setctrlState("mrs_string/kernel", true);
  addctrl("mrs_natural/degree", (mrs_natural)3, ctrl_degree_);
  addctrl("mrs_natural/gamma", (mrs_natural)4, ctrl_gamma_);
  addctrl("mrs_natural/coef0", (mrs_natural)0, ctrl_coef0_);
  addctrl("mrs_real/nu", (mrs_real)0.5, ctrl_nu_);
  addctrl("mrs_natural/cache_size", (mrs_natural)100, ctrl_cache_size_);
  addctrl("mrs_real/C", (mrs_real)1.0, ctrl_C_);
  addctrl("mrs_real/eps", (mrs_real)0.001, ctrl_eps_);
  addctrl("mrs_real/p", (mrs_real)0.1, ctrl_p_);
  addctrl("mrs_bool/shrinking", true, ctrl_shrinking_);
  addctrl("mrs_bool/probability", true, ctrl_probability_);
  addctrl("mrs_natural/nr_weight", (mrs_natural)0, ctrl_nr_weight_);
  addctrl("mrs_realvec/classPerms", realvec(), ctrl_classPerms_);

  // turn off for regression
  addctrl("mrs_bool/output_classPerms", true);
}

void SVMClassifier::myUpdate(MarControlPtr sender) {
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("SVMClassifier.cpp - SVMClassifier:myUpdate");

  ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
  mrs_natural nClasses = getctrl("mrs_natural/nClasses")->to<mrs_natural>();
  ctrl_onObservations_->setValue(2 + nClasses, NOUPDATE);

  if (ctrl_mode_->to<mrs_string>() == "train") {
    training_ = true;
  } else if (ctrl_mode_->to<mrs_string>() == "predict") {
    training_ = false;
  } else {
    cerr << "SVMClassifier.cpp, mode not supported"<<endl;
    exit(1);
  }


  if (ctrl_svm_->to<mrs_string>() == "C_SVC")
    svm_ = C_SVC;
  else if (ctrl_svm_->to<mrs_string>() == "ONE_CLASS")
    svm_ = ONE_CLASS;
  else if (ctrl_svm_->to<mrs_string>() == "EPSILON_SVR")
    svm_ = EPSILON_SVR;
  else if (ctrl_svm_->to<mrs_string>() == "NU_SVR")
    svm_ = NU_SVR;
  else
  {
    cerr << "SVMClassifier.cpp, SVM not supported"<<endl;
    exit(1);
  }

  if (ctrl_kernel_->to<mrs_string>() == "LINEAR")
    kernel_ = LINEAR;
  else if (ctrl_kernel_->to<mrs_string>() == "POLY")
    kernel_ = POLY;
  else if (ctrl_kernel_->to<mrs_string>() == "RBF")
    kernel_ = RBF;
  else if (ctrl_kernel_->to<mrs_string>() == "SIGMOID")
    kernel_ = SIGMOID;
  else if (ctrl_kernel_->to<mrs_string>() == "PRECOMPUTED")
    kernel_ = PRECOMPUTED;
  else
  {
    cerr << "SVMClassifier.cpp, kernel not supported"<<endl;
    exit(1);
  }

  if (!training_) {
    if (!trained_ && was_training_) {

      // When network is switched from "train" mode to "predict",
      // we process the data instances which have been stored
      // in WekaData and pass them onto libsvm classes for actual training.

      svm_param_.svm_type = svm_;
      svm_param_.kernel_type = kernel_;
      svm_param_.degree = ctrl_degree_->to<mrs_natural>();
      svm_param_.gamma = ctrl_gamma_->to<mrs_natural>();
      svm_param_.coef0 = ctrl_coef0_->to<mrs_natural>();
      svm_param_.nu = ctrl_nu_->to<mrs_real>();
      svm_param_.cache_size = ctrl_cache_size_->to<mrs_natural>();
      svm_param_.C = ctrl_C_->to<mrs_real>();
      svm_param_.eps = ctrl_eps_->to<mrs_real>();
      svm_param_.p = ctrl_p_->to<mrs_real>();
      svm_param_.shrinking = ctrl_shrinking_->to<mrs_bool>();
      svm_param_.probability = ctrl_probability_->to<mrs_bool>();
      svm_param_.nr_weight = ctrl_nr_weight_->to<mrs_natural>();

      if (svm_param_.nr_weight) {
        svm_param_.weight_label = Malloc(int,svm_param_.nr_weight);
        svm_param_.weight = Malloc(double,svm_param_.nr_weight);
        for (int i=0; i < svm_param_.nr_weight-1; ++i) {
          svm_param_.weight_label[i]
          = (int)ctrl_weight_label_->to<realvec>()(i);
          svm_param_.weight[i]
          = (double)ctrl_weight_->to<realvec>()(i);
        }
      } else {
        svm_param_.weight_label = NULL;
        svm_param_.weight = NULL;
      }

      // normalize data
      instances_.NormMaxMin();

      // transfer training data instances into svm_problem
      mrs_natural nInstances = instances_.getRows();
      svm_prob_.l = nInstances;

      ensure_free_svm_prob_xy();
      svm_prob_.y = new double[svm_prob_.l];
      svm_prob_.x = new svm_node*[nInstances];
      num_svm_prob_x = nInstances;

      for (int i=0; i < nInstances; ++i) {
        svm_prob_.x[i] = NULL;
      }
      int l;
      mrs_bool seen;

      for (int i=0; i < nInstances; ++i)
      {
        // set class (as number) for each of the instances
        l = instances_.GetClass(i);
        svm_prob_.y[i] = l;

        // store all distinct classes in classPerms_
        seen = false;
        for (size_t j=0; j < classPerms_.size(); j++)
        {
          if (l == classPerms_[j])
            seen = true;
        }
        if (!seen)
          classPerms_.push_back(l);
      }


      {
        MarControlAccessor acc_classPerms(ctrl_classPerms_);
        realvec& classPerms = acc_classPerms.to<mrs_realvec>(); // ?
        classPerms.create((mrs_natural)classPerms_.size());

        for (mrs_natural i=0; i < (mrs_natural)classPerms_.size(); ++i)
        {
          classPerms(i) = classPerms_[i];
        }
      }

      // load each instance data into svm_nodes and store in svm_problem
      for (int i=0; i < nInstances; ++i) {
        svm_prob_.x[i] = new svm_node[inObservations_];
        for (int j=0; j < inObservations_; j++) {
          if (j < inObservations_ -1) {
            svm_prob_.x[i][j].index = j+1;
            svm_prob_.x[i][j].value = instances_.at(i)->at(j);
          } else {
            svm_prob_.x[i][j].index = -1;
            svm_prob_.x[i][j].value = 0.0;
          }
        }
      }

      const char *error_msg;
      error_msg = svm_check_parameter(&svm_prob_, &svm_param_);
      if (error_msg) {
        cerr << "SVMClassifier.cpp libsvm error: " << error_msg
             << endl;
        exit(1);
      }

      ensure_free_svm_model();
      svm_model_ = svm_train(&svm_prob_, &svm_param_);

      trained_ = true;

      MRSDEBUG ("SVMCLassifier train ... done");
      MRSDEBUG ("svm_model_->nr_class = " << svm_model_->nr_class);
      MRSDEBUG ("svm_model_->l = " << svm_model_->l);
      MRSDEBUG ("svm_model_->free_sv = " << svm_model_->free_sv);
      MRSDEBUG ("svm_model_->SV = " << svm_model_->SV);

      int n = 0;


      /////// expose min / max boundary values /////////
      ctrl_minimums_->setValue(instances_.GetMinimums(), NOUPDATE);
      ctrl_maximums_->setValue(instances_.GetMaximums(), NOUPDATE);


      ///////  expose sv_coef and SV ////////
      MarControlAccessor acc_sv_coef(ctrl_sv_coef_, NOUPDATE);
      realvec& sv_coef = acc_sv_coef.to<mrs_realvec>();
      MarControlAccessor acc_sv_indices(ctrl_sv_indices_, NOUPDATE);
      realvec& sv_indices = acc_sv_indices.to<mrs_realvec>();
      MarControlAccessor acc_SV(ctrl_SV_, NOUPDATE);
      realvec& SV = acc_SV.to<mrs_realvec>();
      n = svm_model_->l;
      sv_coef.stretch(svm_model_->nr_class-1,n);
      sv_indices.stretch(n);
      SV.stretch(n, (inObservations_-1));

      for (int i=0; i<n; ++i) {
        for (int j=0; j<svm_model_->nr_class-1; j++) // for every class
          sv_coef(j, i)=svm_model_->sv_coef[j][i]; // copy coeff to sv_coef MarControl
        const svm_node *p = svm_model_->SV[i];
        int ind = 0;
        while (p->index != -1) { // for every observation in the vector
          SV(i, ind)=p->value; // copy to SV MarControl
          p++;
          ind++;
        }
      }

      ///////  expose rho, probA, probB, label, nSV, nr_class, l ////////

      // rho
      {
        MarControlAccessor acc_rho(ctrl_rho_, NOUPDATE);
        realvec& rho = acc_rho.to<mrs_realvec>();
        n = svm_model_->nr_class*(svm_model_->nr_class-1)/2;
        rho.stretch(n);
        for (int i=0; i<n; ++i)
          rho(i)=svm_model_->rho[i];
      }

      // probA
      if (svm_model_->probA) {
        MarControlAccessor acc_probA(ctrl_probA_, NOUPDATE);
        realvec& probA = acc_probA.to<mrs_realvec>();
        n = svm_model_->nr_class*(svm_model_->nr_class-1)/2;
        probA.stretch(n);
        for (int i=0; i<n; ++i)
          probA(i)=svm_model_->probA[i];
      }

      // probB
      if (svm_model_->probB) {
        MarControlAccessor acc_probB(ctrl_probB_, NOUPDATE);
        realvec& probB = acc_probB.to<mrs_realvec>();
        n = svm_model_->nr_class*(svm_model_->nr_class-1)/2;
        probB.stretch(n+1);
        for (int i=0; i<n; ++i)
          probB(i)=svm_model_->probB[i];
      }

      // label
      if (svm_model_->label) {
        MarControlAccessor acc_label(ctrl_label_, NOUPDATE);
        realvec& label = acc_label.to<mrs_realvec>();
        n = svm_model_->nr_class;
        label.stretch(n);
        for (int i=0; i<n; ++i)
          label(i)=svm_model_->label[i];
      }

      // nSV
      if (svm_model_->nSV) {
        MarControlAccessor acc_nSV(ctrl_nSV_, NOUPDATE);
        realvec& nSV = acc_nSV.to<mrs_realvec>();
        n = svm_model_->nr_class;
        nSV.stretch(n);
        for (int i=0; i<n; ++i)
          nSV(i)=svm_model_->nSV[i];
      }

//			if (svm_model_->nSV) {
//				n = svm_model_->nr_class;
//				realvec nSV(n);
//				for (int i=0; i<n; ++i)
//					nSV(i)=svm_model_->nSV[i];
//
//				ctrl_nSV_->setValue(nSV, NOUPDATE);
//			}

      // nr_class
      ctrl_nr_class_->setValue(svm_model_->nr_class, NOUPDATE);

      // l
      ctrl_l_->setValue(svm_model_->l, NOUPDATE);
    }
  }
}





void SVMClassifier::myProcess(realvec& in, realvec& out)
{

  if (training_) {
    // Training here means simply inserting all input vectors to
    // the WekaData instances_. The actual training of libsvm classes
    // happens when we update the mode control to "predict".

    if (!was_training_) {
      instances_.Create(inObservations_);
      trained_ = false;

    }

    instances_.Append(in);
    out(0,0) = in(inObservations_-1, 0);
    out(1,0) = in(inObservations_-1, 0);


  } else {  // predict

    if (!trained_) {
      if (was_training_) {
        ;
      } else {
        // Init libsvm structures and load data from
        // network controls into libsvm in cased they had been stored

        svm_prob_.y = NULL;
        svm_prob_.x = NULL;
        svm_model_ = Malloc(svm_model,1);
        svm_model_->param.svm_type = svm_;
        svm_model_->param.weight_label = NULL;
        svm_model_->param.weight = NULL;
        svm_model_->param.kernel_type = kernel_;
        svm_model_->param.degree = ctrl_degree_->to<mrs_natural>();
        svm_model_->param.gamma = ctrl_gamma_->to<mrs_natural>();
        svm_model_->param.coef0 = ctrl_coef0_->to<mrs_natural>();
        svm_model_->param.nu = ctrl_nu_->to<mrs_real>();
        svm_model_->param.cache_size = ctrl_cache_size_->to<mrs_natural>();
        svm_model_->param.C = ctrl_C_->to<mrs_real>();
        svm_model_->param.eps = ctrl_eps_->to<mrs_real>();
        svm_model_->param.p = ctrl_p_->to<mrs_real>();
        svm_model_->param.shrinking = ctrl_shrinking_->to<mrs_bool>();
        svm_model_->param.probability = ctrl_probability_->to<mrs_bool>();
        svm_model_->param.nr_weight = ctrl_nr_weight_->to<mrs_natural>();

        {
          MarControlAccessor acc_classPerms(ctrl_classPerms_);
          realvec& classPerms = acc_classPerms.to<mrs_realvec>();
          classPerms_.clear();
          for (mrs_natural i=0; i < classPerms.getSize(); ++i)
          {
            classPerms_.push_back((mrs_natural)classPerms(i));
          }
        }

        MRSDEBUG ("svm_model_->param.svm_type = " << svm_model_->param.svm_type);
        MRSDEBUG ("svm_model_->param.kernel_type = " << svm_model_->param.kernel_type);
        MRSDEBUG ("svm_model_->param.degree = " << svm_model_->param.degree);
        MRSDEBUG ("svm_model_->param.gamma = " << svm_model_->param.gamma);
        MRSDEBUG ("svm_model_->param.coef0 = " << svm_model_->param.coef0);
        MRSDEBUG ("svm_model_->param.nu = " << svm_model_->param.nu);
        MRSDEBUG ("svm_model_->param.cache_size = " << svm_model_->param.cache_size);
        MRSDEBUG ("svm_model_->param.C = " << svm_model_->param.C);
        MRSDEBUG ("svm_model_->param.eps = " << svm_model_->param.eps);
        MRSDEBUG ("svm_model_->param.p = " << svm_model_->param.p);
        MRSDEBUG ("svm_model_->param.shrinking = " << svm_model_->param.shrinking);
        MRSDEBUG ("svm_model_->param.probability = " << svm_model_->param.probability);
        MRSDEBUG ("svm_model_->param.nr_weight = " << svm_model_->param.nr_weight);
        MRSDEBUG ("svm_model_->param.weight_label = " << svm_model_->param.weight_label);
        MRSDEBUG ("svm_model_->param.weight = " << svm_model_->param.weight);

        int n = ctrl_nr_class_->to<mrs_natural>();
        int l = ctrl_l_->to<mrs_natural>();
        int m = n*(n-1)/2;

        svm_model_->nr_class = n;
        svm_model_->l = l;

        MRSDEBUG ("svm_model_->nr_class = " << svm_model_->nr_class);
        MRSDEBUG ("svm_model_->l = " << svm_model_->l);



        ////////// Load data from previous training into libsvm if exists /////////////

        if (ctrl_rho_->to<realvec>().getSize()) //rho
        {
          svm_model_->rho = Malloc(double,m);
          for (int i=0; i<m; ++i)
            svm_model_->rho[i]= ctrl_rho_->to<realvec>()(i);
        }
        else
          svm_model_->rho = NULL;

        if (ctrl_probA_->to<realvec>().getSize()) //probA
        {
          svm_model_->probA = Malloc(double,m);
          for (int i=0; i<m; ++i)
            svm_model_->probA[i] = ctrl_probA_->to<realvec>()(i);
        }
        else
          svm_model_->probA = NULL;

        if (ctrl_probB_->to<realvec>().getSize()) //probB
        {
          svm_model_->probB = Malloc(double,m);
          for (int i=0; i<m; ++i)
            svm_model_->probB[i]=ctrl_probB_->to<realvec>()(i);
        }
        else
          svm_model_->probB = NULL;

        if (ctrl_label_->to<realvec>().getSize()) //label
        {
          svm_model_->label = Malloc(int,n);
          for (int i=0; i<n; ++i)
            svm_model_->label[i]
            = (int)ctrl_label_->to<realvec>()(i);
        }
        else
          svm_model_->label = NULL;

        if (ctrl_nSV_->to<realvec>().getSize()) //nr_sv
        {
          svm_model_->nSV = Malloc(int,n);
          for (int i=0; i<n; ++i)
            svm_model_->nSV[i]=(int)ctrl_nSV_->to<realvec>()(i);
        }
        else
          svm_model_->nSV = NULL;

#ifdef MARSYAS_LOG_DEBUGS
        if (svm_model_->rho) {
          MRSDEBUG("svm_model_->rho =");
          for (int i=0; i<m; ++i)
            cout << " "<< svm_model_->rho[i];
          cout << endl;;
        }

        if (svm_model_->probA) {
          MRSDEBUG("svm_model_->probA =");
          for (int i=0; i<m; ++i)
            cout << " " << svm_model_->probA[i];
          cout << endl;;
        }

        if (svm_model_->probB) {
          MRSDEBUG("svm_model_->probB =");
          for (int i=0; i<m; ++i)
            cout << " " << svm_model_->probB[i];
          cout << endl;;
        }

        if (svm_model_->label) {
          MRSDEBUG("svm_model_->label =");
          for (int i=0; i<n; ++i)
            cout << " " << svm_model_->label[i];
          cout << endl;;
        }

        if (svm_model_->nSV) {
          MRSDEBUG("svm_model_->nSV =");
          for (int i=0; i<n; ++i)
            cout << " " << svm_model_->nSV[i];
          cout << endl;;
        }

#endif
        --n;
        m = ctrl_SV_->to<realvec>().getCols();

        if (ctrl_sv_coef_->to<realvec>().getSize()) // sv_coef
        {
          svm_model_->sv_coef = Malloc(double *,n);
          for (int i=0; i<n; ++i)
            svm_model_->sv_coef[i] = Malloc(double,l);
          for (int i=0; i<l; ++i)
            for (int k=0; k<n; k++)
              svm_model_->sv_coef[k][i]
              =ctrl_sv_coef_->to<realvec>()(k, i);
        }
        svm_model_->sv_indices = Malloc(int, n);
        for (int i=0; i<l; i++) {
          // FIXME: interface with libsvm
        }

        if (ctrl_SV_->to<realvec>().getSize()) // SV
        {
          svm_model_->SV = Malloc(svm_node*,l);
          svm_node *x_space=NULL;
          if (l>0) {
            x_space = Malloc(svm_node, 2*m*l);
            num_nodes++;
          }
          int j=0;
          for (int i=0; i<l; ++i) {
            svm_model_->SV[i] = &x_space[j];
            for (int k = 0; k < m; k++) {
              x_space[j].index = k+1;
              x_space[j].value = ctrl_SV_->to<realvec>()(i, k);
              ++j;
            }
            x_space[j++].index = -1;
          }
        }

#ifdef MARSYAS_LOG_DEBUGS

        MRSDEBUG ("svm_model_->SV = ");

        {
          for (int i=0; i<l; ++i) {
            for (int j=0; j<n; j++) {
              if (svm_model_->sv_coef)
                cout <<svm_model_->sv_coef[j][i] << " ";
              if (svm_model_->SV) {
                const svm_node *p = svm_model_->SV[i];
                while (p->index != -1) {
                  cout << p->index << ":";
                  cout << p->value << " ";
                  p++;
                }
              }
              cout << endl;;
            }
          }
        }
#endif
        svm_model_->free_sv = 1;
        MRSDEBUG ("svm_model_->free_sv = " << svm_model_->free_sv);
        trained_ = true;

#ifdef MARSYAS_LOG_DEBUGS
        {
          MRSDEBUG ("mini/maxi : ");
          realvec mini = ctrl_minimums_->to<mrs_realvec>();
          realvec maxi = ctrl_maximums_->to<mrs_realvec>();
          for (int i=0; i<inObservations_ -1; ++i)
            cout << "mini(" << i << ")" << mini(i) << "  maxi("
                 << i << ")" << maxi(i) << endl;
        }
#endif
      } // if(!was_training)
    }// if(!trained)


    /// At this point we should have a trained model ready

    struct svm_node* xv = new svm_node[inObservations_];
    double* probs = new double[svm_model_->nr_class];

    // Get the minimum and maximum values to which the
    // training set was normalized.
    realvec mini = ctrl_minimums_->to<mrs_realvec>();
    realvec maxi = ctrl_maximums_->to<mrs_realvec>();

    // Scale our input the same as our trainingset
    for (int i=0; i<inObservations_ -1; ++i)
      in(i, 0) = (in(i, 0) - mini(i)) / (maxi(i) - mini(i));

    // Copy our input to an SV structure
    for (int j=0; j < inObservations_; ++j) {
      if (j < inObservations_ -1) {
        xv[j].index = j+1;
        xv[j].value = in(j, 0);
      } else {
        // The last index value in the SV is always set to -1
        xv[j].index = -1;
        xv[j].value = 0.0;
      }
    }

    double prediction = 0.0;

    if (ctrl_probability_->to<mrs_bool>())
      prediction = svm_predict_probability(svm_model_, xv, probs);
    else
      prediction = svm_predict(svm_model_, xv);


    // Output
    if (getctrl("mrs_bool/output_classPerms")->isTrue()) {
      for (int i=0; i < svm_model_->nr_class; ++i) {
        out(2 + classPerms_[i], 0) = probs[i];
      }
    }

    out(0,0) = (mrs_real)prediction;
    out(1,0) = in(inObservations_-1,0);


    // Cleanup
    delete [] xv;
    delete [] probs;
  }
  was_training_ = training_;
}
