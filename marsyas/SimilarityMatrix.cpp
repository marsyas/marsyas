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

#include "SimilarityMatrix.h"

using namespace std;
using namespace Marsyas;

SimilarityMatrix::SimilarityMatrix(string name):MarSystem("SimilarityMatrix", name)
{
	isComposite_ = true;
	addControls();
}

SimilarityMatrix::SimilarityMatrix(const SimilarityMatrix& a) : MarSystem(a)
{
	ctrl_covMatrix_ = getctrl("mrs_realvec/covMatrix");
	ctrl_calcCovMatrix_ = getctrl("mrs_natural/calcCovMatrix");
	ctrl_normalize_ = getctrl("mrs_string/normalize");
}

SimilarityMatrix::~SimilarityMatrix()
{
}

MarSystem* 
SimilarityMatrix::clone() const 
{
	return new SimilarityMatrix(*this);
}

void 
SimilarityMatrix::addControls()
{
	addControl("mrs_realvec/covMatrix", realvec(), ctrl_covMatrix_);
	addControl("mrs_natural/calcCovMatrix", SimilarityMatrix::noCovMatrix, ctrl_calcCovMatrix_);
	addControl("mrs_string/normalize", "none", ctrl_normalize_);
}

void
SimilarityMatrix::myUpdate(MarControlPtr sender)
{
	// The output similarity matrix is a squared matrix with
	// dimension equal to the number of elements (i.e. samples)
	// in the input

	//forward flow propagation
	ctrl_onObservations_->setValue(ctrl_inSamples_, NOUPDATE);
	ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
	ctrl_osrate_->setValue(ctrl_osrate_, NOUPDATE); //[?]
	ostringstream oss;
	for(o=0; o < ctrl_onObservations_->to<mrs_natural>(); ++o)
		oss << "SimilarityMatrix_" << o << ",";
	ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);

	//if the metric MarSystem exists and at least there is an element
	// to process at the input (it may not exist!), configure child marSystem.
	// Otherwise do nothing to it.
	if(marsystemsSize_ == 1 && inSamples_ > 0)
	{
		//allocate realvec for the pair of stacked feature vectors
		//to be used in the similarity computation
		i_featVec_.create(ctrl_inObservations_->to<mrs_natural>());
		j_featVec_.create(ctrl_inObservations_->to<mrs_natural>());
		stackedFeatVecs_.create(ctrl_inObservations_->to<mrs_natural>()*2, 1);
		
		//configure the metric child MarSystem:
		//the input to metric are the two vector to process stacked vertically
		marsystems_[0]->setctrl("mrs_natural/inObservations", ctrl_inObservations_->to<mrs_natural>()*2);
		marsystems_[0]->setctrl("mrs_natural/inSamples", 1);
		marsystems_[0]->setctrl("mrs_real/israte", ctrl_israte_->to<mrs_real>()); //[?]
		oss.clear();
		oss << ctrl_inObsNames_->to<mrs_string>() << ctrl_inObsNames_->to<mrs_string>(); //[?]
		marsystems_[0]->setctrl("mrs_string/inObsNames", oss.str());
		marsystems_[0]->update();

		//link covMatrix control
		string childCname = marsystems_[0]->getPrefix() + "mrs_string/covMatrix";
		linkctrl(childCname, "mrs_string/covMatrix");

		//allocate space for the metric result (must be a real value)
		metricResult_.create(1,1);

		//check if the child is a valid metric MarSystem
		if(marsystems_[0]->getctrl("mrs_natural/onObservations") != 1 ||
			marsystems_[0]->getctrl("mrs_natural/onSamples") != 1)
			 MRSWARN("SimilarityMatrix::myUpdate - invalid Child Metric MarSystem (does not output a real value)!");
	}
	else if(marsystemsSize_ > 1)
	{
		MRSWARN("similarityMatrix::myUpdate - more than one children MarSystem exists! Only one MarSystem should be added as a metric!");
	}
}

void 
SimilarityMatrix::myProcess(realvec& in, realvec& out)
{
	//check if there are any elements to process at the input
	//(in some cases, they may not exist!) - otherwise, do nothing
	//(i.e. output is also an empty vector)
	if(inSamples_ > 0)
	{
		if(marsystemsSize_ == 1)
		{
			mrs_natural nfeats = in.getRows();

			//normalize input features if necessary
			if(ctrl_normalize_->to<mrs_string>() == "MinMax")
				in.normObsMinMax(); // (x-min)/max
			else if(ctrl_normalize_->to<mrs_string>() == "MeanStd")
				in.normObs(); // (x-mean)/std

			//calculate the Covariance Matrix from the input, if defined
			if(ctrl_calcCovMatrix_->to<mrs_natural>() & SimilarityMatrix::diagCovMatrix)
			{
				//FASTER -> only get the vars for each feature
				in.varObs(vars_);
				mrs_natural dim = vars_.getSize();
				covMatrix_.create(dim, dim);
				//fill covMatrix diagonal with var values
				for(mrs_natural i=0; i< dim; ++i)
				{
					covMatrix_(i,i) = vars_(i);
				}
				ctrl_covMatrix_->setValue(covMatrix_);
			}
			else if(ctrl_calcCovMatrix_->to<mrs_natural>() & SimilarityMatrix::fullCovMatrix)
			{
				//SLOWER -> estimate the full cov matrix
				realvec tmp;
				in.covariance(tmp);
				ctrl_covMatrix_->setValue(tmp); // SLOWWWWWWW!!! [TODO]
			}
			else if(ctrl_calcCovMatrix_->to<mrs_natural>() == SimilarityMatrix::noCovMatrix)
			{
				ctrl_covMatrix_->setValue(realvec());
			}

			for(mrs_natural i=0; i < in.getCols(); ++i)
			{
				in.getCol(i, i_featVec_);
				for(mrs_natural j=0; j <= i; ++j)
				{
					in.getCol(j, j_featVec_);

					//stack i and j feat vecs
					for(mrs_natural r=0; r < nfeats; ++r)
					{
						stackedFeatVecs_(r, 0) = i_featVec_(r);
						stackedFeatVecs_(r+nfeats, 0) = j_featVec_(r);
					}
					//do the metric calculation for these two feat vectors
					//and store it in the similarity matrix (which is symmetric)
					marsystems_[0]->process(stackedFeatVecs_, metricResult_);
					out(i,j) = metricResult_(0,0);
					//metric should be symmetric!
					out(j, i) = out(i, j); 			
				}
			}
		}
		else
		{
			out.setval(0.0);
			if(marsystemsSize_ == 0)
			{
				MRSWARN("SimilarityMatrix::myProcess - no Child Metric MarSystem added - outputting zero similarity matrix!");
			}
			else
			{
				MRSWARN("SimilarityMatrix::myProcess - more than one Child MarSystem exists (i.e. invalid metric) - outputting zero similarity matrix!");
			}
		}
	}
}








