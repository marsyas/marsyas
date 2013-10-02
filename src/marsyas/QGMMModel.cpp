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

#include "QGMMModel.h"
#include <marsyas/NumericLib.h>

using namespace std;
using namespace Marsyas;

//***********************************************************************************************
// class QGMMModel
//***********************************************************************************************
QGMMModel::QGMMModel()
{
  resetModel();
}

QGMMModel::QGMMModel(const QGMMModel &a)
{
  maxNrGSMixes_ = a.maxNrGSMixes_;
  nrGSMixes_ = a.nrGSMixes_;
  GSmixNrFrames_ = a.GSmixNrFrames_;
  GSmixCovMatrix_ = a.GSmixCovMatrix_;
  totalNrFrames_ = a.totalNrFrames_;

  divergenceThres_ = a.divergenceThres_;
}
QGMMModel::~QGMMModel()
{

}

void
QGMMModel::resetModel()
{
  maxNrGSMixes_ = 32;
  nrGSMixes_ = 0;
  GSmixNrFrames_.clear();
  GSmixCovMatrix_.clear();
  totalNrFrames_ = 0;
  divergenceThres_ = 0.50; // [!]
}

realvec
QGMMModel::updateCovModel(realvec C, mrs_natural N, realvec Cm, mrs_natural Nm)
{
  C *=  ((mrs_real)N / (mrs_real)(N+Nm));
  Cm *= ((mrs_real)Nm / (mrs_real)(N+Nm));
  C += Cm;

  return C;
}

void
QGMMModel::updateModel(realvec &covMatrix, mrs_natural nrModelFrames)
{
  static mrs_real meanDiv, count;
  static bool stopUpdate;

////---------------------------------------
//// qGMM1 with no updating of cov matrix
////---------------------------------------
//	if(nrGSMixes_ == 0)
//	{
//		nrGSMixes_++;
//		GSmixCovMatrix_.push_back(covMatrix);
//		GSmixNrFrames_.push_back(nrModelFrames);
//		totalNrFrames_ = nrModelFrames;
//		return;
//	}
//	else
//	{
//		//do not update - replace using the most recent segment data...
//		GSmixCovMatrix_[0] = covMatrix;
//		GSmixNrFrames_[0] = nrModelFrames;
//		totalNrFrames_ = nrModelFrames;
//		return;
//	}
////--------------------------------------

  //if this is the first time the model gets speaker data
  //create first mixture with that data
  if(nrGSMixes_ == 0)
  {
    meanDiv = 0;
    count = 0;
    stopUpdate = false;

    nrGSMixes_++;
    GSmixCovMatrix_.push_back(covMatrix);
    GSmixNrFrames_.push_back(nrModelFrames);
    totalNrFrames_ = nrModelFrames;

    return;
  }
  //otherwise update current speaker model or create a new mix...
  else
  {
    mrs_natural curMix = nrGSMixes_ - 1;

    //calculate updated model using the new speaker data
    realvec updatedModel = updateCovModel(GSmixCovMatrix_[curMix], GSmixNrFrames_[curMix],
                                          covMatrix, nrModelFrames);

    //calculate the divergence between the current model and the updated one
    mrs_real divergenceShape = NumericLib::divergenceShape(GSmixCovMatrix_[curMix],updatedModel);

    //recursive mean divergenceShape calculation
    meanDiv = meanDiv + 1/(count+1)*(divergenceShape - meanDiv);
    count++;

    //if the updated model has a big enough dissimilarity when compared
    //with current model, update current model...
    if (divergenceShape > meanDiv*divergenceThres_ && stopUpdate == false)
    {
      GSmixCovMatrix_[curMix] = updatedModel;
      GSmixNrFrames_[curMix] += nrModelFrames;
      totalNrFrames_ += nrModelFrames;
    }
    //...if the updated model is very similar to the current one (i.e. divergence distnace < threshold)
    //give up updating and create instead a new mixture...
    else
    {
      if (nrGSMixes_ < maxNrGSMixes_)
      {
        nrGSMixes_++;
        GSmixCovMatrix_.push_back(covMatrix);
        GSmixNrFrames_.push_back(nrModelFrames);
        totalNrFrames_ += nrModelFrames;

        meanDiv = 0;
        count = 0;
      }
      else
      {
        stopUpdate = true;
      }
    }
  }
}

mrs_real
QGMMModel::deltaBIC(realvec C1, mrs_natural N1, realvec C2, mrs_natural N2, realvec C, mrs_real lambda)
{
  //matrices should be square and equal sized
  if(C1.getCols() != C2.getCols() && C1.getCols() != C.getCols() &&
      C1.getCols()!= C1.getRows())
  {
    MRSERR("QGMMModel:deltaBIC: matrices should all be squared and equal sized...");
    return MAXREAL; //just a way to sinalize the above error... [!]
  }

  mrs_real res;
  mrs_real N = (mrs_real)(N1 + N2);
  mrs_real d = (mrs_real)C1.getCols();


  res = N * log(C.det());
  res -= (mrs_real)N1 * log(C1.det());
  res -= (mrs_real)N2 * log(C2.det());
  res *= 0.5f;

  res -= 0.5f * lambda * (d + 0.5f*d*(d+1.0f))* log(N);

  return res;
}

mrs_real
QGMMModel::BICdistance(realvec &covMatrix, mrs_natural Nframes, mrs_real lambda)
{
  mrs_real wi;
  mrs_real res = 0;

  //mrs_real d = (mrs_real)covMatrix.getCols();

  //mrs_natural curMix = nrGSMixes_ - 1;

  for(mrs_natural i = 0; i < nrGSMixes_; ++i)
  {
    realvec combinedModel = updateCovModel(GSmixCovMatrix_[i],GSmixNrFrames_[i],
                                           covMatrix, Nframes);

    wi = (mrs_real)GSmixNrFrames_[i]/(mrs_real)totalNrFrames_;
    res += wi * deltaBIC(GSmixCovMatrix_[i], GSmixNrFrames_[i], covMatrix, Nframes, combinedModel, lambda);
  }

  return res;
}
