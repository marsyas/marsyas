#include "DTW.h"

using std::ostringstream;
using namespace Marsyas;

DTW::DTW(mrs_string name):MarSystem("DTW", name)
{
  addControls();
}

DTW::DTW(const DTW& a):MarSystem(a)
{
  ctrl_mode_ = getctrl("mrs_string/mode");
  ctrl_localPath_ = getctrl("mrs_string/localPath");
  ctrl_startPos_ = getctrl("mrs_string/startPos");
  ctrl_lastPos_ = getctrl("mrs_string/lastPos");
  ctrl_totalDis_ = getctrl("mrs_real/totalDistance");
  ctrl_sizes_ = getctrl("mrs_realvec/sizes");
  ctrl_weight_ = getctrl("mrs_bool/weight");
}

DTW::~DTW()
{
}

MarSystem*
DTW::clone() const
{
  return new DTW(*this);
}

void
DTW::addControls()
{
  totalDis_ = 0;
  addControl("mrs_string/mode", "normal", ctrl_mode_);
  addControl("mrs_string/localPath", "normal", ctrl_localPath_);
  addControl("mrs_string/startPos", "zero", ctrl_startPos_);
  addControl("mrs_string/lastPos", "end", ctrl_lastPos_);
  addControl("mrs_real/totalDistance", totalDis_, ctrl_totalDis_);
  addControl("mrs_realvec/sizes", realvec(), ctrl_sizes_);
  addControl("mrs_bool/weight", false, ctrl_weight_);
}

void DTW::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)

  ctrl_onSamples_->setValue(2,NOUPDATE);
  ctrl_onObservations_->setValue(ctrl_inSamples_+ctrl_inObservations_, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_osrate_,NOUPDATE);
  ostringstream oss;
  for(mrs_natural o=0; o<ctrl_onObservations_->to<mrs_natural>(); ++o)
    oss << "DTW_" << o << ",";
  ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);

  MarControlAccessor acc(ctrl_sizes_);
  realvec& tmpvec = acc.to<mrs_realvec>();
  if(tmpvec.getRows() == 1 && tmpvec.getCols() >= 2)
  {
    sizes_.create(tmpvec.getCols());
    for(mrs_natural i=0; i<tmpvec.getCols(); ++i)
    {
      sizes_(i) = (mrs_natural)tmpvec(0,i);
    }
  }
  else if(tmpvec.getRows() >= 2 && tmpvec.getCols() == 1)
  {
    sizes_.create(tmpvec.getRows());
    for(mrs_natural i=0; i<tmpvec.getRows(); ++i)
    {
      sizes_(i) = (mrs_natural)tmpvec(i,0);
    }
  }

  alignment_.create(ctrl_inObservations_->to<mrs_natural>(), ctrl_inSamples_->to<mrs_natural>());
  if(ctrl_localPath_->to<mrs_string>() == "normal")
  {
    costMatrix_.create(ctrl_inObservations_->to<mrs_natural>(), 2);
    matrixPos_.create(2);
  }
  else if(ctrl_localPath_->to<mrs_string>() == "diagonal")
  {
    costMatrix_.create(ctrl_inObservations_->to<mrs_natural>(), 3);
    matrixPos_.create(3);
  }
  if(ctrl_mode_->to<mrs_string>() == "OnePass")
  {
    mrs_natural nTemplates = sizes_.getSize()-1;
    beginPos_.create(nTemplates);
    endPos_.create(nTemplates);
    beginPos_(0) = 0;
    for(mrs_natural l=1; l<nTemplates; l++)
    {
      beginPos_(l) = sizes_(l) + beginPos_(l-1);
    }
    for(mrs_natural l=0; l<nTemplates; l++)
    {
      endPos_(l) = beginPos_(l) + sizes_(l+1);

    }
  }
}

void
DTW::myProcess(realvec& in, realvec& out)
{
  mrs_natural i, j, k, l;
  j = 0;

  mrs_real nObs = in.getRows();
  mrs_real nSmp = in.getCols();
  mrs_real tmpReal = 0.0;
  mrs_bool weight = ctrl_weight_->to<mrs_bool>();

  if(inSamples_ > 0)
  {
    if(ctrl_mode_->to<mrs_string>() == "normal")
    {
      if(ctrl_localPath_->to<mrs_string>() == "normal" || ((nSmp > 2*nObs || nObs > 2*nSmp) && ctrl_localPath_->to<mrs_string>() == "diagonal"))
      {
        if((nSmp > 2*nObs || nObs > 2*nSmp) && ctrl_localPath_->to<mrs_string>() == "diagonal") {
          MRSWARN("DTW::myProcess - invalid local path control: diagonal (processes with normal local path)");
        }

        for(i=0; i<2; ++i)
        {
          matrixPos_(i) = i;
        }
        // |vertical:1, /diagonal:2, _horizonal:3

        if(ctrl_startPos_->to<mrs_string>() == "zero")
        {
          // copying first SimilarityMatrix
          costMatrix_(0,(mrs_natural)matrixPos_(0)) = in(0,0);
          alignment_(0,0) = 0;
          // calculating other cost of the first col
          for(j=1; j<nObs; j++)
          {
            costMatrix_(j,(mrs_natural)matrixPos_(0)) = in(j,0)+costMatrix_(j-1,(mrs_natural)matrixPos_(0));
            alignment_(j,0) = 1;
          }
        }
        else if(ctrl_startPos_->to<mrs_string>() == "lowest")
        {
          // copying first col of SimilarityMatrix
          for(j=0; j<nObs; j++)
          {
            costMatrix_(j, (mrs_natural)matrixPos_(0)) = in(j,0);
            alignment_(j,0) = 0;
          }
        }
        // after first col
        for(i=1; i<nSmp; ++i)
        {
          costMatrix_(0,(mrs_natural)matrixPos_(1)) = costMatrix_(0,(mrs_natural)matrixPos_(0)) + in(0,i);
          alignment_(0,i) = 3;
          for(j=1; j<nObs; j++)
          {
            costMatrix_(j,(mrs_natural)matrixPos_(1)) = costMatrix_(j-1,(mrs_natural)matrixPos_(1)) + in(j,i);
            alignment_(j,i) = 1;
            tmpReal = costMatrix_(j-1,(mrs_natural)matrixPos_(0)) + in(j,i);
            if(weight)
              tmpReal += in(j,i);
            if(tmpReal < costMatrix_(j,(mrs_natural)matrixPos_(1)))
            {
              costMatrix_(j,(mrs_natural)matrixPos_(1)) = tmpReal;
              alignment_(j,i) = 2;
            }
            tmpReal = costMatrix_(j,(mrs_natural)matrixPos_(0)) + in(j,i);
            if(tmpReal < costMatrix_(j,(mrs_natural)matrixPos_(1)))
            {
              costMatrix_(j,(mrs_natural)matrixPos_(1)) = tmpReal;
              alignment_(j,i) = 3;
            }
          }
          matrixPos_(0) = 1-matrixPos_(0);
          matrixPos_(1) = 1-matrixPos_(1);
        }

        // backtrace
        for(i=0; i<out.getRows(); ++i)
        {
          for(j=0; j<out.getCols(); j++)
          {
            out(i,j) = -1;
          }
        }
        if(ctrl_lastPos_->to<mrs_string>() == "end")
        {
          totalDis_ = costMatrix_((mrs_natural)(nObs-1),(mrs_natural)matrixPos_(0));
          ctrl_totalDis_->setValue(totalDis_);
          i = (mrs_natural)(nSmp-1);
          j = (mrs_natural)(nObs-1);
        }
        else if(ctrl_lastPos_->to<mrs_string>() == "lowest")
        {
          tmpReal = costMatrix_(0, (mrs_natural)matrixPos_(0));
          j = 0;
          for(i=1; i<nObs; ++i)
          {
            if(costMatrix_(i, (mrs_natural)matrixPos_(0)) < tmpReal)
            {
              tmpReal = costMatrix_(i, (mrs_natural)matrixPos_(0));
              j = i;
            }
          }
          i = (mrs_natural)(nSmp-1);
          totalDis_ = tmpReal;
          ctrl_totalDis_->setValue(totalDis_);
        }
        k = (mrs_natural)(nSmp + nObs - 1);
        while(alignment_(j,i) != 0 && k>=0)
        {
          if(alignment_(j,i) == 1)
          {
            out(k,0) = i;
            out(k,1) = j;
            j--;
            k--;
          }
          else if(alignment_(j,i) == 2)
          {
            out(k,0) = i;
            out(k,1) = j;
            k--;
            if(weight)
            {
              out(k,0) = i;
              out(k,1) = j;
              k--;
            }
            i--;
            j--;
          }
          else if(alignment_(j,i) == 3)
          {
            out(k,0) = i;
            out(k,1) = j;
            k--;
            i--;
          }
        }
        out(k,0) = i;
        out(k,1) = j;
      }

      else if(ctrl_localPath_->to<mrs_string>() == "diagonal")
      {
        for(i=0; i<3; ++i)
        {
          matrixPos_(i) = i;
        }
        // /|diagonal,vertical:1, /diagonal:2, /-diagonal,horizonal:3

        if(ctrl_startPos_->to<mrs_string>() == "zero")
        {
          // copying the first SimilarityMatrix
          costMatrix_(0,(mrs_natural)matrixPos_(0)) = in(0,0);
          alignment_(0,0) = 0;
          // calculating the second col
          costMatrix_(1,(mrs_natural)matrixPos_(1)) = costMatrix_(0,(mrs_natural)matrixPos_(0)) + in(1,1);
          if(weight)
            costMatrix_(1,(mrs_natural)matrixPos_(1)) += in(1,1);
          costMatrix_(2,(mrs_natural)matrixPos_(1)) = costMatrix_(0,(mrs_natural)matrixPos_(0)) + in(1,1) + in(2,1);
          if(weight)
            costMatrix_(2,(mrs_natural)matrixPos_(1)) += in(1,1);
          alignment_(1,1) = 2;
          alignment_(2,1) = 1;
          // calculating the third col
          costMatrix_(1,(mrs_natural)matrixPos_(2)) = costMatrix_(0,(mrs_natural)matrixPos_(0)) + in(1,1) + in(1,2);
          if(weight)
            costMatrix_(1,(mrs_natural)matrixPos_(2)) += in(1,1);
          alignment_(1,2) = 3;
          costMatrix_(2,(mrs_natural)matrixPos_(2)) = costMatrix_(1,(mrs_natural)matrixPos_(1)) + in(2,2);
          if(weight)
            costMatrix_(2,(mrs_natural)matrixPos_(2)) += in(2,2);
          alignment_(2,2) = 2;
          costMatrix_(3,(mrs_natural)matrixPos_(2)) = costMatrix_(2,(mrs_natural)matrixPos_(1)) + in(3,2);
          if(weight)
            costMatrix_(3,(mrs_natural)matrixPos_(2)) += in(3,2);
          alignment_(3,2) = 2;
          tmpReal = costMatrix_(1,(mrs_natural)matrixPos_(1)) + in(2,2) + in(3,2);
          if(weight)
            tmpReal += in(2,2);
          if(tmpReal < costMatrix_(3,(mrs_natural)matrixPos_(2)))
          {
            costMatrix_(3,(mrs_natural)matrixPos_(2)) = tmpReal;
            alignment_(3,2) = 1;
          }
        }
        else if(ctrl_startPos_->to<mrs_string>() == "lowest")
        {
          // copying first col of SimilarityMatrix
          for(j=0; j<nObs; j++)
          {
            costMatrix_(j, (mrs_natural)matrixPos_(0)) = in(j,0);
            alignment_(j,0) = 0;
          }
          // calculating the second col
          costMatrix_(1,(mrs_natural)matrixPos_(1)) = costMatrix_(0,(mrs_natural)matrixPos_(0)) + in(1,1);
          if(weight)
            costMatrix_(1,(mrs_natural)matrixPos_(1)) += in(1,1);
          alignment_(1,1) = 2;
          for(j=2; j<nObs; j++)
          {
            costMatrix_(j,(mrs_natural)matrixPos_(1)) = costMatrix_(j-1,(mrs_natural)matrixPos_(0)) + in(j,1);
            if(weight)
              costMatrix_(j,(mrs_natural)matrixPos_(1)) += in(j,1);
            alignment_(j,1) = 2;
            tmpReal = costMatrix_(j-2,(mrs_natural)matrixPos_(0)) + in(j-1,1) + in(j,1);
            if(weight)
              tmpReal += in(j-1,1);
            if(tmpReal < costMatrix_(j,(mrs_natural)matrixPos_(1)))
            {
              costMatrix_(j,(mrs_natural)matrixPos_(1)) = tmpReal;
              alignment_(j,1) = 1;
            }
          }
          // calculating the third col
          costMatrix_(1,(mrs_natural)matrixPos_(2)) = costMatrix_(0,(mrs_natural)matrixPos_(0)) + in(1,1) + in(1,2);
          if(weight)
            costMatrix_(1,(mrs_natural)matrixPos_(2)) += in(1,1);
          alignment_(1,2) = 3;
          for(j=2; j<nObs; j++)
          {
            costMatrix_(j,(mrs_natural)matrixPos_(2)) = costMatrix_(j-1,(mrs_natural)matrixPos_(1)) + in(j,2);
            if(weight)
              costMatrix_(j,(mrs_natural)matrixPos_(2)) += in(j,2);
            alignment_(j,2) = 2;
            if(alignment_(j-2,2) != 0)
            {
              tmpReal = costMatrix_(j-2,(mrs_natural)matrixPos_(1)) + in(j-1,2) + in(j,2);
              if(weight)
                tmpReal += in(j-1,2);
              if(tmpReal < costMatrix_(j,(mrs_natural)matrixPos_(2)))
              {
                costMatrix_(j,(mrs_natural)matrixPos_(2));
                alignment_(j,2) = 1;
              }
            }
            tmpReal = costMatrix_(j-1,(mrs_natural)matrixPos_(0)) + in(j,1) + in(j,2);
            if(weight)
              tmpReal += in(j,1);
            if(tmpReal < costMatrix_(j,(mrs_natural)matrixPos_(2)))
            {
              costMatrix_(j,(mrs_natural)matrixPos_(2)) = tmpReal;
              alignment_(j,2) = 3;
            }
          }
        }
        for(i=0; i<3; ++i)
        {
          matrixPos_(i)++;
          if(matrixPos_(i)>=3)
            matrixPos_(i) = 0;
        }
        // after third col
        for(i=3; i<nSmp; ++i)
        {
          for(j=2; j<nObs; j++)
          {
            if(alignment_(j-1,i-2) != 0)
            {
              costMatrix_(j,(mrs_natural)matrixPos_(2)) = costMatrix_(j-1,(mrs_natural)matrixPos_(0)) + in(j,i-1) + in(j,i);
              if(weight)
                costMatrix_(j,(mrs_natural)matrixPos_(2)) += in(j,i-1);
              alignment_(j,i) = 3;
              if(alignment_(j-1,i-1) != 0)
              {
                tmpReal = costMatrix_(j-1,(mrs_natural)matrixPos_(1)) + in(j,i);
                if(weight)
                  tmpReal += in(j,i);
                if(tmpReal < costMatrix_(j,(mrs_natural)matrixPos_(2)))
                {
                  costMatrix_(j,(mrs_natural)matrixPos_(2)) = tmpReal;
                  alignment_(j,i) = 2;
                }
              }
              if(alignment_(j-2,i-1) != 0)
              {
                tmpReal = costMatrix_(j-2,(mrs_natural)matrixPos_(1)) + in(j-1,i) + in(j,i);
                if(weight)
                  tmpReal += in(j-1,i);
                if(tmpReal < costMatrix_(j,(mrs_natural)matrixPos_(2)))
                {
                  costMatrix_(j,(mrs_natural)matrixPos_(2)) = tmpReal;
                  alignment_(j,i) = 1;
                }
              }
            }
            else if(alignment_(j-1,i-1) != 0)
            {
              costMatrix_(j,(mrs_natural)matrixPos_(2)) = costMatrix_(j-1,(mrs_natural)matrixPos_(1)) + in(j,i);
              if(weight)
                costMatrix_(j,(mrs_natural)matrixPos_(2)) += in(j,i);
              alignment_(j,i) = 2;
              if(alignment_(j-2,i-1) != 0)
              {
                tmpReal = costMatrix_(j-2,(mrs_natural)matrixPos_(1)) + in(j-1,i) + in(j,i);
                if(weight)
                  tmpReal += in(j-1,i);
                alignment_(j,i) = 1;
              }
            }
            else if(alignment_(j-2,i-1) != 0)
            {
              costMatrix_(j,(mrs_natural)matrixPos_(2)) = costMatrix_(j-2,(mrs_natural)matrixPos_(1)) + in(j-1,i) + in(j,i);
              if(weight)
                costMatrix_(j,(mrs_natural)matrixPos_(2)) += in(j-1,i);
              alignment_(j,i) = 1;
            }
          }
          for(j=0; j<3; j++)
          {
            matrixPos_(j)++;
            if(matrixPos_(j) >= 3)
              matrixPos_(j) = 0;
          }
        }

        // backtrace
        for(i=0; i<out.getRows(); ++i)
        {
          for(j=0; j<out.getCols(); j++)
          {
            out(i,j) = -1;
          }
        }
        if(ctrl_lastPos_->to<mrs_string>() == "end")
        {
          totalDis_ = costMatrix_((mrs_natural)(nObs-1),(mrs_natural)matrixPos_(1));
          ctrl_totalDis_->setValue(totalDis_);
          i = (mrs_natural)(nSmp-1);
          j = (mrs_natural)(nObs-1);
        }
        else if(ctrl_lastPos_->to<mrs_string>() == "lowest")
        {
          tmpReal = costMatrix_((mrs_natural)(nObs-1), (mrs_natural)matrixPos_(1));
          j = (mrs_natural)(nObs-1);
          for(i=0; i<nObs-1; ++i)
          {
            if(costMatrix_(i, (mrs_natural)matrixPos_(1)) < tmpReal && alignment_(i,(mrs_natural)(nSmp-1)) != 0)
            {
              tmpReal = costMatrix_(i, (mrs_natural)matrixPos_(1));
              j = i;
            }
          }
          i = (mrs_natural)(nSmp-1);
          totalDis_ = tmpReal;
          ctrl_totalDis_->setValue(totalDis_);
        }
        k = (mrs_natural)(nSmp + nObs - 1);
        while(alignment_(j,i) != 0 && k>=0)
        {
          if(alignment_(j,i) == 1)
          {
            out(k,0) = i;
            out(k,1) = j;
            j--;
            k--;
            out(k,0) = i;
            out(k,1) = j;
            k--;
            if(weight)
            {
              out(k,0) = i;
              out(k,1) = j;
              k--;
            }
            i--;
            j--;
          }
          else if(alignment_(j,i) == 2)
          {
            out(k,0) = i;
            out(k,1) = j;
            k--;
            if(weight)
            {
              out(k,0) = i;
              out(k,1) = j;
              k--;
            }
            i--;
            j--;
          }
          else if(alignment_(j,i) == 3)
          {
            out(k,0) = i;
            out(k,1) = j;
            k--;
            i--;
            out(k,0) = i;
            out(k,1) = j;
            k--;
            if(weight)
            {
              out(k,0) = i;
              out(k,1) = j;
              k--;
            }
            i--;
            j--;
          }
        }
        out(k,0) = i;
        out(k,1) = j;
      }
    }

    else if(ctrl_mode_->to<mrs_string>() == "OnePass")
    {
      mrs_natural nTemplates = sizes_.getSize()-1;
      if(sizes_.getSize() > 0)
      {

        if(ctrl_localPath_->to<mrs_string>() == "normal")
        {
          for(i=0; i<2; ++i)
          {
            matrixPos_(i) = i;
          }
          // |vertical:1, /diagonal:2, _horizonal:3

          if(ctrl_startPos_->to<mrs_string>() == "zero")
          {
            // copying first SimilarityMatrix
            for(l=0; l<nTemplates; l++)
            {
              costMatrix_((mrs_natural)beginPos_(l),(mrs_natural)matrixPos_(0)) = in((mrs_natural)beginPos_(l),0);
              alignment_((mrs_natural)beginPos_(l),0) = 0;
            }
            // calculating other cost of the first col
            for(l=0; l<nTemplates; l++)
            {
              for(j=(mrs_natural)beginPos_(l)+1; j<endPos_(l); j++)
              {
                costMatrix_(j,(mrs_natural)matrixPos_(0)) = in(j,0) + costMatrix_(j-1,(mrs_natural)matrixPos_(0));
                alignment_(j,0) = 1;
              }
            }
          }
          else if(ctrl_startPos_->to<mrs_string>() == "lowest")
          {
            // copying first col of SimilarityMatrix
            for(j=0; j<nObs; j++)
            {
              costMatrix_(j,(mrs_natural)matrixPos_(0)) = in(j,0);
              alignment_(j,0) = 0;
            }
          }
          // after first col
          for(i=1; i<nSmp; ++i)
          {
            for(l=0; l<nTemplates; l++)
            {
              costMatrix_((mrs_natural)beginPos_(l),(mrs_natural)matrixPos_(1)) = costMatrix_((mrs_natural)endPos_(l)-1,(mrs_natural)matrixPos_(0)) + in((mrs_natural)beginPos_(l),i);
              if(weight)
                costMatrix_((mrs_natural)beginPos_(l),(mrs_natural)matrixPos_(1)) += in((mrs_natural)beginPos_(l),i);
              alignment_((mrs_natural)beginPos_(l),i) = -1*(endPos_(l)-1);
              tmpReal = costMatrix_((mrs_natural)beginPos_(l),(mrs_natural)matrixPos_(0)) + in((mrs_natural)beginPos_(l),i);
              if(tmpReal < costMatrix_((mrs_natural)beginPos_(l),(mrs_natural)matrixPos_(1)))
              {
                costMatrix_((mrs_natural)beginPos_(l),(mrs_natural)matrixPos_(1)) = tmpReal;
                alignment_((mrs_natural)beginPos_(l), i) = 3;
              }
              for(j=(mrs_natural)beginPos_(l)+1; j<(mrs_natural)endPos_(l); j++)
              {
                costMatrix_(j,(mrs_natural)matrixPos_(1)) = costMatrix_(j-1,(mrs_natural)matrixPos_(1)) + in(j,i);
                alignment_(j,i) = 1;
                tmpReal = costMatrix_(j-1,(mrs_natural)matrixPos_(0)) + in(j,i);
                if(weight)
                  tmpReal += in(j,i);
                if(tmpReal < costMatrix_(j,(mrs_natural)matrixPos_(1)))
                {
                  costMatrix_(j,(mrs_natural)matrixPos_(1)) = tmpReal;
                  alignment_(j,i) = 2;
                }
                tmpReal = costMatrix_(j,(mrs_natural)matrixPos_(0)) + in(j,i);
                if(tmpReal < costMatrix_(j,(mrs_natural)matrixPos_(1)))
                {
                  costMatrix_(j,(mrs_natural)matrixPos_(1)) = tmpReal;
                  alignment_(j,i) = 3;
                }
              }
            }
            matrixPos_(0) = 1-matrixPos_(0);
            matrixPos_(1) = 1-matrixPos_(1);
          }

          // backtrace
          for(i=0; i<out.getRows(); ++i)
          {
            for(j=0; j<out.getCols(); j++)
            {
              out(i,j) = -1;
            }
          }
          if(ctrl_lastPos_->to<mrs_string>() == "end")
          {
            tmpReal = costMatrix_((mrs_natural)endPos_(0)-1,(mrs_natural)matrixPos_(0));
            j = (mrs_natural)endPos_(0)-1;
            for(l=1; l<nTemplates; l++)
            {
              if(costMatrix_((mrs_natural)endPos_(l)-1,(mrs_natural)matrixPos_(0)) < tmpReal)
              {
                tmpReal = costMatrix_((mrs_natural)endPos_(l)-1,(mrs_natural)matrixPos_(0));
                j = (mrs_natural)endPos_(l)-1;
              }
            }
            totalDis_ = tmpReal;
            ctrl_totalDis_->setValue(totalDis_);
            i = (mrs_natural)(nSmp-1);
          }
          else if(ctrl_lastPos_->to<mrs_string>() == "lowest")
          {
            tmpReal = costMatrix_(0, (mrs_natural)matrixPos_(0));
            j=0;
            for(i=1; i<nObs; ++i)
            {
              if(costMatrix_(i,(mrs_natural)matrixPos_(0)) < tmpReal)
              {
                tmpReal = costMatrix_(i, (mrs_natural)matrixPos_(0));
                j = i;
              }
            }
            i = (mrs_natural)nSmp-1;
            totalDis_ = tmpReal;
            ctrl_totalDis_->setValue(totalDis_);
          }
          k = (mrs_natural)(3*nSmp - 1);//+ nObs - 1;
          while(alignment_(j,i) != 0 && k>=0)
          {
            if(alignment_(j,i) == 1)
            {
              out(k,0) = i;
              out(k,1) = j;
              j--;
              k--;
            }
            else if(alignment_(j,i) == 2)
            {
              out(k,0) = i;
              out(k,1) = j;
              k--;
              if(weight)
              {
                out(k,0) = i;
                out(k,1) = j;
                k--;
              }
              i--;
              j--;
            }
            else if(alignment_(j,i) == 3)
            {
              out(k,0) = i;
              out(k,1) = j;
              k--;
              i--;
            }
            else if(alignment_(j,i) < 0)
            {
              out(k,0) = i;
              out(k,1) = j;
              k--;
              if(weight)
              {
                out(k,0) = i;
                out(k,1) = j;
                k--;
              }
              j = -1*(mrs_natural)alignment_(j,i);
              i--;
            }
          }
          out(k,0) = i;
          out(k,1) = j;
        }
        else if(ctrl_localPath_->to<mrs_string>() == "diagonal")
        {
          for(i=0; i<3; ++i)
          {
            matrixPos_(i) = i;
          }
          // /|diagonal,vertical:1, /diagonal:2, /-diagonal,holizonal:3

          if(ctrl_startPos_->to<mrs_string>() == "zero")
          {
            // copying first SimilarityMatrix
            for(l=0; l<nTemplates; l++)
            {
              costMatrix_((mrs_natural)beginPos_(l),(mrs_natural)matrixPos_(0)) = in((mrs_natural)beginPos_(l),0);
              alignment_((mrs_natural)beginPos_(l),0) = 0;
            }
            // calculating the second col
            for(l=0; l<nTemplates; l++)
            {
              costMatrix_((mrs_natural)beginPos_(l)+1,(mrs_natural)matrixPos_(1)) = costMatrix_((mrs_natural)beginPos_(l),(mrs_natural)matrixPos_(0)) + in((mrs_natural)beginPos_(l)+1,1);
              if(weight)
                costMatrix_((mrs_natural)beginPos_(l)+1,(mrs_natural)matrixPos_(1)) += in((mrs_natural)beginPos_(l)+1,1);
              costMatrix_((mrs_natural)beginPos_(l)+2,(mrs_natural)matrixPos_(1)) = costMatrix_((mrs_natural)beginPos_(l),(mrs_natural)matrixPos_(0)) + in((mrs_natural)beginPos_(l)+1,1) + in((mrs_natural)beginPos_(l)+2,1);
              if(weight)
                costMatrix_((mrs_natural)beginPos_(l)+2,(mrs_natural)matrixPos_(1)) += in((mrs_natural)beginPos_(l)+1,1);
              alignment_((mrs_natural)beginPos_(l)+1,1) = 2;
              alignment_((mrs_natural)beginPos_(l)+2,1) = 1;
            }
            // calculating the third col
            for(l=0; l<nTemplates; l++)
            {
              costMatrix_((mrs_natural)beginPos_(l)+1,(mrs_natural)matrixPos_(2)) = costMatrix_((mrs_natural)beginPos_(l),(mrs_natural)matrixPos_(0)) + in((mrs_natural)beginPos_(l)+1,1) + in((mrs_natural)beginPos_(l)+1,2);
              if(weight)
                costMatrix_((mrs_natural)beginPos_(l)+1,(mrs_natural)matrixPos_(2)) += in((mrs_natural)beginPos_(l)+1,1);
              alignment_((mrs_natural)beginPos_(l)+1,2) = 3;
              costMatrix_((mrs_natural)beginPos_(l)+2,(mrs_natural)matrixPos_(2)) = costMatrix_((mrs_natural)beginPos_(l)+1,(mrs_natural)matrixPos_(1)) + in((mrs_natural)beginPos_(l)+2,2);
              if(weight)
                costMatrix_((mrs_natural)beginPos_(l)+2,(mrs_natural)matrixPos_(2)) += in((mrs_natural)beginPos_(l)+2,2);
              alignment_((mrs_natural)beginPos_(l)+2,2) = 2;
              costMatrix_((mrs_natural)beginPos_(l)+3,(mrs_natural)matrixPos_(2)) = costMatrix_((mrs_natural)beginPos_(l)+2,(mrs_natural)matrixPos_(1)) + in((mrs_natural)beginPos_(l)+3,2);
              if(weight)
                costMatrix_((mrs_natural)beginPos_(l)+3,(mrs_natural)matrixPos_(2)) += in((mrs_natural)beginPos_(l)+3,2);
              alignment_((mrs_natural)beginPos_(l)+3,2) = 2;
              tmpReal = costMatrix_((mrs_natural)beginPos_(l)+1,(mrs_natural)matrixPos_(1)) + in((mrs_natural)beginPos_(l)+2,2) + in((mrs_natural)beginPos_(l)+3,2);
              if(weight)
                tmpReal += in((mrs_natural)beginPos_(l)+2,2);
              if(tmpReal < costMatrix_((mrs_natural)beginPos_(l)+3,(mrs_natural)matrixPos_(2)))
              {
                costMatrix_((mrs_natural)beginPos_(l)+3,(mrs_natural)matrixPos_(2)) = tmpReal;
                alignment_((mrs_natural)beginPos_(l)+3,2) = 1;
              }
            }
          }
          else if(ctrl_startPos_->to<mrs_string>() == "lowest")
          {
            // copying first col of SimilarityMatrix
            for(j=0; j<nObs; j++)
            {
              costMatrix_(j, (mrs_natural)matrixPos_(0)) = in(j,0);
              alignment_(j,0) = 0;
            }
            // calculating the second col
            tmpReal = costMatrix_((mrs_natural)endPos_(0)-1,(mrs_natural)matrixPos_(0));
            j=(mrs_natural)endPos_(0)-1;
            for(l=1; l<nTemplates; l++)
            {
              if(costMatrix_((mrs_natural)endPos_(l)-1,(mrs_natural)matrixPos_(0)) < tmpReal)
              {
                tmpReal = costMatrix_((mrs_natural)endPos_(l)-1,(mrs_natural)matrixPos_(0));
                j=(mrs_natural)endPos_(l)-1;
              }
            }
            for(l=0; l<nTemplates; l++)
            {
              costMatrix_((mrs_natural)beginPos_(l),(mrs_natural)matrixPos_(1)) = costMatrix_(j,(mrs_natural)matrixPos_(0)) + in((mrs_natural)beginPos_(l),1);
              if(weight)
                costMatrix_((mrs_natural)beginPos_(l),(mrs_natural)matrixPos_(1)) += in((mrs_natural)beginPos_(l),1);
              alignment_((mrs_natural)beginPos_(l),1) = -1*j;
            }
            for(l=0; l<nTemplates; l++)
            {
              costMatrix_((mrs_natural)beginPos_(l)+1,(mrs_natural)matrixPos_(1)) = costMatrix_((mrs_natural)beginPos_(l),(mrs_natural)matrixPos_(0)) + in((mrs_natural)beginPos_(l)+1,1);
              if(weight)
                costMatrix_((mrs_natural)beginPos_(l)+1,(mrs_natural)matrixPos_(1)) += in((mrs_natural)beginPos_(l)+1,1);
              alignment_((mrs_natural)beginPos_(l)+1,1) = 2;
              for(j=(mrs_natural)beginPos_(l)+2; j<(mrs_natural)endPos_(l); j++)
              {
                costMatrix_(j,(mrs_natural)matrixPos_(1)) = costMatrix_(j-1,(mrs_natural)matrixPos_(0)) + in(j,1);
                if(weight)
                  costMatrix_(j,(mrs_natural)matrixPos_(1)) += in(j,1);
                alignment_(j,1) = 2;
                tmpReal = costMatrix_(j-2,(mrs_natural)matrixPos_(0)) + in(j-1,1) + in(j,1);
                if(weight)
                  tmpReal += in(j-1,1);
                if(tmpReal < costMatrix_(j,(mrs_natural)matrixPos_(1)))
                {
                  costMatrix_(j,(mrs_natural)matrixPos_(1)) = tmpReal;
                  alignment_(j,1) = 1;
                }
              }
            }
            // calculating the third col
            tmpReal = costMatrix_((mrs_natural)endPos_(0)-1,(mrs_natural)matrixPos_(1));
            j=(mrs_natural)endPos_(0)-1;
            for(l=1; l<nTemplates; l++)
            {
              if(costMatrix_((mrs_natural)endPos_(l)-1,(mrs_natural)matrixPos_(1)) < tmpReal)
              {
                tmpReal = costMatrix_((mrs_natural)endPos_(l)-1, (mrs_natural)matrixPos_(1));
                j=(mrs_natural)endPos_(l)-1;
              }
            }
            for(l=0; l<nTemplates; l++)
            {
              costMatrix_((mrs_natural)beginPos_(l),(mrs_natural)matrixPos_(2)) = costMatrix_(j,(mrs_natural)matrixPos_(1)) + in((mrs_natural)beginPos_(l),2);
              if(weight)
                costMatrix_((mrs_natural)beginPos_(l),(mrs_natural)matrixPos_(2)) += in((mrs_natural)beginPos_(l),2);
              alignment_((mrs_natural)beginPos_(l),2) = -1*j;
            }
            //tmpReal = costMatrix_(endPos_(0)-1,matrixPos_(1));
            //j=endPos_(0)-1;
            //for(l=1; l<nTemplates; l++)
            //{
            //  if(costMatrix_(endPos_(l)-1,matrixPos_(1)) < tmpReal)
            //    {
            //      tmpReal = costMatrix_(endPos_(l)-1, matrixPos_(1));
            //      j=endPos_(l)-1;
            //    }
            //}
            //for(l=0; l<nTemplates; l++)
            //{
            //  tmpReal = costMatrix_(j,matrixPos_(1)) + 2.0*in(beginPos_(l),matrixPos_(2));
            //  if(tmpReal < costMatrix_(beginPos_(l),matrixPos_(2)))
            //    {
            //      costMatrix_(beginPos_(l),matrixPos_(2)) = tmpReal;
            //      alignment_(beginPos_(l),2) = -1*j;
            //    }
            //}
            for(l=0; l<nTemplates; l++)
            {
              costMatrix_((mrs_natural)beginPos_(l)+1,(mrs_natural)matrixPos_(2)) = costMatrix_((mrs_natural)beginPos_(l),(mrs_natural)matrixPos_(1)) + in((mrs_natural)beginPos_(l)+1,2);
              if(weight)
                costMatrix_((mrs_natural)beginPos_(l)+1,(mrs_natural)matrixPos_(2)) += in((mrs_natural)beginPos_(l)+1,2);
              alignment_((mrs_natural)beginPos_(l)+1,2) = 2;
              tmpReal = costMatrix_((mrs_natural)beginPos_(l),(mrs_natural)matrixPos_(0)) + in((mrs_natural)beginPos_(l)+1,1) + in((mrs_natural)beginPos_(l)+1,2);
              if(weight)
                tmpReal += in((mrs_natural)beginPos_(l)+1,1);
              if(tmpReal < costMatrix_((mrs_natural)beginPos_(l)+1,(mrs_natural)matrixPos_(2)))
              {
                costMatrix_((mrs_natural)beginPos_(l)+1,(mrs_natural)matrixPos_(2)) = tmpReal;
                alignment_((mrs_natural)beginPos_(l)+1,2) = 3;
              }
              for(j=(mrs_natural)beginPos_(l)+2; j<(mrs_natural)endPos_(l); j++)
              {
                costMatrix_(j,(mrs_natural)matrixPos_(2)) = costMatrix_(j-1,(mrs_natural)matrixPos_(0)) + in(j,1) + in(j,2);
                if(weight)
                  costMatrix_(j,(mrs_natural)matrixPos_(2)) += in(j,1);
                alignment_(j,2) = 3;
                tmpReal = costMatrix_(j-1,(mrs_natural)matrixPos_(1)) + in(j,2);
                if(weight)
                  tmpReal += in(j,2);
                if(tmpReal < costMatrix_(j,(mrs_natural)matrixPos_(2)))
                {
                  costMatrix_(j,(mrs_natural)matrixPos_(2)) = tmpReal;
                  alignment_(j,2) = 2;
                }
                tmpReal = costMatrix_(j-2,(mrs_natural)matrixPos_(1)) + in(j-1,2) + in(j,2);
                if(weight)
                  tmpReal += in(j-1,2);
                if(tmpReal < costMatrix_(j,(mrs_natural)matrixPos_(2)))
                {
                  costMatrix_(j,(mrs_natural)matrixPos_(2)) = tmpReal;
                  alignment_(j,2) = 1;
                }
              }
            }
          }
          for(i=0; i<3; ++i)
          {
            matrixPos_(i)++;
            if(matrixPos_(i)>=3)
              matrixPos_(i) = 0;
          }
          // after third col
          for(i=3; i<nSmp; ++i)
          {
            j = -1;
            for(l=0; l<nTemplates; l++)
            {
              if(alignment_((mrs_natural)endPos_(l)-1,i-1) != 0)
              {
                if(j<0 || (j>=0&&costMatrix_((mrs_natural)endPos_(l)-1,(mrs_natural)matrixPos_(1))<tmpReal))
                {
                  tmpReal = costMatrix_((mrs_natural)endPos_(l)-1,(mrs_natural)matrixPos_(1));
                  j = (mrs_natural)endPos_(l)-1;
                }
              }
            }
            if(j>=0)
            {
              for(l=0; l<nTemplates; l++)
              {
                costMatrix_((mrs_natural)beginPos_(l),(mrs_natural)matrixPos_(2)) = costMatrix_(j,(mrs_natural)matrixPos_(1)) + in((mrs_natural)beginPos_(l),i);
                if(weight)
                  costMatrix_((mrs_natural)beginPos_(l),(mrs_natural)matrixPos_(2)) += in((mrs_natural)beginPos_(l),i);
                alignment_((mrs_natural)beginPos_(l),i) = -1*j;
              }
              //j = -1;
              //for(l=0; l<nTemplates; l++)
              //  {
              //    if(alignment_(endPos_(l)-1,i-2) != 0)
              //	{
              //if(j<0 || (j>=0&&costMatrix_(endPos_(l)-1,matrixPos_(0))<tmpReal))
              //    {
              //      tmpReal = costMatrix_(endPos_(l)-1,matrixPos_(0));
              //      j = endPos_(l)-1;
              //    }
              //}
              //}
              //if(j>=0)
              //  {
              //    for(l=0; l<nTemplates; l++)
              //	{
              //	  tmpReal = costMatrix_(j,matrixPos_(0)) + 2.0*in(beginPos_(l),i-1) + in(beginPos_(l),i);
              //	  if(tmpReal < costMatrix_(beginPos_(l),matrixPos_(2)))
              //	    {
              //	      costMatrix_(beginPos_(l),matrixPos_(2)) = tmpReal;
              //	      alignment_(beginPos_(l),i) = -1*j;
              //	    }
              //	}
              //}
            }/*
		      else
			{
			  j = -1;
			  for(l=0; l<nTemplates; l++)
			    {
			      if(alignment_(endPos_(l)-1,i-2) != 0)
				{
				  if(j<0 || (j>=0&&costMatrix_(endPos_(l)-1,matrixPos_(0))<tmpReal))
				    {
				      tmpReal = costMatrix_(endPos_(l)-1,matrixPos_(0));
				      j = endPos_(l)-1;
				    }
				}
			    }
			  if(j>=0)
			    {
			      for(l=0; l<nTemplates; l++)
				{
				  costMatrix_(beginPos_(l),matrixPos_(2)) = costMatrix_(j,matrixPos_(0)) + 2.0*in(beginPos_(l),i-1) + in(beginPos_(l),i);
				  alignment_(beginPos_(l),i) = -1*j;
				}
			    }
			    }*/
            for(l=0; l<nTemplates; l++)
            {
              if(alignment_((mrs_natural)beginPos_(l),i-1) != 0)
              {
                costMatrix_((mrs_natural)beginPos_(l)+1,(mrs_natural)matrixPos_(2)) = costMatrix_((mrs_natural)beginPos_(l),(mrs_natural)matrixPos_(1)) + in((mrs_natural)beginPos_(l)+1,i);
                if(weight)
                  costMatrix_((mrs_natural)beginPos_(l)+1,(mrs_natural)matrixPos_(2)) += in((mrs_natural)beginPos_(l)+1,i);
                alignment_((mrs_natural)beginPos_(l)+1,i) = 2;
                if(alignment_((mrs_natural)beginPos_(l),i-2) != 0)
                {
                  tmpReal = costMatrix_((mrs_natural)beginPos_(l),(mrs_natural)matrixPos_(0)) + in((mrs_natural)beginPos_(l)+1,i-1) + in((mrs_natural)beginPos_(l)+1,i);
                  if(weight)
                    tmpReal += in((mrs_natural)beginPos_(l)+1,i-1);
                  if(tmpReal < costMatrix_((mrs_natural)beginPos_(l)+1,(mrs_natural)matrixPos_(2)))
                  {
                    costMatrix_((mrs_natural)beginPos_(l)+1,(mrs_natural)matrixPos_(2)) = tmpReal;
                    alignment_((mrs_natural)beginPos_(l)+1,i) = 3;
                  }
                }
              }
              else if(alignment_((mrs_natural)beginPos_(l),i-2) != 0)
              {
                costMatrix_((mrs_natural)beginPos_(l)+1,(mrs_natural)matrixPos_(2)) = costMatrix_((mrs_natural)beginPos_(l),(mrs_natural)matrixPos_(0)) + in((mrs_natural)beginPos_(l)+1,i-1) + in((mrs_natural)beginPos_(l)+1,i);
                if(weight)
                  costMatrix_((mrs_natural)beginPos_(l)+1,(mrs_natural)matrixPos_(2)) += in((mrs_natural)beginPos_(l)+1,i-1);
                alignment_((mrs_natural)beginPos_(l)+1,i) = 3;
              }
              for(j=(mrs_natural)beginPos_(l)+2; j<(mrs_natural)endPos_(l); j++)
              {
                if(alignment_(j-1,i-2) != 0)
                {
                  costMatrix_(j,(mrs_natural)matrixPos_(2)) = costMatrix_(j-1,(mrs_natural)matrixPos_(0)) + in(j,i-1) + in(j,i);
                  if(weight)
                    costMatrix_(j,(mrs_natural)matrixPos_(2)) += in(j,i-1);
                  alignment_(j,i) = 3;
                  if(alignment_(j-1,i-1) != 0)
                  {
                    tmpReal = costMatrix_(j-1,(mrs_natural)matrixPos_(1)) + in(j,i);
                    if(weight)
                      tmpReal += in(j,i);
                    if(tmpReal < costMatrix_(j,(mrs_natural)matrixPos_(2)))
                    {
                      costMatrix_(j,(mrs_natural)matrixPos_(2)) = tmpReal;
                      alignment_(j,i) = 2;
                    }
                  }
                  if(alignment_(j-2,i-1) != 0)
                  {
                    tmpReal = costMatrix_(j-2,(mrs_natural)matrixPos_(1)) + in(j-1,i) + in(j,i);
                    if(weight)
                      tmpReal += in(j-1,i);
                    if(tmpReal < costMatrix_(j,(mrs_natural)matrixPos_(2)))
                    {
                      costMatrix_(j,(mrs_natural)matrixPos_(2)) = tmpReal;
                      alignment_(j,i) = 1;
                    }
                  }
                }
                else if(alignment_(j-1,i-1) != 0)
                {
                  costMatrix_(j,(mrs_natural)matrixPos_(2)) = costMatrix_(j-1,(mrs_natural)matrixPos_(1)) + in(j,i);
                  if(weight)
                    costMatrix_(j,(mrs_natural)matrixPos_(2)) += in(j,i);
                  alignment_(j,i) = 2;
                  if(alignment_(j-2,i-1) != 0)
                  {
                    tmpReal = costMatrix_(j-2,(mrs_natural)matrixPos_(1)) + in(j-1,i) + in(j,i);
                    if(weight)
                      tmpReal += in(j-1,i);
                    if(tmpReal < costMatrix_(j,(mrs_natural)matrixPos_(2)))
                    {
                      costMatrix_(j,(mrs_natural)matrixPos_(2)) = tmpReal;
                      alignment_(j,i) = 1;
                    }

                  }
                }
                else if(alignment_(j-2,i-1) != 0)
                {
                  costMatrix_(j,(mrs_natural)matrixPos_(2)) = costMatrix_(j-2,(mrs_natural)matrixPos_(1)) + in(j-1,i) + in(j,i);
                  if(weight)
                    costMatrix_(j,(mrs_natural)matrixPos_(2)) += in(j-1,i);
                  alignment_(j,i) = 1;
                }
              }
            }
            for(j=0; j<3; j++)
            {
              matrixPos_(j)++;
              if(matrixPos_(j) >= 3)
                matrixPos_(j) = 0;
            }
          }

          // backtrace
          for(i=0; i<out.getRows(); ++i)
          {
            for(j=0; j<out.getCols(); j++)
            {
              out(i,j) = -1;
            }
          }
          if(ctrl_lastPos_->to<mrs_string>() == "end")
          {
            tmpReal = costMatrix_((mrs_natural)endPos_(0)-1,(mrs_natural)matrixPos_(1));
            j = (mrs_natural)endPos_(0)-1;
            for(l=1; l<nTemplates; l++)
            {
              if(costMatrix_((mrs_natural)endPos_(l)-1,(mrs_natural)matrixPos_(1)) < tmpReal)
              {
                tmpReal = costMatrix_((mrs_natural)endPos_(l)-1,(mrs_natural)matrixPos_(1));
                j = (mrs_natural)endPos_(l)-1;
              }
            }
            totalDis_ = tmpReal;
            ctrl_totalDis_->setValue(totalDis_);
            i = (mrs_natural)nSmp-1;
          }
          else if(ctrl_lastPos_->to<mrs_string>() == "lowest")
          {
            tmpReal = costMatrix_(0,(mrs_natural)matrixPos_(1));
            j=0;
            for(i=1; i<nObs; ++i)
            {
              if(costMatrix_(i,(mrs_natural)matrixPos_(1)) < tmpReal)
              {
                tmpReal = costMatrix_(i,(mrs_natural)matrixPos_(1));
                j = i;
              }
            }
            i = (mrs_natural)nSmp-1;
            totalDis_ = tmpReal;
            ctrl_totalDis_->setValue(totalDis_);
          }
          k = (mrs_natural)(3*nSmp -1);// + nObs - 1;
          while(alignment_(j,i) != 0 && k>=0)
          {
            if(alignment_(j,i) == 1)
            {
              out(k,0) = i;
              out(k,1) = j;
              j--;
              k--;
              out(k,0) = i;
              out(k,1) = j;
              k--;
              if(weight)
              {
                out(k,0) = i;
                out(k,1) = j;
                k--;
              }
              j--;
              i--;
            }
            else if(alignment_(j,i) == 2)
            {
              out(k,0) = i;
              out(k,1) = j;
              k--;
              if(weight)
              {
                out(k,0) = i;
                out(k,1) = j;
                k--;
              }
              i--;
              j--;
            }
            else if(alignment_(j,i) == 3)
            {
              out(k,0) = i;
              out(k,1) = j;
              k--;
              i--;
              out(k,0) = i;
              out(k,1) = j;
              k--;
              if(weight)
              {
                out(k,0) = i;
                out(k,1) = j;
                k--;
              }
              i--;
              j--;
            }
            else if(alignment_(j,i) < 0)
            {
              out(k,0) = i;
              out(k,1) = j;
              k--;
              if(weight)
              {
                out(k,0) = i;
                out(k,1) = j;
                k--;
              }
              j = (mrs_natural)(-1*alignment_(j,i));
              i--;
            }
          }
          out(k,0) = i;
          out(k,1) = j;
        }
      }
      else
      {
        MRSWARN("DTW::myProcess - invalid sizes vector (does not output a real value)!");
      }
    }
  }
}





