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

/**
    \class ClassificationReport
    \brief Reports various measures of classification accuracy

	This MarSystem typically follows a classification MarSystem
	and output various measures used to evaluate the accuracy of a classifier
	for a particular dataset. These include accuracy, error-rate, the
	confusion matrix. When the mode control is set to "predict" then the
	classification decisions are tracked and the measures are computed
	until the done control is set to true. Then the measures are displayed
	to stdout. The formatting is similar to the one used by the Weka machine
	learning software.

	Controls:
	- \b mrs_bool/regression [w] : output regression
	  statistics instead of classication

*/

#ifndef MARSYAS_CLASSIFICATIONREPORT_H
#define MARSYAS_CLASSIFICATIONREPORT_H

#include <marsyas/system/MarSystem.h>


namespace Marsyas
{

//Some statistical information computed by computeSummaryStatistics
typedef struct
{
  mrs_natural	instances;
  mrs_natural	correctInstances;
  mrs_real kappa;
  mrs_real meanAbsoluteError;
  mrs_real rootMeanSquaredError;
  mrs_real relativeAbsoluteError;
  mrs_real rootRelativeSquaredError;
} summaryStatistics;

typedef struct
{
  // rewritten from weka:
  // https://svn.scms.waikato.ac.nz/svn/weka/trunk/weka/src/main/java/weka/classifiers/Evaluation.java
  mrs_real sumClass;
  mrs_real sumSqrClass;
  mrs_real sumClassPredicted;
  mrs_real sumPredicted;
  mrs_real sumSqrPredicted;
  mrs_real withClass;
} RegressionCorrelationCalculate;

class ClassificationReport: public MarSystem
{
private:
  void addControls();
  void myUpdate(MarControlPtr sender);

  realvec confusionMatrix;
  std::string classNames;

  summaryStatistics computeSummaryStatistics(const realvec& mat);
  RegressionCorrelationCalculate regCorr;

public:
  ClassificationReport(std::string name);
  ~ClassificationReport();
  MarSystem* clone()const;

  void myProcess(realvec& in, realvec& out);
};//class ClassificationReport
}//namespace Marsyas
#endif
