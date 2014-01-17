/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.princeton.edu>
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

*/



#include "ClassificationReport.h"
#include "../common_source.h"



using std::ostringstream;
using std::vector;
using std::cout;
using std::endl;


using namespace Marsyas;

ClassificationReport::ClassificationReport(mrs_string name) : MarSystem("ClassificationReport", name)
{
  regCorr.sumClass = 0;
  regCorr.sumSqrClass = 0;
  regCorr.sumClassPredicted = 0;
  regCorr.sumPredicted = 0;
  regCorr.sumSqrPredicted = 0;
  regCorr.withClass = 0;
  addControls();
}


ClassificationReport::~ClassificationReport()
{
}

MarSystem *ClassificationReport::clone() const
{
  return new ClassificationReport(*this);
}

void ClassificationReport::addControls()
{
  addctrl("mrs_string/mode", "train");
  setctrlState("mrs_string/mode", true);
  addctrl("mrs_natural/nClasses", 2);
  setctrlState("mrs_natural/nClasses", true);
  addctrl("mrs_string/classNames", "Music,Speech");
  setctrlState("mrs_string/classNames", true);
  addctrl("mrs_bool/done", false);
  addctrl("mrs_bool/regression", false);
}

void ClassificationReport::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("ClassificationReport.cpp - ClassificationReport:myUpdate");

  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", (mrs_natural)2);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

  mrs_natural nClasses = getctrl("mrs_natural/nClasses")->to<mrs_natural>();
  if (confusionMatrix.getRows() != nClasses)
  {
    confusionMatrix.create(nClasses, nClasses);
  }//if
  classNames = getctrl("mrs_string/classNames")->to<mrs_string>();

}//myUpdate

void ClassificationReport::myProcess(realvec& in, realvec& out)
{

  static int count = 0;


  mrs_natural t;
  mrs_string mode = getctrl("mrs_string/mode")->to<mrs_string>();

  //modified this code to check the done flag-dale
  bool done = getctrl("mrs_bool/done")->to<mrs_bool>();



  if ((mode == "train") && !done)
  {
    for (t=0; t < inSamples_; t++)
    {
      mrs_real label = in(inObservations_-1, t);
      out(0,t) = label;
      out(1,t) = label;
    }//for t
  }//if train
  else if ((mode == "predict") && !done)
  {
    count++;

    for (t=0; t < inSamples_; t++)
    {
      if (getctrl("mrs_bool/regression")->isTrue()) {
        mrs_real prediction = in(0, t);	//prediction
        mrs_real actual = in(1, t);	//actual
        //cout<<prediction<<'\t'<<actual<<endl;
        regCorr.sumClass += actual;
        regCorr.sumSqrClass += actual*actual;
        regCorr.sumClassPredicted += actual*prediction;
        regCorr.sumPredicted += prediction;
        regCorr.sumSqrPredicted += prediction*prediction;
        regCorr.withClass += 1.0;
        out(0,t) = prediction;
        out(1,t) = actual;
      } else {
        //swapped the x and y values-dale
        mrs_natural prediction = (mrs_natural)in(0, t);	//prediction
        mrs_natural actual = (mrs_natural)in(1, t);	//actual

        confusionMatrix(actual,prediction)++;
        //cout << "(y,x) (" << y << ","<< x << ")"<< endl;

        out(0,t) = prediction;
        out(1,t) = actual;
      }
    }
  }
  else if(mode == "report" || done)
  {
    ostringstream stream;

    if (getctrl("mrs_bool/regression")->isTrue()) {

      mrs_real varActual = regCorr.sumSqrClass -
                           (regCorr.sumClass*regCorr.sumClass) /
                           regCorr.withClass;
      mrs_real varPredicted = regCorr.sumSqrPredicted -
                              (regCorr.sumPredicted*regCorr.sumPredicted) /
                              regCorr.withClass;
      mrs_real varProd = regCorr.sumClassPredicted -
                         (regCorr.sumClass*regCorr.sumPredicted) /
                         regCorr.withClass;

      mrs_real correlation;
      if (varActual * varPredicted <= 0) {
        correlation = 0.0;
      } else {
        correlation = varProd / sqrt(varActual*varPredicted);
      }

      mrs_real meanAbsoluteError = 0.0;
      mrs_real rootMeanSquaredError = 0.0;
      mrs_real relativeAbsoluteError = 0.0;
      mrs_real rootRelativeSquaredError = 0.0;
      mrs_real instances = 0;
      stream << "=== ClassificationReport ===" << endl << endl;
      stream << "Correlation coefficient" << "\t\t\t" << correlation << "\t" << endl;
      stream << "Mean absolute error" << "\t\t\t" << meanAbsoluteError << endl;
      stream << "Root mean squared error" << "\t\t\t" << rootMeanSquaredError << endl;
      stream << "Relative absolute error" << "\t\t\t" << relativeAbsoluteError << endl;
      stream << "Root relative squared error" << "\t\t" << rootRelativeSquaredError << endl;
      stream << "Total Number of Instances" << "\t\t" << instances << endl << endl;
    } else {

      summaryStatistics stats = computeSummaryStatistics(confusionMatrix);
      stream << "=== ClassificationReport ===" << endl << endl;

      stream << "Correctly Classified Instances" << "\t\t" << stats.correctInstances << "\t";
      stream << (((mrs_real)stats.correctInstances / (mrs_real)stats.instances)*100.0);
      stream << " %" << endl;

      stream << "Incorrectly Classified Instances" << "\t" << (stats.instances - stats.correctInstances) << "\t";
      stream << (((mrs_real)(stats.instances - stats.correctInstances) / (mrs_real)stats.instances)*100.0);
      stream << " %" << endl;

      stream << "Kappa statistic" << "\t\t\t\t" << stats.kappa << "\t" << endl;
      stream << "Mean absolute error" << "\t\t\t" << stats.meanAbsoluteError << endl;
      stream << "Root mean squared error" << "\t\t\t" << stats.rootMeanSquaredError << endl;
      stream << "Relative absolute error" << "\t\t\t" << stats.relativeAbsoluteError << endl;
      stream << "Root relative squared error" << "\t\t" << stats.rootRelativeSquaredError << endl;
      stream << "Total Number of Instances" << "\t\t" << stats.instances << endl << endl;

      stream << "=== Confusion Matrix ===";
      stream << endl; stream << endl;

      if(!classNames.size())
        classNames = ",";

      mrs_string::size_type from = 0;
      mrs_string::size_type to = classNames.find(",");

      mrs_natural correct = 0;
      mrs_natural total = 0;
      for (mrs_natural x = 0; x<confusionMatrix.getCols(); x++)
        stream << "\t" << (char)(x+'a');
      stream << "\t" << "<-- classified as";
      stream << endl;

      for(mrs_natural y = 0; y<confusionMatrix.getRows(); y++)
      {
        for(mrs_natural x = 0; x<confusionMatrix.getCols(); x++)
        {
          mrs_natural value = (mrs_natural)confusionMatrix(y, x);
          total += value;
          if(x == y)
            correct += value;

          stream << "\t" << value;
        }//for x
        stream << "\t" << "| ";
        if(from < classNames.size())
        {
          stream << (char)(y+'a') << " = " << classNames.substr(from, to - from);
          from = to + 1;
          to = classNames.find(",", from);
          if(to == mrs_string::npos)
            to = classNames.size();
        }//if
        stream << endl;
      }//for y
      stream << (total > 0 ? correct * 100 / total: 0) << "% classified correctly (" << correct << "/" << total << ")" << endl;
    }

    MrsLog::mrsMessage(stream);

    updControl("mrs_bool/done", true);
  }//if done
}//myProcess

summaryStatistics ClassificationReport::computeSummaryStatistics(const realvec& mat)
{
  MRSASSERT(mat.getCols()==mat.getRows());

  summaryStatistics stats;

  mrs_natural size = mat.getCols();

  vector<mrs_natural>rowSums(size);
  for(int ii=0; ii<size; ++ii) rowSums[ii] = 0;
  vector<mrs_natural>colSums(size);
  for(int ii=0; ii<size; ++ii) colSums[ii] = 0;
  mrs_natural diagonalSum = 0;

  mrs_natural instanceCount = 0;
  for(mrs_natural row=0; row<size; row++)
  {
    for(mrs_natural col=0; col<size; col++)
    {
      mrs_natural num = (mrs_natural)mat(row,col);
      instanceCount += num;

      rowSums[row] += num;
      colSums[col] += num;

      if(row==col)
        diagonalSum += num;
    }
  }
  //printf("row1 sum:%d\n",rowSums[0]);
  //printf("row2 sum:%d\n",rowSums[1]);
  //printf("col1 sum:%d\n",colSums[0]);
  //printf("col2 sum:%d\n",colSums[1]);
  //printf("diagonal sum:%d\n",diagonalSum);
  //printf("instanceCount:%d\n",instanceCount);

  mrs_natural N = instanceCount;
  mrs_natural N2 = (N*N);
  stats.instances = instanceCount;
  stats.correctInstances = diagonalSum;

  mrs_natural sum = 0;
  for(mrs_natural ii=0; ii<size; ++ii)
  {
    sum += (rowSums[ii] * colSums[ii]);
  }
  mrs_real PE = (mrs_real)sum / (mrs_real)N2;
  mrs_real PA = (mrs_real)diagonalSum / (mrs_real)N;
  stats.kappa = (PA - PE) / (1.0 - PE);

  mrs_natural not_diagonal_sum = instanceCount - diagonalSum;
  mrs_real MeanAbsoluteError = (mrs_real)not_diagonal_sum / (mrs_real)instanceCount;
  //printf("MeanAbsoluteError:%f\n",MeanAbsoluteError);
  stats.meanAbsoluteError = MeanAbsoluteError;

  mrs_real RootMeanSquaredError = sqrt(MeanAbsoluteError);
  //printf("RootMeanSquaredError:%f\n",RootMeanSquaredError);
  stats.rootMeanSquaredError = RootMeanSquaredError;

  mrs_real RelativeAbsoluteError = (MeanAbsoluteError / 0.5) * 100.0;
  //printf("RelativeAbsoluteError:%f%%\n",RelativeAbsoluteError);
  stats.relativeAbsoluteError = RelativeAbsoluteError;

  mrs_real RootRelativeSquaredError = (RootMeanSquaredError / (0.5)) * 100.0;
  //printf("RootRelativeSquaredError:%f%%\n",RootRelativeSquaredError);
  stats.rootRelativeSquaredError = RootRelativeSquaredError;

  return stats;
}//computeSummaryStatistics
