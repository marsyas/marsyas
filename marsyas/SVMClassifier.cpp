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

#include "SVMClassifier.h"

using namespace std;
using namespace Marsyas;

SVMClassifier::SVMClassifier(string name):MarSystem("SVMClassifier", name)
{
  prev_mode_ = "predict";
  addControls();
}

SVMClassifier::SVMClassifier(const SVMClassifier& a) : MarSystem(a)
{
  prev_mode_ = "predict";

}


SVMClassifier::~SVMClassifier()
{
}

MarSystem*
SVMClassifier::clone() const
{
	return new SVMClassifier(*this);
}

void
SVMClassifier::addControls()
{
  addctrl("mrs_string/mode", "train");
}

void
SVMClassifier::myUpdate(MarControlPtr sender)
{
	(void) sender;
	MRSDIAG("SVMClassifier.cpp - SVMClassifier:myUpdate");
	ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
	setctrl("mrs_natural/onObservations", 2);


	// default values
        svm_param_.svm_type = C_SVC;
        svm_param_.kernel_type = RBF;
        svm_param_.degree = 3;
        svm_param_.gamma = 0;        // 1/k
        svm_param_.coef0 = 0;
        svm_param_.nu = 0.5;
        svm_param_.cache_size = 100;
        svm_param_.C = 1;
        svm_param_.eps = 1e-3;
        svm_param_.p = 0.1;
        svm_param_.shrinking = 1;
        svm_param_.probability = 0;
        svm_param_.nr_weight = 0;
        svm_param_.weight_label = NULL;
        svm_param_.weight = NULL;


}

void
SVMClassifier::myProcess(realvec& in, realvec& out)
{
  mode_ = getctrl("mrs_string/mode")->to<mrs_string>();
  /* cout << "SVMClassifier::myProcess" << endl;
  cout << "mode_ = " << mode_ << endl;
  cout << "prev_mode_ = " << prev_mode_  << endl;
  */ 

  if (mode_ == "train")
    {
      if (prev_mode_ == "predict")
	{
	  mrs_natural nAttributes = getctrl("mrs_natural/inObservations")->to<mrs_natural>();
	  cout << "nAttributes = " << nAttributes << endl;
	  instances_.Create(nAttributes);
	}
      instances_.Append(in);
      out(0,t) = in(inObservations_-1, t);
      out(1,t) = in(inObservations_-1, t);
    }
  
  
  if ((prev_mode_ == "train") && (mode_ == "predict"))
    {
      mrs_natural nInstances = instances_.getRows(); 
      mrs_natural nAttributes = getctrl("mrs_natural/inObservations")->to<mrs_natural>();
      cout << "nInstances = " << nInstances << endl;
      svm_prob_.l = nInstances;
      svm_prob_.y = new double[svm_prob_.l];
      svm_prob_.x = new svm_node*[nInstances];
      for (int i=0; i < nInstances; i++) 
	svm_prob_.y[i] = instances_.GetClass(i);

      for (int i=0; i < nInstances; i++) 
	{
	  svm_prob_.x[i] = new svm_node[nAttributes];
	  for (int j=0; j < nAttributes; j++)
	    {
	      if (j < nAttributes -1) 
		{
		  svm_prob_.x[i][j].index = j+1;
		  svm_prob_.x[i][j].value = instances_.at(i)->at(j);
		}
	      else 
		{
		  svm_prob_.x[i][j].index = -1;
		  svm_prob_.x[i][j].value = 0.0;
		}
	    }
	}
      

      const char *error_msg;
      error_msg = svm_check_parameter(&svm_prob_, &svm_param_);
      if(error_msg)
        {
	  fprintf(stderr,"Error: %s\n",error_msg);
	  exit(1);
        }
      
      cout << "starting training" << endl;
      svm_model_ = svm_train(&svm_prob_, &svm_param_);
      cout << "end training" << endl;
      
      std::vector<std::string>classesFound;      
      classesFound.push_back("iris_1");
      classesFound.push_back("iris_2");
      classesFound.push_back("iris_3");      
      instances_.Dump("svmdump.txt", classesFound);
    }

  if (mode_ == "predict") 
    {
      // FIXME These variables are unused (and one is allocating memory!)
      // mrs_natural nAttributes = getctrl("mrs_natural/inObservations")->to<mrs_natural>();
      // struct svm_node* xv = new svm_node[nAttributes];
      (void) 42; // avoid warnings
    }



  prev_mode_ = mode_;



  
}


