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

#include "PeakClusterSelect.h"

using namespace std;
using namespace Marsyas;

PeakClusterSelect::PeakClusterSelect(string name):MarSystem("PeakClusterSelect", name)
{
	addControls();  
}

PeakClusterSelect::PeakClusterSelect(const PeakClusterSelect& a) : MarSystem(a)
{
	ctrl_numClustersToKeep_ = getctrl("mrs_natural/numClustersToKeep");
}

PeakClusterSelect::~PeakClusterSelect()
{
}

MarSystem* 
PeakClusterSelect::clone() const 
{
	return new PeakClusterSelect(*this);
}

void 
PeakClusterSelect::addControls()
{
	addctrl("mrs_natural/numClustersToKeep", 1, ctrl_numClustersToKeep_);
	ctrl_numClustersToKeep_->setState(false);
}

void
PeakClusterSelect::myUpdate(MarControlPtr sender)
{
	(void) sender;
	MRSDIAG("PeakClusterSelect.cpp - PeakClusterSelect:myUpdate");

	ctrl_onObservations_->setValue(1, NOUPDATE);
	ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
	ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
	ctrl_onObsNames_->setValue("peakLabels", NOUPDATE);
}

void
PeakClusterSelect::sort(realvec& rv, mrs_natural dimension, mrs_natural left, mrs_natural right, mrs_bool sortColumns)
{
	if( left < right )
	{
		int part = partition(rv, dimension, left, right, sortColumns);
		sort(rv, dimension, left, part-1, sortColumns);
		sort(rv, dimension, part+1, right, sortColumns);
	}   
}

int
PeakClusterSelect::partition(realvec& rv, mrs_natural dimension, mrs_natural left, mrs_natural right, mrs_bool sortColumns)
{
	// Not quite fair, but good enough random partitioning
	int pivot_i = rand()%(right-left+1) + left;
	// Place pivot val at the end of the series
	swap(rv, pivot_i, right, sortColumns);   

	mrs_real pivot_val;
	if( sortColumns )
		pivot_val = rv(dimension,right);
	else
		pivot_val = rv(right,dimension);

	int i = left-1;   

	if( sortColumns )
	{
		for( int j=left ; j<right ; j++ )
		{
			if( rv(dimension,j) <= pivot_val )
			{
				++i;
				swap(rv, i, j, sortColumns);
			}
		}
		// re-insert pivot val
		swap(rv, i+1, right, sortColumns);      
	}
	else
	{
		for( int j=left ; j<right ; j++ )
		{
			if( rv(j,dimension) <= pivot_val )
			{
				++i;
				swap(rv, i, j, sortColumns);
			}
		}
		// re-insert pivot val
		swap(rv, i+1, right, sortColumns);
	}

	return i+1;
}

void
PeakClusterSelect::swap(realvec& rv, mrs_natural sample1, mrs_natural sample2, mrs_bool swapColumns)
{
	if( swapColumns ) // swap two columns
	{
		int rows = rv.getRows();
		mrs_real tmp;

		for( int i=0 ; i<rows ; ++i )
		{
			tmp = rv(i, sample1);
			rv(i,sample1) = rv(i,sample2);
			rv(i,sample2) = tmp;
		}      
	}
	else // swap two rows
	{         
		int cols = rv.getCols();
		mrs_real tmp;

		for( int i=0 ; i<cols ; ++i )
		{
			tmp = rv(sample1,i);
			rv(sample1,i) = rv(sample2,i);
			rv(sample2,i) = tmp;
		}
	}
}

void 
PeakClusterSelect::myProcess(realvec& in, realvec& out)
{	
	mrs_natural t;
	mrs_natural numClustersToKeep = ctrl_numClustersToKeep_->to<mrs_natural>();
	mrs_natural curNumClusters=-1, i, j, curClusterLabel;

	// Determine number of clusters in input by finding maximum index plus 1
	for (t = 0; t < inSamples_; t++)
		if( in(0,t) > curNumClusters )
			curNumClusters = (mrs_natural)in(0,t);
	curNumClusters += 1;

	realvec intraClusterSimilarities(3, curNumClusters);
	for( i=0 ; i<curNumClusters ; ++i )
	{
		// Store cluster index in first row
		// (so upon sorting of density values, 
		// we have reference to the cluster)
		intraClusterSimilarities(0,i) = i;
		// Keep track of the number of peaks in each cluster
		intraClusterSimilarities(1,i) = 0;
		// Finally, keep track of the sum of intra-cluster similarities
		intraClusterSimilarities(2,i) = 0;
	}

	for( i=0 ; i<ctrl_inSamples_->to<mrs_natural>() ; ++i )
	{
		for( j=0 ; j<i ; j++ )
		{
			if( in(0,i) == in(0,j) )
			{
				intraClusterSimilarities( 1 , (mrs_natural)in(0,i) ) += 1;
				intraClusterSimilarities( 2 , (mrs_natural)in(0,i) ) += in( i+1 , j );
			}
		}
	}

	// Normalize by the number of elements
	for( i=0 ; i<curNumClusters ; ++i )
	{
		intraClusterSimilarities( 2 , i ) /= intraClusterSimilarities( 1 , i );
	}

	// (Quick) Sort by intra-cluster similarity density
	sort( intraClusterSimilarities, 2, 0, curNumClusters-1 );   

	for (t = 0; t < inSamples_; t++)
	{
		curClusterLabel = (mrs_natural)in(0,t);
		out(0,t) = 0; //curClusterLabel; //signals clusters to be synthesized 

		for( i=0 ; i < (curNumClusters - numClustersToKeep) ; ++i )
		{
			if( curClusterLabel == intraClusterSimilarities(0,i) )
			{
				out(0,t) = -1; //-1 signals clusters to be discarded
				break;
			}
		}
	}
}



