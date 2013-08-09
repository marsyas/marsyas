#include "AveragingPattern.h"

using std::ostringstream;
using namespace Marsyas;

AveragingPattern::AveragingPattern(mrs_string name):MarSystem("AveragingPattern", name)
{
  addControls();
}

AveragingPattern::AveragingPattern(const AveragingPattern& a):MarSystem(a)
{
  ctrl_sizes_ = getctrl("mrs_realvec/sizes");
  ctrl_alignment_ = getctrl("mrs_realvec/alignment");
  ctrl_counts_ = getctrl("mrs_realvec/counts");
  ctrl_input_ = getctrl("mrs_bool/input");
  ctrl_countVector_ = getctrl("mrs_realvec/countVector");
  ctrl_setCountVector_ = getctrl("mrs_bool/setCountVector");
}

AveragingPattern::~AveragingPattern()
{
}

MarSystem*
AveragingPattern::clone() const
{
  return new AveragingPattern(*this);
}

void
AveragingPattern::addControls()
{
  addControl("mrs_realvec/sizes", realvec(), ctrl_sizes_);
  addControl("mrs_realvec/alignment", realvec(), ctrl_alignment_);
  addControl("mrs_realvec/counts", realvec(), ctrl_counts_);
  addControl("mrs_bool/input", true, ctrl_input_);
  addControl("mrs_realvec/countVector", realvec(), ctrl_countVector_);
  addControl("mrs_bool/setCountVector", false, ctrl_setCountVector_);
}

void
AveragingPattern::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  // The output is a same size matrix with the input

  // forward flow propagation
  if(ctrl_input_->to<mrs_bool>()) {
    ctrl_onObservations_->setValue(ctrl_inObservations_, NOUPDATE);
    ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
  } else {
    realvec formaxsize = ctrl_sizes_->to<mrs_realvec>();
    formaxsize(0) = 0;
    mrs_natural tmpNTL = ctrl_inObservations_->to<mrs_natural>()/formaxsize.getSize();
    ctrl_onObservations_->setValue(tmpNTL*(formaxsize.getSize()-1), NOUPDATE);
    ctrl_onSamples_->setValue(formaxsize.maxval(),NOUPDATE);
  }
  ctrl_osrate_->setValue(ctrl_osrate_, NOUPDATE);
  ostringstream oss;
  for(mrs_natural o=0; o<ctrl_onObservations_->to<mrs_natural>(); ++o)
    oss << "AveragingPattern_" << o << ",";
  ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);

  const realvec& tmpvec = ctrl_sizes_->to<mrs_realvec>();
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

  mrs_natural numVec = sizes_.getSize();
  mrs_natural dimVec = ctrl_inObservations_->to<mrs_natural>()/numVec;
  mrs_natural templateSize = 0;
  for(mrs_natural i=1; i<numVec; ++i)
  {
    templateSize += (mrs_natural)sizes_(i);
  }
  const realvec& tmpvec2 = ctrl_countVector_->to<mrs_realvec>();
  if(tmpvec2.getSize() > 0 && ctrl_setCountVector_->to<mrs_bool>())
    countvector_ = tmpvec2;
  else
    countvector_.create(templateSize);

  average_.create(dimVec,templateSize);
  counts_.create(numVec);
  beginPos_.create(numVec-1);
  endPos_.create(numVec-1);
  beginPos_(0) = 0;

  for(mrs_natural l=1; l<numVec-1; l++)
  {
    beginPos_(l) = sizes_(l) + beginPos_(l-1);
  }
  for(mrs_natural l=0; l<numVec-1; l++)
  {
    endPos_(l) = beginPos_(l) + sizes_(l+1);
  }
}

void
AveragingPattern::myProcess(realvec& in, realvec& out)
{

  mrs_natural j, k, l;
  if(inSamples_>0)
  {
    const realvec& alignment = ctrl_alignment_->to<mrs_realvec>();

    if(sizes_.getSize() > 0 && alignment.getSize() > 0)
    {
      mrs_natural numVec = sizes_.getSize();
      mrs_natural dimVec = ctrl_inObservations_->to<mrs_natural>()/numVec;
      mrs_natural templateSize = 0;
      mrs_natural tmpNatural = 0;
      for(mrs_natural i=1; i<numVec; ++i)
      {
        templateSize += (mrs_natural)sizes_(i);
      }

      if(!ctrl_setCountVector_->to<mrs_bool>())
      {
        for(mrs_natural i=0; i<countvector_.getSize(); ++i)
        {
          countvector_(i) = 0;
        }
      }
      for(mrs_natural i=0; i<counts_.getSize(); ++i) {
        counts_(i) = 0;
      }
      // initialize according to countVector
      if(ctrl_setCountVector_->to<mrs_bool>())
      {
        for(k=0; k<numVec-1; k++)
        {
          for(mrs_natural i=0; i<sizes_(k+1); ++i)
          {
            for(j=0; j<dimVec; j++)
            {
              average_(j,i+tmpNatural) = countvector_(i+tmpNatural)*out(j+dimVec*k,i);
            }
          }
          tmpNatural += (mrs_natural)sizes_(k+1);
        }
        tmpNatural = 0;
      }
      else
      {
        for(mrs_natural i=0; i<average_.getCols(); ++i)
        {
          for(j=0; j<dimVec; j++)
          {
            average_(j,i) = 0;
          }
        }
      }

      mrs_bool b_begin = false;

      // averaging
      for(k=0; k<alignment.getRows(); k++)
      {
        if(alignment(k,0)>=0 && alignment(k,1)>=0)
        {
          for(l=0; l<numVec-1; l++)
          {
            if(beginPos_(l) == alignment(k,1))
            {
              b_begin = true;
              break;
            }
            else if(endPos_(l)-1 == alignment(k,1))
            {
              if(b_begin)
                counts_(l+1) ++;
              b_begin = false;
              break;
            }
          }
          for(j=0; j<dimVec; j++)
          {
            average_(j,(mrs_natural)alignment(k,1)) += in(j,(mrs_natural)alignment(k,0));
          }
          countvector_((mrs_natural)alignment(k,1))++;
        }
      }
      for(mrs_natural n=0; n<templateSize; ++n)
      {
        if(countvector_(n)>0)
        {
          for(j=0; j<dimVec; j++)
          {
            average_(j,n) /= countvector_(n);
          }
        }
      }
      if(ctrl_input_->to<mrs_bool>())
      {
        for(mrs_natural i=0; i<sizes_(0); ++i)
        {
          for(j=0; j<dimVec; j++)
          {
            out(j,i) = in(j,i);
          }
        }
        l=0;
      }
      else
        l=1;
      for(k=1; k<numVec; k++)
      {
        for(mrs_natural n=0; n<sizes_(k); ++n)
        {
          if(countvector_(n+tmpNatural)>0)
          {
            for(j=0; j<dimVec; j++)
            {
              out(j+(k-l)*dimVec,n) = average_(j,n+tmpNatural);
            }
          }
          else
          {
            for(j=0; j<dimVec; j++)
            {
              out(j+(k-l)*dimVec,n) = in(j+k*dimVec,n);
            }
          }
        }
        tmpNatural += (mrs_natural)sizes_(k);
      }
      for(l=1; l<numVec; l++)
      {
        counts_(0) += counts_(l);
      }
      ctrl_counts_->setValue(counts_);
      ctrl_countVector_->setValue(countvector_);
    }
    else
    {
      MRSWARN("AveragingPattern:myProcess - invalid sizes and alignment vectors (does not output a real value)!");
    }
  }
}

