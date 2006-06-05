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
    \class GaussianClassifier
    \brief Single Gaussian multidimensional classifier

   Simple multidimensional Gaussian classifier. The classifier
parameters (or model) are stored in the the theta control.  When the
mode control is set to "train" the input slice is used to accumulate
information for training.  The actual final theta calculation is
completed when the control train is set to true. That can accomodate
non-incremental or batch training. The labels control is used to
provide ground truth about the label(s). The output of the classifier is
the ground truth label(s) when the mode control is train.

When the mode control is set to "predict" then the output 
of the classifier is the predicted labels using the trained 
parameter vector theta. 

This MarSystems serves as a prototypical classification/regression 
MarSystem. 

*/

#ifndef MARSYAS_GAUSSIANCLASSIFIER_H
#define MARSYAS_GAUSSIANCLASSIFIER_H

#include "MarSystem.h"	


class GaussianClassifier: public MarSystem
{
private: 
  void addControls();
  realvec means_;
  realvec covars_;
  realvec labelSizes_;
  

public:
  GaussianClassifier(std::string name);
  ~GaussianClassifier();
  MarSystem* clone() const;  
  
  void update();
  void process(realvec& in, realvec& out);
};

#endif
	
	
