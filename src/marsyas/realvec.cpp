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

#include <marsyas/realvec.h>
#include <marsyas/common_source.h>
#include <marsyas/NumericLib.h>
#include "Communicator.h"

#include <algorithm>
#include <limits>

#ifdef MARSYAS_MATLAB
//#define _MATLAB_REALVEC_
#endif

using namespace std;

namespace Marsyas
{



/// constructor
realvec::realvec()
  : size_(0),
    allocatedSize_(0),
    data_(NULL),
    rows_(1),
    cols_(0)
{
  allocateData(size_);
}

realvec::~realvec()
{
  delete[] data_;
  data_ = NULL;
}

realvec::realvec(mrs_natural size)
  : size_(size),
    allocatedSize_(0),
    data_(NULL),
    rows_(1),
    cols_(size)
{
  allocateData(size_);
}

realvec::realvec(mrs_natural rows, mrs_natural cols, mrs_real value):
  size_(rows * cols),
  allocatedSize_(rows * cols),
  data_(0),
  rows_(rows),
  cols_(cols)
{
  if (size_ > 0)
  {
    data_ = new mrs_real[size_];
    for (mrs_natural i=0; i<size_; ++i)
      data_[i] = value;
  }
}

realvec::realvec(const realvec& a)
  : size_(a.size_),
    allocatedSize_(0),
    data_(NULL),
    rows_(a.rows_),
    cols_(a.cols_)
{
  allocateData(size_);

  for (mrs_natural i=0; i<size_; ++i)
    data_[i] = a.data_[i];

}

realvec&
realvec::operator=(const realvec& a)
{
  if (this != &a)
  {
    size_ = a.size_;
    rows_ = a.rows_;
    cols_ = a.cols_;

    //check if we need to allocate more memory
    if (allocatedSize_ < size_)
      allocateData(size_);

    //copy data
    // for (mrs_natural i=0; i < size_; ++i)
    // 'data_[i] = a.data_[i];

    memcpy (data_, a.data_, sizeof(mrs_real)*size_);

  }

  return *this;
}

mrs_real *
realvec::getData() const
{
  return data_;
}

void
realvec::appendRealvec(const realvec newValues)
{
  mrs_natural origSize = size_;

  stretch(origSize + newValues.getSize());

  for (mrs_natural i=0; i<newValues.getSize(); ++i)
    data_[origSize + i] = newValues.data_[i];
}

realvec
realvec::getSubVector(mrs_natural startPos, mrs_natural length) const
{
  realvec subVector(length);
  for (mrs_natural i=0; i<length; ++i)
    subVector.data_[i] = data_[startPos + i];
  return subVector;
}

void
realvec::transpose()
{
  mrs_real *tmp_ = new mrs_real[size_];

  for (mrs_natural i=0; i < rows_; ++i)
    for (mrs_natural j=0; j < cols_; j++)
      tmp_[i * cols_ + j] = data_[j * rows_ + i];

  mrs_natural tmp = rows_;
  rows_ = cols_;
  cols_ = tmp;

  delete [] data_;
  data_ = tmp_;
}

void
realvec::fliplr()
{
  mrs_natural i,j,jtmp;

  for (i=0; i < rows_; i++)
    for (j=0, jtmp = cols_-1; j < cols_/2; j++, jtmp--)
    {
      mrs_real tmp	= (*this)(i,j);
      (*this)(i,j)	= (*this)(i,jtmp);
      (*this)(i,jtmp)	= tmp;
    }
}

void
realvec::flipud()
{
  mrs_natural i,j,itmp;

  for (i=0, itmp = rows_-1; i < rows_/2; i++, itmp--)
    for (j=0; j < cols_; j++)
    {
      mrs_real tmp	= (*this)(i,j);
      (*this)(i,j)	= (*this)(itmp,j);
      (*this)(itmp,j)	= tmp;
    }
}

mrs_real
realvec::median() const
{
  if (size_ == 0)
    return 0;
  realvec tmp(*this);
  mrs_real *tmpData = tmp.data_;
  std::sort(tmpData, tmpData+size_);
  return tmpData[size_/2];
}

mrs_real
realvec::mean() const
{
  mrs_real sum = 0.0;
  for (mrs_natural i=0; i < size_; ++i)
  {
    sum += data_[i];
  }
  if (sum != 0.0) sum /= size_;
  return sum;
}

void
realvec::sort() //non-descending sort - assumes one dimensional vector only!
{
  std::sort(data_, data_+size_);
}

mrs_real
realvec::sum() const
{
  mrs_real sum = 0.0;
  for (mrs_natural i=0; i < size_; ++i)
  {
    sum += data_[i];
  }
  return sum;
}


mrs_real
realvec::var() const
{
  mrs_real sum = 0.0;
  mrs_real sum_sq = 0.0;
  mrs_real val;
  mrs_real var;

  for (mrs_natural i=0; i < size_; ++i)
  {
    val = data_[i];
    sum += val;
    sum_sq += (val * val);
  }
  if (sum != 0.0) sum /= size_;
  if (sum_sq != 0.0) sum_sq /= size_;

  var = sum_sq - sum * sum;

  if (var < 0.0)
    var = 0.0;
  return var;
}

mrs_real
realvec::std() const
{
  mrs_real vr = var();
  if (vr != 0)
    return sqrt(vr);
  else
    return 0.0;
}

mrs_natural
realvec::getRows() const
{
  return rows_;
}

mrs_natural
realvec::getCols() const
{
  return cols_;
}

mrs_natural
realvec::getSize() const
{
  return size_;
}

void
realvec::debug_info()
{
  MRSERR("realvec information");
  MRSERR("size = " << size_);
}

/* keep the old data and possibly extend */
void
realvec::stretch(mrs_natural size)
{
  if (size_ == size)
    return; //no need for more memory allocation

  if (size < allocatedSize_)
  {
    size_ = size;
    rows_ = 1;
    cols_ = size_;
    return; //no need for more memory allocation
  }

  // we need more memory allocation!
  // size should be always >= 1 at this point
  mrs_real* ndata = new mrs_real[size];

  mrs_natural i=0;
  for (; i < size_; ++i)
    ndata[i] = data_[i]; //copy existing data
  for(; i < size; ++i)
    ndata[i] = 0.0; //zero new data

  //deallocate existing memory...
  delete [] data_;
  //...and point to new data memory (if any - it can be NULL, when size == 0)
  data_ = ndata;

  //update internal parameters
  size_ = size;
  allocatedSize_ = size;
  rows_ = 1;
  cols_ = size_;
}

/* keep the old data and possibly extend */
void
realvec::stretch(mrs_natural rows, mrs_natural cols)
{
  mrs_natural size = rows * cols;

  if ((rows == rows_)&&(cols == cols_))
    return;

  if(size == 0) {
    size_ = size;
    rows_ = rows;
    cols_ = cols;
    return;
  }

	/*lgmartins: this messes up the internal data of the realvec!!
	//FIXME: for this optimization to work on 2D realvecs, data has to be
	//reorganized inside the realvec...
  if (size < allocatedSize_)
  {
    size_ = size;
    rows_ = rows;
    cols_ = cols;
    return; //no need for more memory allocation
  }
	 */

  mrs_real *ndata = new mrs_real[size];

  // copy and zero new data
  for (mrs_natural r=0; r < rows; ++r)
    for (mrs_natural c = 0; c < cols; ++c)
    {
      if ((r < rows_)&&(c < cols_))
        ndata[c * rows + r] = data_[c * rows_ + r];
      else
        ndata[c * rows + r] = 0.0;
    }

  delete [] data_;
  data_ = ndata;

  size_ = size;
  allocatedSize_ = size;
  rows_ = rows;
  cols_ = cols;
}

void
realvec::stretchWrite(const mrs_natural pos, const mrs_real val)
{
  // don't forget the zero-indexing position!
  //   i.e.  pos=0  is the first value to store
  mrs_natural wantSize = pos+1;
  if (wantSize > size_) {
    if ( wantSize < 2*size_ )
      // grow exponentially with sequential access
      stretch( 2*size_ );
    else
      // if we have a sudden large value, don't double it
      stretch( wantSize );
  }
  // FIXME: add a MRSASSERT here for debugging.
  // cout<<"List stretched to "<<size_<<endl;
  data_[pos] = val;
}

void
realvec::stretchWrite(const mrs_natural r, const mrs_natural c, const mrs_real val)
{
  mrs_natural nextR = rows_;
  mrs_natural nextC = cols_;
  mrs_natural wantR = r+1;
  mrs_natural wantC = c+1;
  if ( (wantR >= rows_) || (wantC >= cols_) )
  {
    if ( wantR >= rows_ ) {
      if ( wantR < 2*rows_ ) {
        nextR = 2*rows_;
      } else {
        nextR = wantR;
      }
    }

    if ( wantC >= cols_ ) {
      if ( wantC < 2*cols_ ) {
        nextC = 2*cols_;
      } else {
        nextC = wantC;
      }
    }

    stretch( nextR, nextC );
    // FIXME: add a MRSASSERT here for debugging.
    // cout<<"List stretched to "<<rows_<<", "<<cols_<<endl;
  }
  data_[c * rows_ + r] = val;
}

void
realvec::create(mrs_natural size)
{
  size_ = size;
  allocateData(size_);
  rows_ = 1;
  cols_ = size_;
}

void
realvec::create(mrs_natural rows, mrs_natural cols)
{
  size_ = rows * cols;
  rows_ = rows;
  cols_ = cols;
  allocateData(size_);
}


void
realvec::create(mrs_real val, mrs_natural rows, mrs_natural cols)
{
  size_ = rows * cols;
  rows_ = rows;
  cols_ = cols;
  delete [] data_;
  data_ = NULL;
  if (size_ > 0)
    data_ = new mrs_real[size_];
  for (mrs_natural i=0; i<size_; ++i)
    data_[i] = val;
  allocatedSize_ = size_;
}



void
realvec::allocate(mrs_natural size)
{
  delete [] data_;
  data_ = NULL;
  size_ = size;
  cols_ = size_;
  rows_ = 1;
  allocatedSize_ = size;
  if (size_ > 0)
    data_ = new mrs_real[size_];
}

void
realvec::allocate(mrs_natural rows, mrs_natural cols)
{
  delete [] data_;
  data_ = NULL;
  size_ = rows*cols;
  cols_ = cols;
  rows_ = rows;
  allocatedSize_ = size_;
  if (size_ > 0)
    data_ = new mrs_real[size_];
}


void
realvec::setval(mrs_natural start, mrs_natural end, mrs_real val)
{
  MRSASSERT(start >= (mrs_natural)0);
  MRSASSERT(start < (mrs_natural)size_);
  MRSASSERT(end < (mrs_natural)size_);

  for (mrs_natural i=start; i<end; ++i)
  {
    data_[i] = val;
  }
}

void
realvec::apply(mrs_real (*func) (mrs_real))
{
  for (mrs_natural i=0; i<size_; ++i)
  {
    data_[i] = func(data_[i]);
  }
}

void
realvec::setval(mrs_real val)
{
  for (mrs_natural i=0; i<size_; ++i)
  {
    data_[i] = val;
  }
}

void
realvec::abs()
{
  for (mrs_natural i=0; i<size_; ++i)
  {
    data_[i] = fabs(data_[i]);
  }
}

void
realvec::pow (mrs_real exp)
{
  for (mrs_natural i=0; i<size_; i++)
  {
    data_[i] = ::pow (data_[i], exp);
  }
}

void
realvec::norm()
{
  mrs_real mean = this->mean();
  mrs_real std = this->std();
  for (mrs_natural i=0; i < size_; ++i)
  {
    data_[i] = (data_[i] - mean) / std;
  }
}


void
realvec::normMaxMin()
{
  mrs_real max = DBL_MIN;
  mrs_real min = DBL_MAX;

  for (mrs_natural i=0; i < size_; ++i)
  {
    if (data_[i] > max)
      max = data_[i];
    if (data_[i] < min)
      min = data_[i];
  }


  for (mrs_natural i=0; i < size_; ++i)
  {
    data_[i] = (data_[i] - min) / (max - min);
  }


}


void
realvec::norm(mrs_real mean, mrs_real std)
{
  for (mrs_natural i=0; i < size_; ++i)
  {
    data_[i] = (data_[i] - mean) / std;
  }
}

void
realvec::renorm(mrs_real old_mean, mrs_real old_std, mrs_real new_mean, mrs_real new_std)
{
  for (mrs_natural i=0; i < size_; ++i)
  {
    data_[i] = (data_[i] - old_mean) / old_std;
    data_[i] *= new_std;
    data_[i] += new_mean;
  }
}

mrs_natural
realvec::invert(realvec& res)
{
  if (rows_ != cols_)
  {
    MRSERR("realvec::invert() - matrix should be square!");
    res.create(0);
    return -1;
  }
  if (this != &res)
  {
    mrs_natural rank;
    mrs_natural r,c,i;
    mrs_real temp;

    res.create(rows_, cols_);

    rank = 0;
    for (r = 0; r < rows_; ++r)
      for (c=0; c < cols_; ++c)
      {
        if (r == c)
          res(r,c) = 1.0;
        else
          res(r,c) = 0.0;
      }
    for (i = 0; i < rows_; ++i)
    {
      if ((*this)(i,i) == 0)
      {
        for (r = i; r < rows_; ++r)
          for (c = 0; c < cols_; ++c)
          {
            (*this)(i,c) += (*this)(r,c);
            res(i,c) += res(r,c);
          }
      }
      for (r = i; r < rows_; ++r)
      {
        temp = (*this)(r,i);
        if (temp != 0)
          for (c =0; c < cols_; ++c)
          {
            (*this)(r,c) /= temp;
            res(r,c) /= temp;
          }
      }
      if (i != rows_-1)
      {
        for (r = i+1; r < rows_; ++r)
        {
          temp = (*this)(r,i);
          if (temp != 0.0)
            for (c=0; c < cols_; ++c)
            {
              (*this)(r,c) -= (*this)(i,c);
              res(r,c) -= res(i,c);
            }
        }
      }
    }
    for (i=1; i < rows_; ++i)
      for (r=0; r < i; ++r)
      {
        temp = (*this)(r,i);
        for (c=0; c < cols_; ++c)
        {
          (*this)(r,c) -= (temp * (*this)(i,c));
          res(r,c) -= (temp * res(i,c));
        }
      }
    for (r =0; r < rows_; ++r)
      for (c=0; c < cols_; ++c)
        (*this)(r,c) = res(r,c);
    return rank;
  }
  else
  {
    res.create(0);
    MRSERR("realvec::invert() - inPlace operation not supported - returning empty result vector!");
    return -1;
  }
}

void
realvec::sqr()
{
  for (mrs_natural i=0; i<size_; ++i)
  {
    data_[i] *= data_[i];
  }
}

mrs_natural
realvec::search(mrs_real val)
{
  mrs_real minDiff = MAXREAL;
  mrs_natural index=-1;
  for (mrs_natural i=0; i<size_; ++i)
    if (fabs(data_[i]-val)< minDiff)
    {
      minDiff = fabs(data_[i]-val);
      index=i;
    }
  return index;
}

void
realvec::sqroot()
{
  for (mrs_natural i=0; i<size_; ++i)
  {
    data_[i] = sqrt(data_[i]);
  }
}

void
realvec::send(Communicator *com)
{
  static char *buf = new char[256];
  mrs_string message;
  sprintf(buf, "%i\n", (int)size_);
  message = buf;
  com->send_message(message);
  for (mrs_natural i=0; i<size_; ++i)
  {
    sprintf(buf, "%f\n", data_[i]);
    message = buf;
    com->send_message(message);
  }
  MRSERR("realvec::send numbers were sent to the client");

}

bool
realvec::read(mrs_string filename)
{
  ifstream from(filename.c_str());
  if (from.is_open())
  {
    from >> (*this);
    return true;
  }
  else
  {
    MRSERR("realvec::read: failed to open file: " << filename);
    return false;
  }
}


void
realvec::shuffle()
{
  // Use a Fisher-Yates shuffle : http://en.wikipedia.org/wiki/Fisher-Yates_shuffle
  unsigned int n = cols_;
  while (n > 1)
  {
    // Generate a random index in the range [0, n).
    unsigned int k = (unsigned int)((mrs_real)n * (mrs_real)rand() / (mrs_real)(RAND_MAX + 1.0));

    n--;

    // Swap column n and column k.
    if (k != n)
    {
      for (int r = 0; r < rows_; ++r)
        swap(data_[k * rows_ + r], data_[n * rows_ + r]);
    }
  }
}


bool
realvec::write(mrs_string filename) const
{
  ofstream os(filename.c_str());
  if (os.is_open())
  {
    os << (*this) << endl;
    return true;
  }
  else
  {
    MRSERR("realvec::write: failed to open file to write: filename");
    return false;
  }
}

void
realvec::dump()
{
  for (mrs_natural i =0 ; i< size_ ; ++i)
    MRSMSG(data_[i] << " ") ;
  MRSMSG(endl);
}

bool
realvec::readText(mrs_string filename)
{
  ifstream infile(filename.c_str());
  if (infile.is_open())
  {
    int i = 0;
    if (size_ == 0)
      create(1);

    mrs_real value;
    while (infile >> value)
    {
      // slower but safer
      stretchWrite(i,value);
      ++i;
    }
    stretch(i-1);
    infile.close();
    return true;
  }
  else
  {
    MRSERR("realvec::readText: failed to open file: " << filename);
    return false;
  }
}

bool
realvec::writeText(mrs_string filename)
{
  if (size_ == 0)
    return true; //[?]

  ofstream outfile(filename.c_str());
  if (outfile.is_open())
  {
    for (mrs_natural i=0; i<size_; ++i)
    {
      outfile << data_[i] <<endl;
    }
    outfile.close();
    return true;
  }
  else
  {
    MRSERR("realvec::writeText: failed to open file: " << filename);
    return false;
  }
}

/**
 * Dump the realvec data (only the data, no header info ) to an outputstream.
 * Use the given column and row separators between columns and rows respectively.
 */
void
realvec::dumpDataOnly(std::ostream& o, std::string columnSep, std::string rowSep) const
{
  for (mrs_natural r = 0; r < this->rows_; ++r)
  {
    for (mrs_natural c = 0; c < this->cols_; ++c)
    {
      o << this->data_[c * this->rows_ + r];
      if (c < this->cols_ - 1)
      {
        o << columnSep ;
      }
    }
    if (r < this->rows_ - 1)
    {
      o << rowSep;
    }
  }
}

ostream&
operator<< (ostream& o, const realvec& vec)
{
  o << "# MARSYAS mrs_realvec" << endl;
  o << "# Size = " << vec.size_ << endl << endl;
  o << endl;

  o << "# type: matrix" << endl;
  o << "# rows: " << vec.rows_ << endl;
  o << "# columns: " << vec.cols_ << endl;

  vec.dumpDataOnly(o, " ", "\n");
  o << endl;

  // for (mrs_natural i=0; i<vec.size_; ++i)
  // o << vec.data_[i] << endl;
  o << endl;
  o << "# Size = " << vec.size_ << endl;
  o << "# MARSYAS mrs_realvec" << endl;
  return o;
}

istream&
operator>>(istream& is, realvec& vec)
{
  // [WTF] ... is this necessary?  doesn't allocate() delete the data array, and reallocate? (Jen)
  //  if (vec.size_ != 0)
  //    {
  //      MRSERR("realvec::operator>>: realvec already allocated cannot read from istream");
  //      MRSERR("vec.size_ = " << vec.size_);
  //
  //      return is;
  //    }
  mrs_string str0,str1,str2;
  mrs_natural size;
  mrs_natural i;

  is >> str0;
  is >> str1;
  is >> str2;


  if ((str0 != "#") || (str1 != "MARSYAS") || (str2 != "mrs_realvec"))
  {
    MRSERR("realvec::operator>>: Problem1 reading realvec object from istream");
    MRSERR("-str0 = " << str0 << endl);
    MRSERR("-str1 = " << str1 << endl);
    MRSERR("-str2 = " << str2 << endl);
    return is;
  }
  is >> str0;
  is >> str1;
  is >> str2;


  if ((str0 != "#") || (str1 != "Size") || (str2 != "="))
  {
    MRSERR("realvec::operator>>: Problem2 reading realvec object from istream");
    MRSERR("-str0 = " << str0 << endl);
    MRSERR("-str1 = " << str1 << endl);
    MRSERR("-str2 = " << str2 << endl);
    return is;
  }
  is >> size;

  vec.create(size);
  for (i=0; i<3; ++i)
  {
    is >> str0;
  }
  is >> str0 >> str1 >> vec.rows_;
  is >> str0 >> str1 >> vec.cols_;

  for (mrs_natural r = 0; r < vec.rows_; ++r)
    for (mrs_natural c = 0; c < vec.cols_; ++c)
    {
      is >> str0;
      if (str0 == "nan") {
        vec.data_[c*vec.rows_ + r] = std::numeric_limits<double>::quiet_NaN();
      } else {
        std::stringstream s(str0);
        s >> vec.data_[c * vec.rows_ + r];
      }
    }

  is >> str0;
  is >> str1;
  is >> str2;
  if ((str0 != "#") || (str1 != "Size") || (str2 != "="))
  {
    MRSERR("realvec::operator>>: Problem3 reading realvec object from istream");
    MRSERR("-str0 = " << str0 << endl);
    MRSERR("-str1 = " << str1 << endl);
    MRSERR("-str2 = " << str2 << endl);
    is >> str0;
    is >> str1;
    is >> str2;
    MRSERR("-str0 = " << str0 << endl);
    MRSERR("-str1 = " << str1 << endl);
    MRSERR("-str2 = " << str2 << endl);
    return is;
  }
  is >> size;
  is >> str0;
  is >> str1;
  is >> str2;

  if ((str0 != "#") || (str1 != "MARSYAS") || (str2 != "mrs_realvec"))
  {
    MRSERR("realvec::operator>>: Problem4 reading realvec object from istream");
    MRSERR("-str0 = " << str0 << endl);
    MRSERR("-str1 = " << str1 << endl);
    MRSERR("-str2 = " << str2 << endl);
    return is;
  }
  return is;
}

realvec
realvec::operator()(std::string r, std::string c)
{
  mrs_string::size_type r_l = r.length();
  mrs_string::size_type c_l = c.length();

  mrs_string::size_type r_c = r.find(":");
  mrs_string::size_type c_c = c.find(":");

  mrs_string::size_type r_a;
  mrs_string::size_type r_b;

  mrs_string::size_type c_a;
  mrs_string::size_type c_b;

  char *endptr;

  MRSASSERT( (r_c == 0 && r_l == 1) || (r_c == mrs_string::npos) || (r_c>0 && r_l-r_c>1) );
  MRSASSERT( (c_c == 0 && c_l == 1) || (c_c == mrs_string::npos) || (c_c>0 && c_l-c_c>1) );

  if ( r_c != mrs_string::npos && r_l > 1 )
  {
    r_a = strtol( r.substr(0,r_c).c_str() , &endptr , 10  );
    MRSASSERT( *endptr == '\0' );
    r_b = strtol( r.substr(r_c+1,r_l-r_c-1).c_str() , &endptr , 10  );
    MRSASSERT( *endptr == '\0' );
  }
  else if ( r_c == mrs_string::npos )
  {
    r_a = r_b = strtol( r.c_str() , &endptr , 10 );
    MRSASSERT( *endptr == '\0' );
  }
  else
  {
    r_a = 0;
    r_b = rows_-1;
  }

  MRSASSERT( (mrs_natural)r_b < rows_ );

  if ( c_c != mrs_string::npos && c_l > 1 )
  {
    c_a = strtol( c.substr(0,c_c).c_str() , &endptr , 10  );
    MRSASSERT( *endptr == '\0' );
    c_b = strtol( c.substr(c_c+1,c_l-c_c-1).c_str() , &endptr , 10 );
    MRSASSERT( *endptr == '\0' );
  }
  else if ( c_c == mrs_string::npos )
  {
    c_a = c_b = strtol( c.c_str() , &endptr , 10 );
    MRSASSERT( *endptr == '\0' );
  }
  else
  {
    c_a = 0;
    c_b = cols_-1;
  }

  MRSASSERT( (mrs_natural)c_b < cols_ );

  r_l = r_b - r_a + 1;
  c_l = c_b - c_a + 1;

  realvec matrix;

  matrix.create( (mrs_natural) r_l , (mrs_natural) c_l );

  for ( c_c = c_a ; c_c <= c_b ; c_c++ )
  {
    for ( r_c = r_a ; r_c <= r_b ; r_c++ )
    {
      matrix.data_[(c_c-c_a) * r_l + (r_c-r_a)] = data_[c_c * rows_ + r_c];
    }
  }

  return matrix;
}

realvec
realvec::operator()(std::string c)
{
  mrs_string::size_type c_l = c.length();
  mrs_string::size_type c_c = c.find(":");
  mrs_string::size_type c_a;
  mrs_string::size_type c_b;
  char *endptr;

  MRSASSERT( (c_c == 0 && c_l == 1) || (c_c == mrs_string::npos) || (c_c>0 && c_l-c_c>1) );

  if ( c_c != mrs_string::npos && c_l > 1 )
  {
    c_a = strtol( c.substr(0,c_c).c_str() , &endptr , 10  );
    MRSASSERT( *endptr == '\0' );
    c_b = strtol( c.substr(c_c+1,c_l-c_c-1).c_str() , &endptr , 10  );
    MRSASSERT( *endptr == '\0' );
  }
  else if ( c_c == mrs_string::npos )
  {
    c_a = c_b = strtol( c.c_str() , &endptr , 10 );
    MRSASSERT( *endptr == '\0' );
  }
  else
  {
    c_a = 0;
    c_b = (rows_*cols_)-1;
  }

  MRSASSERT( (mrs_natural)c_b < rows_*cols_ );
  c_l = c_b - c_a + 1;

  realvec matrix;
  matrix.create( (mrs_natural) c_l );

  for ( c_c = c_a ; c_c <= c_b ; c_c++ )
  {
    matrix.data_[(c_c-c_a)] = data_[c_c];
  }
  return matrix;
}

void
realvec::getRow(const mrs_natural r, realvec& res) const
{
  if (this != &res)
  {
    if (r >= rows_ )
    {
      MRSERR("realvec::getRow() - row index greater than realvec number of rows! Returning empty result vector.");
      res.create(0);
      return;
    }
    res.stretch(cols_);
    for (mrs_natural c=0; c < cols_; ++c)
    {
      res(c) = (*this)(r,c);
    }
  }
  else
  {
    res.create(0);
    MRSERR("realvec::getRow() - inPlace operation not supported - returning empty result vector!");
  }
}

void
realvec::getCol(const mrs_natural c, realvec& res) const
{
  if (this != &res)
  {
    if (c >= cols_)
    {
      MRSERR("realvec::getCol() - row index greater than realvec number of rows! Returning empty result vector.");
      res.create(0);
      return;
    }
    res.stretch(rows_,1);
    for (mrs_natural r=0; r < rows_; ++r)
    {
      res(r) = (*this)(r,c);
    }
  }
  else
  {
    res.create(0);
    MRSERR("realvec::getCol() - inPlace operation not supported - returning empty result vector!");
  }
}
void
realvec::getSubMatrix (const mrs_natural r, const mrs_natural c, realvec& res)
{
  if (this != &res)
  {
    mrs_natural	numRows	= res.getRows (),
                numCols	= res.getCols ();
    //if (c + numCols > cols_ || r + numRows > rows_) this might also be a reasonable check...
    if (c >= cols_ || r >= rows_)
    {
      MRSERR("realvec::getSubMatrix() - index larger than realvec number of rows/cols! Returning empty result vector.");
      res.create(0);
      return;
    }
    mrs_natural	m,n,i,j;
    mrs_natural	endRow	= min(rows_, r + numRows),
                 endCol	= min(cols_, c + numCols);
    for (m=r, i=0; m < endRow; m++,++i)
    {
      for (n=c, j=0; n < endCol; n++, j++)
        res(i,j) = (*this)(m,n);
    }

    // if there are remaining elements, fill up with zeros (or should we throw something? see MRSERR check above)
    for (i=endRow-r; i < numRows; ++i)
      for (j=0; j < numCols; j++)
        res(i,j)	= 0;
    for (j=endCol-c; j < numCols; j++)
      for (i=0; i < numRows; ++i)
        res(i,j)	= 0;
  }
  else
  {
    res.create(0);
    MRSERR("realvec::getSubMatrix() - inPlace operation not supported - returning empty result vector!");
  }
}

void
realvec::setRow (const mrs_natural r, const realvec src)
{
  setSubMatrix (r,0, src);
}
void
realvec::setCol (const mrs_natural c, const realvec src)
{
  setSubMatrix (0,c,src);
}
void
realvec::setSubMatrix (const mrs_natural r, const mrs_natural c, const realvec src)
{
  mrs_natural	m,n;
  mrs_natural	numRows	= src.getRows (),
              numCols	= src.getCols ();
  if (c+numCols > cols_ || r+numRows > rows_)
  {
    MRSERR("realvec::setSubMatrix() - dimension mismatch! Abort.");
    return;
  }

  mrs_natural	endRow	= min(rows_, r + numRows),
               endCol	= min(cols_, c + numCols);
  for (m=r; m < endRow; m++)
  {
    for (n=c; n < endCol; n++)
      (*this)(m,n)	= src(m-r,n-c);
  }
}


mrs_real
realvec::maxval(mrs_natural* index) const
{
  mrs_real max = numeric_limits<mrs_real>::max() * -1.0;
  mrs_natural ind = 0;
  for (mrs_natural i=0; i < size_; ++i)
  {
    if (data_[i] > max)
    {
      max = data_[i];
      ind = i;
    }
  }
  if (index)
    *index = ind;
  return max;
}

mrs_real
realvec::minval() const
{
  mrs_real min = numeric_limits<mrs_real>::max();
  for (mrs_natural i=0; i < size_; ++i)
  {
    if (data_[i] < min)
      min = data_[i];
  }
  return min;
}

void
realvec::meanObs(realvec& res) const
{
  if (this != &res)
  {
    realvec obsrow(cols_); //row vector //[TODO]
    res.stretch(rows_, 1); //column vector

    for (mrs_natural r=0; r<rows_; ++r)
    {
      //obsrow = getRow(r);
      for (mrs_natural c=0; c<cols_; ++c)
      {
        obsrow(c) = (*this)(r,c); //get observation row
      }
      res(r,0) = obsrow.mean();
    }
  }
  else
  {
    res.create(0);
    MRSERR("realvec::meanObs() - inPlace operation not supported - returning empty result vector!");
  }
}

void
realvec::varObs(realvec& res) const
{
  if (this != &res)
  {
    res.create(rows_, 1); //column vector
    realvec obsrow(cols_); //row vector //[TODO]

    for (mrs_natural r=0; r<rows_; ++r)
    {
      //obsrow = getRow(r);
      for (mrs_natural c=0; c<cols_; ++c)
      {
        obsrow(c) = (*this)(r,c); //get observation row
      }
      res(r,0) = obsrow.var();
    }
  }
  else
  {
    res.create(0);
    MRSERR("realvec::varObs() - inPlace operation not supported - returning empty result vector!");
  }
}

void
realvec::stdObs(realvec& res) const
{
  if (this != &res)
  {
    realvec obsrow(cols_); //row vector //[TODO]
    res.stretch(rows_, 1); //column vector
    for (mrs_natural r=0; r<rows_; ++r)
    {
      //obsrow = getRow(r);
      for (mrs_natural c=0; c < cols_; ++c)
      {
        obsrow(c) = (*this)(r,c); //get observation row
      }
      res(r,0) = obsrow.std();
    }
  }
  else
  {
    res.create(0);
    MRSERR("realvec::stdObs() - inPlace operation not supported - returning empty result vector!");
  }
}

void
realvec::normObs()
{
  //normalize (aka standardize) matrix
  //using observations means and standard deviations
  realvec obsrow(cols_); //[TODO]
  mrs_real mean, std;

  for (mrs_natural r=0; r < rows_; ++r)
  {
    for (mrs_natural c=0; c < cols_; ++c)
    {
      obsrow(c) = (*this)(r,c); //get observation row
    }
    //obsrow = getRow(r);
    mean = obsrow.mean();
    std = obsrow.std();
    for (mrs_natural c=0; c < cols_; ++c)
    {
      (*this)(r,c) -= mean;
      (*this)(r,c) /= std;
    }
  }
}

void
realvec::normObsMinMax()
{
  //normalize (aka standardize) matrix
  //using observations min and max values

  realvec obsrow(cols_);
  mrs_real min, max, dif;

  for (mrs_natural r=0; r < rows_; ++r)
  {
    //for (mrs_natural c=0; c < cols_; ++c)
    //{
    //	obsrow(c) = (*this)(r,c); //get observation row
    //}
    getRow(r, obsrow); //[TODO]
    min = obsrow.minval();
    max = obsrow.maxval();
    dif = max-min;
    if (dif ==0)
      dif=1.0;
    for (mrs_natural c=0; c < cols_; ++c)
    {
      (*this)(r,c) -= min;
      (*this)(r,c) /= dif;
    }
  }
}

void
realvec::normSpl(mrs_natural index)
{
  //normalize (aka standardize) matrix
  //using ???		[?] [TODO]
  mrs_real mean;
  mrs_real std;

  realvec colVec;//[TODO]

  if (!index)
    index=cols_;
  for (mrs_natural r=0; r < index; ++r)
  {
    //for (mrs_natural c=0; c < cols_; ++c)
    //{
    //	obsrow(c) = (*this)(r,c); //get observation row
    //}

    getCol(r, colVec);//[TODO]
    mean = colVec.mean();
    std = colVec.std();

    if (std)
      for (mrs_natural c=0; c < rows_; ++c)
      {
        (*this)(c, r) -= mean;
        (*this)(c, r) /= std;
      }
  }
}

void
realvec::normSplMinMax(mrs_natural index)
{
  //normalize (aka standardize) matrix
  //using ???		[?] [TODO]
  mrs_real min;
  mrs_real max, dif;
  realvec colVec; //[TODO]

  if (!index)
    index=cols_;
  for (mrs_natural r=0; r < index; ++r)
  {
    //for (mrs_natural c=0; c < cols_; ++c)
    //{
    //	obsrow(c) = (*this)(r,c); //get observation row
    //}
    getCol(r, colVec);
    min = colVec.minval(); //[TODO]
    max = colVec.maxval(); //[TODO]
    dif = max-min;
    if (dif ==0)
      dif=1.0;
    if (max)
      for (mrs_natural c=0; c < rows_; ++c)
      {
        (*this)(c, r) -= min;
        (*this)(c, r) /= dif;
      }
  }
}

void
realvec::correlation(realvec& res) const
{
  //correlation as computed in Marsyas0.1
  //computes the correlation between observations
  //Assumes data points (i.e. examples) in columns and features (i.e. observations) in rows (as usual in Marsyas0.2).
  if (size_ == 0)
  {
    MRSERR("realvec::correlation() : empty input matrix! returning empty correlation matrix!");
    res.create(0);
    return;
  }
  if (this != &res)
  {
    res.stretch(rows_, rows_);//correlation matrix
    // normalize observations (i.e subtract obs. mean, divide by obs. standard dev)
    realvec temp = (*this); //[TODO]
    temp.normObs();

    mrs_real sum = 0.0;
    for (mrs_natural r1=0; r1< rows_; ++r1)
    {
      for (mrs_natural r2=0; r2 < rows_; ++r2)
      {
        sum = 0.0;

        for (mrs_natural c=0; c < cols_; ++c)
          sum += (temp(r1,c) * temp(r2,c));

        sum /= cols_;
        res(r1,r2) = sum;
      }
    }
  }
  else
  {
    res.create(0);
    MRSERR("realvec::correlation() - inPlace operation not supported - returning empty result vector!");
  }
}

void
realvec::covariance(realvec& res) const
{
  //computes the (unbiased estimate) covariance between observations (as in MATLAB cov()).
  //Assumes data points (i.e. examples) in columns and features (i.e. observations) in rows (as usual in Marsyas0.2).
  //This method assumes non-standardized data (typical covariance calculation).
  if (size_ == 0)
  {
    MRSERR("realvec::covariance(): empty input matrix! returning empty covariance matrix!");
    res.create(0);
    return;
  }

  if (this != &res)
  {
    res.stretch(rows_, rows_); //covariance matrix
    //check if there are sufficient data points for a good covariance estimation...
    if (cols_ < (rows_ + 1))
    {
      MRSWARN("realvec::covariance() : nr. data points < nr. observations + 1 => covariance matrix is SINGULAR!");
    }
    if ( (mrs_real)cols_ < ((mrs_real)rows_*(mrs_real)(rows_-1.0)/2.0))
    {
      MRSWARN("realvec::covariance() : too few data points => ill-calculation of covariance matrix!");
    }

    realvec meanobs;
    this->meanObs(meanobs);//observation means //[TODO]

    mrs_real sum = 0.0;
    for (mrs_natural r1=0; r1< rows_; ++r1)
    {
      for (mrs_natural r2=0; r2 < rows_; ++r2)
      {
        sum = 0.0;
        for (mrs_natural c=0; c < cols_; ++c)
          sum += ((data_[c * rows_ + r1] - meanobs(r1)) * (data_[c * rows_ + r2]- meanobs(r2)));

        if (cols_ > 1)
          sum /= (cols_ - 1);//unbiased covariance estimate
        else
          sum /= cols_; //biased covariance estimate

        res(r1,r2) = sum;
      }
    }
  }
  else
  {
    res.create(0);
    MRSERR("realvec::covariance() - inPlace operation not supported - returning empty result vector!");
  }
}

void
realvec::covariance2(realvec& res) const
{
  //covariance matrix as computed in Marsyas0.1
  //computes the covariance between observations.
  //Assumes data points (i.e. examples) in columns and features (i.e. observations) in rows (as usual in Marsyas0.2).
  //This calculation assumes standardized data at its input...
  if (size_ == 0)
  {
    MRSERR("realvec::covariance() : empty input matrix! returning empty and invalid covariance matrix!");
    res.create(0);
    return;
  }
  if (this != &res)
  {
    res.stretch(rows_, rows_); //covariance matrix
    //check if there are sufficient data points for a good covariance estimation...
    if (cols_ < (rows_ + 1))
    {
      MRSWARN("realvec::covariance() : nr. data points < nr. observations + 1 => covariance matrix is SINGULAR!");
    }
    if ( (mrs_real)cols_ < ((mrs_real)rows_*(mrs_real)(rows_-1.0)/2.0))
    {
      MRSWARN("realvec::covariance() : too few data points => ill-calculation of covariance matrix!");
    }

    for (mrs_natural r1=0; r1< rows_; ++r1)
    {
      for (mrs_natural r2=0; r2 < rows_; ++r2)
      {
        mrs_real sum(0);

        for (mrs_natural c=0; c < cols_; ++c)
          sum += (data_[c * rows_ + r1] * data_[c * rows_ + r2]);

        sum /= cols_;
        res(r1,r2) = sum;
      }
    }
  }
  else
  {
    res.create(0);
    MRSERR("realvec::covariance2() - inPlace operation not supported - returning empty result vector!");
  }
}

mrs_real
realvec::trace() const
{
  if (cols_ != rows_)
  {
    MRSWARN("realvec::trace() - matrix is not square!");
  }

  mrs_real res(0);

  for (mrs_natural i = 0; i < size_;)
  {
    res += data_[i];
    i += cols_+1;
  }

  return res;
}

mrs_real
realvec::det() const
{
  NumericLib numlib;
  return numlib.determinant(*this);
}



// allocate data and initialize to zero
// minimum of 1 element is allocated to prevent null pointers
// not that the size_ of the object will stay 0 just like cols_
// Allocating data doen't have anything to do with the size count	of the object.
void realvec::allocateData(mrs_natural size)
{
  // if data is not null delete it
  delete[] data_;
  data_ = NULL;

  allocatedSize_ = size > 0 ? size : 1; // minimum of 1
  data_ = new mrs_real[allocatedSize_];

  for (mrs_natural i=0; i<allocatedSize_; ++i)
    data_[i] = 0.0;
}


//////////////////////////////////////////////////////////////////////////////

/////////////////////// THESE USED TO BE INLINED  ////////////////////////////






bool realvec::operator==(const realvec &v1) const
{
  //different size -> not equal
  if (v1.getRows()!=rows_) return false;
  if (v1.getCols()!=cols_) return false;

  for(int r=0; r<v1.getRows(); ++r)
  {
    for(int c=0; c<v1.getCols(); ++c)
    {
      if(v1(r,c)!=data_[c * rows_ + r])return false;
    }
  }
  return true;
}


bool realvec::operator!=(const realvec &v1)  const
{
  return !(*this == v1);
}

realvec&
realvec::operator+=(const realvec& rhs)
{
  if (size_ != rhs.size_)
    throw std::runtime_error("realvec: Trying to sum matrices of incompatible size.");

  for (mrs_natural i=0; i<size_; ++i)
    data_[i] += rhs.data_[i];
  return *this;
}


realvec&
realvec::operator-=(const realvec& rhs)
{
  if (size_ != rhs.size_)
    throw std::runtime_error("realvec: Trying to subtract matrices of incompatible size.");

  for (mrs_natural i=0; i<size_; ++i)
    data_[i] -= rhs.data_[i];
  return *this;
}


realvec&
realvec::operator*=(const realvec& rhs)
{
  if (size_ != rhs.size_)
    throw std::runtime_error("realvec: Trying to multiply matrices of incompatible size.");

  for (mrs_natural i=0; i<size_; ++i)
    data_[i] *= rhs.data_[i];
  return *this;
}


realvec&
realvec::operator/=(const realvec& rhs)
{
  if (size_ != rhs.size_)
    throw std::runtime_error("realvec: Trying to divide matrices of incompatible size.");

  for (mrs_natural i=0; i<size_; ++i)
  {
    MRSASSERT(rhs.data_[i] != 0);
    data_[i] /= rhs.data_[i];
  }
  return *this;
}

/**
 * \brief matrix Multiplication
 *
 * \param a first input matrix
 * \param b second input matrix
 * \param out preallocated realvec for the output
 * \return the value at the requested index.
 * \exception std::out_of_range is thrown when the index is out of bounds.
 */

void realvec::matrixMulti(const mrs_realvec& a,const mrs_realvec& b,mrs_realvec& out)
{
  //naive Matrix multiplication

  MRSASSERT(a.getCols()==b.getRows());
  MRSASSERT(out.getRows()==a.getRows());
  MRSASSERT(out.getCols()==b.getCols());

  out.setval (0.);

  for (mrs_natural r=0; r<out.getRows(); ++r)
  {
    for (mrs_natural c=0; c<out.getCols(); ++c)
    {
      for (mrs_natural i=0; i<a.getCols(); ++i)
      {
        out(r,c)+=a(r,i)*b(i,c);
      }
    }
  }
}



/**
 * \brief Get the value at index i or raise Exception when out of bounds.
 *
 * \param i the index to get the value at.
 * \return the value at the requested index.
 * \exception std::out_of_range is thrown when the index is out of bounds.
 */

mrs_real realvec::getValueFenced(const mrs_natural i) const
{

  if (i < 0 || i >= (mrs_natural)size_) {
    // TODO: use a Marsyas branded exception here?
    throw std::out_of_range("realvec indexing out of bounds.");
  }
  return data_[i];
}

/**
 * \brief Get the value at position (r, c) or raise Exception when out of bounds.
 *
 * \param r the row index of the position to get the value from.
 * \param c the column index of the position to get the value from.
 * \return the value at the requested position.
 * \exception std::out_of_range is thrown when the row or column index are out of bounds.
 */

mrs_real realvec::getValueFenced(const mrs_natural r, const mrs_natural c) const
{
  if (r < 0 || r >= rows_ || c < 0 || c >= cols_) {
    // TODO: use a Marsyas branded exception here?
    throw std::out_of_range("realvec indexing out of bounds.");
  }
  return data_[c * rows_ + r];
}

/**
 * \brief Get reference to value at index i or raise Exception when out of bounds.
 *
 * Returned reference can be used as left hand side value (lvalue).
 *
 * \param i the index to get the value at.
 * \return the value at the requested index.
 * \exception std::out_of_range is thrown when the index is out of bounds.
 */

mrs_real& realvec::getValueFenced(const mrs_natural i)
{
  if (i < 0 || i >= (mrs_natural)size_) {
    // TODO: use a Marsyas branded exception here?
    throw std::out_of_range("realvec indexing out of bounds.");
  }
  return data_[i];
}

/**
 * \brief Get reference to value at position (r, c) or raise Exception when out of bounds.
 *
 * Returned reference can be used as left hand side value (lvalue).
 *
 * \param r the row index of the position to get the value from.
 * \param c the column index of the position to get the value from.
 * \return the value at the requested position.
 * \exception std::out_of_range is thrown when the row or column index are out of bounds.
 */

mrs_real& realvec::getValueFenced(const mrs_natural r, const mrs_natural c)
{
  if (r < 0 || r >= rows_ || c < 0 || c >= cols_) {
    // TODO: use a Marsyas branded exception here?
    throw std::out_of_range("realvec indexing out of bounds.");
  }
  return data_[c * rows_ + r];
}


}
