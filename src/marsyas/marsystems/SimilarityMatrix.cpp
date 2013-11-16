#include "SimilarityMatrix.h"

using namespace std;
using namespace Marsyas;

SimilarityMatrix::SimilarityMatrix(mrs_string name):MarSystem("SimilarityMatrix", name)
{
  isComposite_ = true;
  addControls();
}

SimilarityMatrix::SimilarityMatrix(const SimilarityMatrix& a):MarSystem(a)
{
  ctrl_covMatrix_ = getctrl("mrs_realvec/covMatrix");
  ctrl_calcCovMatrix_ = getctrl("mrs_natural/calcCovMatrix");
  ctrl_normalize_ = getctrl("mrs_string/normalize");
  ctrl_stdDev_ = getctrl("mrs_real/stdDev");
  ctrl_sizes_ = getctrl("mrs_realvec/sizes");
  //ctrl_disMatrix_ = getctrl("mrs_realvec/disMatrix");
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
  //  addControl("mrs_realvec/disMatrix", realvec(), ctrl_disMatrix_);
  addControl("mrs_realvec/covMatrix", realvec(), ctrl_covMatrix_);
  addControl("mrs_natural/calcCovMatrix", SimilarityMatrix::noCovMatrix, ctrl_calcCovMatrix_);
  addControl("mrs_string/normalize", "none", ctrl_normalize_);
  addControl("mrs_real/stdDev", 1.0, ctrl_stdDev_);
  addControl("mrs_realvec/sizes", realvec(), ctrl_sizes_);
}

void SimilarityMatrix::myUpdate(MarControlPtr sender)
{

  (void) sender;  //suppress warning of unused parameter(s)
  MarControlAccessor acc(ctrl_sizes_);
  realvec& tmpvec = acc.to<mrs_realvec>();
  mrs_natural insize = ctrl_inSamples_->to<mrs_natural>();
  if(tmpvec.getRows() == 1 && tmpvec.getCols() >= 2)
  {
    sizes_.create(tmpvec.getCols());
    for(mrs_natural i=0; i<tmpvec.getCols(); ++i)
    {
      sizes_(i) = (mrs_natural)tmpvec(0,i);
    }
    for(mrs_natural i=0; i<tmpvec.getCols(); ++i)
    {
      if(sizes_(i) > insize)
        sizes_(i) = insize;
    }
  }
  else if(tmpvec.getRows() >= 2 && tmpvec.getCols() == 1)
  {
    sizes_.create(tmpvec.getRows());
    for(mrs_natural i=0; i<tmpvec.getRows(); ++i)
    {
      sizes_(i) = (mrs_natural)tmpvec(i,0);
    }
    for(mrs_natural i=0; i<tmpvec.getRows(); ++i)
    {
      if(sizes_(i) > insize)
        sizes_(i) = insize;
    }
  }
  else
  {
    sizes_.create(2);
    sizes_(0) = insize;
    sizes_(1) = insize;
  }

  mrs_natural obs = 0;
  for(mrs_natural i=1; i<sizes_.getSize(); ++i)
  {
    obs += (mrs_natural)sizes_(i);
  }
  ctrl_onObservations_->setValue(obs, NOUPDATE);
  ctrl_onSamples_->setValue((mrs_natural)sizes_(0), NOUPDATE);
  ctrl_osrate_->setValue(ctrl_osrate_, NOUPDATE);
  ostringstream oss;
  for(mrs_natural o=0; o<ctrl_onObservations_->to<mrs_natural>(); o++)
    oss << "SimilarityMatrix_" << o << ",";
  ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);

  invecs_.resize(sizes_.getSize());
  obs = getctrl("mrs_natural/inObservations")->to<mrs_natural>()/sizes_.getSize();
  for(mrs_natural k=0; k<sizes_.getSize(); k++)
  {
    invecs_[k].create(obs, (mrs_natural)sizes_(k));
  }

  child_count_t child_count = marsystems_.size();
  if(child_count == 1 && inSamples_ > 0)
  {
    // allocate realvec for the pair of stacked feature vectors
    // to be used in the similarity computation
    i_featVec_.create(ctrl_inObservations_->to<mrs_natural>()/sizes_.getSize());
    j_featVec_.create(ctrl_inObservations_->to<mrs_natural>()/sizes_.getSize());
    stackedFeatVecs_.create(ctrl_inObservations_->to<mrs_natural>()/sizes_.getSize()*2, 1);

    // configure the metric child MarSystem:
    // the input to metric are the two vectors to process stacked vertically
    marsystems_[0]->setctrl("mrs_natural/inObservations", stackedFeatVecs_.getRows());
    marsystems_[0]->setctrl("mrs_natural/inSamples", 1);
    marsystems_[0]->setctrl("mrs_real/israte", ctrl_israte_->to<mrs_real>());
    oss.clear();
    oss << ctrl_inObsNames_->to<mrs_string>() << ctrl_inObsNames_->to<mrs_string>();
    marsystems_[0]->setctrl("mrs_string/inObsNames", oss.str());
    marsystems_[0]->update();

    // link covMatrix control
    MarControlPtr ctrl_childCovMat = marsystems_[0]->getctrl("mrs_realvec/covMatrix");
    if(!ctrl_childCovMat.isInvalid())
      ctrl_childCovMat->linkTo(ctrl_covMatrix_);
    metricResult_.create(1,1);
    if(marsystems_[0]->getctrl("mrs_natural/onObservations") != 1 ||
        marsystems_[0]->getctrl("mrs_natural/onSamples") != 1)
    {
      MRSWARN("SimilarityMatrix:myUpdate - invalid Child Metric MarSystem (does not output a real value)!");
    }
  }
  else if(child_count > 1)
  {
    MRSWARN("similarityMatrix2:myUpdate - more than one children MarSystem exist! Only one MarSystem should be added as a metric!");
  }
}

void
SimilarityMatrix::myProcess(realvec& in, realvec& out)
{
  //check if there are any elements to process at the input
  //(in some cases, they may not exist!) - otherwise, do nothing
  //(i.e. output is also an empty vector)
  mrs_natural i, j, k, l;

  if(inSamples_ > 0)
  {
    child_count_t child_count = marsystems_.size();
    if(child_count == 1)
    {
      mrs_natural nfeats = in.getRows()/sizes_.getSize();

      // calculate hte Covariance Matrix from the input, if defined
      mrs_natural obs = 0;
      for(i=0; i<sizes_.getSize(); ++i)
      {
        for(j=0; j<sizes_(i); j++)
        {
          for(k=0; (mrs_natural)k<invecs_[i].getRows(); k++)
          {
            invecs_[i](k, j) = in(k+obs, j);
          }
        }
        obs += invecs_[i].getRows();
      }

      // normalize input features if necessary
      if(ctrl_normalize_->to<mrs_string>() == "MinMax")
        for(i=0; i<sizes_.getSize(); ++i)
        {
          invecs_[i].normObsMinMax(); // (x - min)/(max - min)
        }
      else if(ctrl_normalize_->to<mrs_string>() == "MeanStd")
        for(i=0; i<sizes_.getSize(); ++i)
        {
          invecs_[i].normObs();  // (x - mean)/std
        }

      if(ctrl_calcCovMatrix_->to<mrs_natural>() & SimilarityMatrix::fixedStdDev)
      {
        MarControlAccessor acc(ctrl_covMatrix_);
        realvec& covMatrix = acc.to<mrs_realvec>();
        covMatrix.create(inObservations_/sizes_.getSize(), inObservations_/sizes_.getSize());
        mrs_real var = ctrl_stdDev_->to<mrs_real>();
        var *= var;
        for(i=0; i< inObservations_/sizes_.getSize(); ++i)
        {
          covMatrix(i,i) = var;
        }
      }
      else if(ctrl_calcCovMatrix_->to<mrs_natural>() & SimilarityMatrix::diagCovMatrix)
      {
        invecs_[0].varObs(vars_); // Faster
        mrs_natural dim = vars_.getSize();
        // fill covMatrix diagonal with var values (remaining values are zero)
        MarControlAccessor acc(ctrl_covMatrix_);
        realvec& covMatrix = acc.to<mrs_realvec>();
        covMatrix.create(dim, dim);
        for(i=0; i<(mrs_natural)dim; ++i)
        {
          covMatrix(i,i) = vars_(i);
        }
      }
      else if(ctrl_calcCovMatrix_->to<mrs_natural>() & SimilarityMatrix::fullCovMatrix)
      {
        MarControlAccessor acc(ctrl_covMatrix_);
        realvec& covMatrix = acc.to<mrs_realvec>();
        invecs_[0].covariance(covMatrix); // Slower
      }
      else if(ctrl_calcCovMatrix_->to<mrs_natural>() & SimilarityMatrix::noCovMatrix)
      {
        ctrl_covMatrix_->setValue(realvec());
      }

      for(i=0; i<sizes_(0); ++i)
      {
        obs = 0;
        invecs_[0].getCol(i, i_featVec_);
        for(l=0; l<(mrs_natural)nfeats; l++)
        {
          stackedFeatVecs_(l,0) = i_featVec_(l);
        }
        for(j=1; j<sizes_.getSize(); j++)
        {
          for(k=0; k<sizes_(j); k++)
          {
            invecs_[j].getCol(k, j_featVec_);
            // stack i and j feat vectors
            for(l=0; l<(mrs_natural)nfeats; l++)
            {
              stackedFeatVecs_(l+nfeats,0) = j_featVec_(l);
            }
            marsystems_[0]->process(stackedFeatVecs_, metricResult_);
            out(k+obs,i) = metricResult_(0,0);
          }
          obs += (mrs_natural)sizes_(j);
        }
      }
    }
    else
    {
      out.setval(0.0);
      if(child_count == 0)
      {
        MRSWARN("SimilarityMatrix::myProcess - no Child Metric MarSystem added - outputting zero similarity matrix!");
      }
      else
      {
        MRSWARN("SimilarityMatrix::myProcess - more than one Child MarSystem exists (i.e. invalid metric) - outputting zero similarity matrix!");
      }
    }
  }
  //MATLAB_PUT(out, "simMat");
  //MATLAB_EVAL(name_+"=["+name_+",simMat(:)'];");
}
