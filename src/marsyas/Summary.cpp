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
    \class Summary
    When the mode control is set to "predict" then then classifications are tracked
    when done control is set, then the confusion matrix is shown
*/

#include "Summary.h"

using namespace std;
using namespace Marsyas;

Summary::Summary(string name) : MarSystem("Summary", name)
{
	addControls();
}


Summary::~Summary()
{
}

MarSystem *Summary::clone() const
{
	return new Summary(*this);
}

void Summary::addControls()
{
	addctrl("mrs_string/mode", "train");
	setctrlState("mrs_string/mode", true);
	addctrl("mrs_natural/nClasses", 2);
	setctrlState("mrs_natural/nClasses", true);
	addctrl("mrs_string/classNames", "Music,Speech");
	setctrlState("mrs_string/classNames", true);
	addctrl("mrs_bool/done", false);
}

void Summary::myUpdate(MarControlPtr sender)
{
	(void) sender;
	MRSDIAG("Summary.cpp - Summary:myUpdate");
  
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

void Summary::myProcess(realvec& in, realvec& out)
{

	static int count = 0;
	

	mrs_natural t;
	string mode = getctrl("mrs_string/mode")->to<mrs_string>();
  
	//modified this code to check the done flag-dale
	bool done = getctrl("mrs_bool/done")->to<mrs_bool>();
	
	
  
	if (strcmp(mode.c_str(), "train") == 0 && !done)
    {
		for (t=0; t < inSamples_; t++)
		{
			mrs_real label = in(inObservations_-1, t);
			out(0,t) = label;
			out(1,t) = label;
		}//for t
    }//if train
	else if (strcmp(mode.c_str(), "predict") == 0 && !done)
	{
		count++;

		for (t=0; t < inSamples_; t++)
		{    
			//swapped the x and y values-dale
			mrs_natural prediction = (mrs_natural)in(0, t);	//prediction  
			mrs_natural actual = (mrs_natural)in(1, t);	//actual
		  
			confusionMatrix(actual,prediction)++;	  
			//cout << "(y,x) (" << y << ","<< x << ")"<< endl;
		  
			out(0,t) = prediction;
			out(1,t) = actual;
		}

	  

      
    }//if
  
	if (done)
    {

		summaryStatistics stats = computeSummaryStatistics(confusionMatrix);
		cout << "=== Summary ===" << endl << endl;
      
		cout << "Correctly Classified Instances" << "\t\t" << stats.correctInstances << "\t";
		cout << (((mrs_real)stats.correctInstances / (mrs_real)stats.instances)*100.0);
		cout << " %" << endl;
      
		cout << "Incorrectly Classified Instances" << "\t" << (stats.instances - stats.correctInstances) << "\t";
		cout << (((mrs_real)(stats.instances - stats.correctInstances) / (mrs_real)stats.instances)*100.0);
		cout << " %" << endl;
      
		cout << "Kappa statistic" << "\t\t\t\t" << stats.kappa << "\t" << endl;
		cout << "Mean absolute error" << "\t\t\t" << stats.meanAbsoluteError << endl;
		cout << "Root mean squared error" << "\t\t\t" << stats.rootMeanSquaredError << endl;
		cout << "Relative absolute error" << "\t\t\t" << stats.relativeAbsoluteError << endl;
		cout << "Root relative squared error" << "\t\t" << stats.rootRelativeSquaredError << endl;
		cout << "Total Number of Instances" << "\t\t" << stats.instances << endl << endl;
      
		cout << "=== Confusion Matrix ===";
		cout << endl; cout << endl;
      
		if(!classNames.size())
			classNames = ",";
      
		string::size_type from = 0;
		string::size_type to = classNames.find(",");
      
		mrs_natural correct = 0;
		mrs_natural total = 0;
		for (mrs_natural x = 0;x<confusionMatrix.getCols();x++)
			cout << "\t" << (char)(x+'a');
		cout << "\t" << "<-- classified as";
		cout << endl;
      
		for(mrs_natural y = 0;y<confusionMatrix.getRows();y++)
		{
			for(mrs_natural x = 0;x<confusionMatrix.getCols();x++)
			{
				mrs_natural value = (mrs_natural)confusionMatrix(y, x);
				total += value;
				if(x == y)
					correct += value;
	      
				cout << "\t" << value;
			}//for x
			cout << "\t" << "| ";
			if(from < classNames.size())
			{
				cout << (char)(y+'a') << " = " << classNames.substr(from, to - from);
				from = to + 1;
				to = classNames.find(",", from);
				if(to == string::npos)
					to = classNames.size();
			}//if
			cout << endl;
		}//for y
		cout << (total > 0 ? correct * 100 / total: 0) << "% classified correctly (" << correct << "/" << total << ")" << endl;
    }//if done
}//myProcess

summaryStatistics Summary::computeSummaryStatistics(const realvec& mat)
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
