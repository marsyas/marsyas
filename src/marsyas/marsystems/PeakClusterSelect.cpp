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
#include "../common_source.h"
#include <algorithm>

using std::min;
using std::max;
using std::ostringstream;
using namespace Marsyas;

static std::ofstream outTextFile;
//#define MTLB_DBG_LOG
//#define DBG_FILE_OUT
#ifdef DBG_FILE_OUT
static const std::string outFileName("d:/temp/peakcluster.txt");
#endif


PeakClusterSelect::PeakClusterSelect(mrs_string name):MarSystem("PeakClusterSelect", name)
{
  addControls();
}

PeakClusterSelect::PeakClusterSelect(const PeakClusterSelect& a) : MarSystem(a)
{
  ctrl_numClustersToKeep_ = getctrl("mrs_natural/numClustersToKeep");
}

PeakClusterSelect::~PeakClusterSelect()
{
#ifdef DBG_FILE_OUT
  outTextFile.close ();
#endif
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
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("PeakClusterSelect.cpp - PeakClusterSelect:myUpdate");

  ctrl_onObservations_->setValue(1, NOUPDATE);
  ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
  ctrl_onObsNames_->setValue("peakLabels", NOUPDATE);

#ifdef DBG_FILE_OUT
  if (!outTextFile.good () || ! outTextFile.is_open ())
    outTextFile.open(outFileName.c_str ());
#endif
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
  mrs_natural curNumClusters=-1, i, j, k, curClusterLabel;
  mrs_natural numPeaks = ctrl_inSamples_->to<mrs_natural>();

  // Determine number of clusters in input by finding maximum index plus 1
  for (t = 0; t < inSamples_; t++)
    if( in(0,t) > curNumClusters )
      curNumClusters = (mrs_natural)in(0,t);
  curNumClusters += 1;

  mrs_realvec intraClusterSimilarities(3, curNumClusters);
  mrs_realvec clusterSimilarity (curNumClusters, curNumClusters);
  mrs_realvec clusterNorm (curNumClusters, curNumClusters);
  mrs_realvec keepMe(curNumClusters);
  mrs_real	intraSimKeepThresh	= .5;

  clusterSimilarity.setval (0.);
  clusterNorm.setval (0.);
  keepMe.setval (1.);

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


  // compute the similarity of each peak to each each other
  // accumulate per cluster pair
  for( i=0 ; i<numPeaks ; ++i )
  {
    mrs_realvec numAcc(curNumClusters);
    mrs_realvec similarity(curNumClusters);

    numAcc.setval (0.);
    similarity.setval (0.);
    for( j=0 ; j<numPeaks ; j++ )
    {
      if (i==j)
        continue;
      similarity((mrs_natural)(in(0,j)+.1))								+= in (i+1, j);
      clusterNorm ((mrs_natural)(in(0,i)+.1),(mrs_natural)(in(0,j)+.1))	+= 1;
    }
    for (k = 0; k < curNumClusters; k++)
      clusterSimilarity ((mrs_natural)(in(0,i)+.1), k)	+= similarity(k);
  }

  for (i = 0; i < curNumClusters; i++)
    for (j = 0; j < curNumClusters; j++)
      clusterSimilarity(i,j)	/= (clusterNorm(i,j)>0)? clusterNorm(i,j) : 1.;

  // compute something similar to silhouette coefficients
  mrs_realvec silhouetteCoeffs (curNumClusters);
  silhouetteCoeffs.setval(0.);

  for (k = 0; k < curNumClusters; k++)
  {
    mrs_real selfSim, maximum, minSim = 0;//1e37;
    selfSim	= clusterSimilarity(k,k);
    for (i = 0; i < curNumClusters; i++)
    {
      if (i == k)
        continue;
      //minSim	= (clusterSimilarity(k,i) < minSim)?clusterSimilarity(k,i) :  minSim;
      minSim	+= clusterSimilarity(k,i);
    }
    minSim	/= (curNumClusters-1);

    if ((maximum = max(selfSim, minSim)) != 0)
      silhouetteCoeffs(k)	= (selfSim - minSim)/ maximum;
  }

  // update output values
  for (k = 0; k < curNumClusters; k++)
    intraClusterSimilarities(2,k)	= clusterSimilarity(k,k);

#ifdef MARSYAS_MATLAB
#ifdef MTLB_DBG_LOG
  //MATLAB_PUT(clusterSimilarity, "sim");
  //MATLAB_PUT(curNumClusters, "numClust");
  //MATLAB_EVAL ("figure(13),imagesc(0:(numClust-1),0:(numClust-1),sim,[0 1]),colorbar");
  //MATLAB_PUT(silhouetteCoeffs, "sil");
  //MATLAB_EVAL ("figure(14),plot(0:(length(sil)-1), sil),axis([0 length(sil)-1 0 1]), grid on");
#endif
#endif



  // (Quick) Sort by intra-cluster similarity density
  sort( intraClusterSimilarities, 2, 0, curNumClusters-1 );


#ifdef DBG_FILE_OUT
  if (outTextFile.good ())
  {
    if (curNumClusters < 6)
    {
      for (k = 0; k < 6-curNumClusters; k++)
        outTextFile << 0 <<"\t";
    }
    for (k = 0; k < curNumClusters; k++)
      outTextFile << silhouetteCoeffs(k) <<"\t"; ;
    //outTextFile << intraClusterSimilarities(2,k) <<"\t"; ;

    outTextFile << std::endl;
  }
#endif

  // ignore setting of numclusters2Keep and set it by threshold of the intraclustersimilarity
  if (numClustersToKeep == 0)
  {
    const mrs_real intraThreshBounds[2]	= {.3,.6};
    const mrs_real silThreshBound		= 1./curNumClusters;
    intraSimKeepThresh	= max((mrs_real)intraThreshBounds[0],min((mrs_real)intraThreshBounds[1],(mrs_real).5*((mrs_real)intraClusterSimilarities(2,0) + (mrs_real)intraClusterSimilarities(2, curNumClusters-1))));
    for (k = 0; k < curNumClusters; k++)
    {
      // intra cluster similarity threshold
      if (intraClusterSimilarities(2,k) < intraSimKeepThresh)
        keepMe(k)	= 0;

      // silhouette coefficient threshold
      if (silhouetteCoeffs((mrs_natural)(intraClusterSimilarities(0,k)+.1)) < silThreshBound)
        keepMe(k)	= 0;
    }
    numClustersToKeep	= (mrs_natural)(keepMe.sum () +.1);

    if (numClustersToKeep == curNumClusters)
      keepMe(0)	= 0; // always abandon one cluster!
    //std::cout << numClustersToKeep << " " << std::endl;
  }
  else
  {
    for (k = 0; k < (curNumClusters - numClustersToKeep); k++)
      keepMe(k) = 0;
  }

  for (t = 0; t < inSamples_; t++)
  {
    curClusterLabel = (mrs_natural)in(0,t);
    out(0,t) = curClusterLabel; //signals clusters to be synthesized

    for( k=0 ; k < curNumClusters; k++)
    {
      if( curClusterLabel == intraClusterSimilarities(0,k) )
      {
        if (keepMe(k) < .5)
          out(0,t) = (curClusterLabel)? -curClusterLabel : -1; //negative value signals clusters to be discarded, zero is mapped to -1
        break;
      }
    }
  }
}
