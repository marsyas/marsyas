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


/** 
    \class realvec
    \brief Vector of real values

    Array (vector in the numerical sense) of real values. Basic 
arithmetic operations and statistics are supported. 
*/



#include "realvec.h"
using namespace std;


realvec::realvec()
{
  size_ = 0;
  data_ = NULL;
  rows_ = 1;
  cols_ = size_;
}


realvec::~realvec()
{
  if (size_ == 0)
    data_ = NULL;

  delete [] data_;
}

realvec::realvec(natural size)
{
  
  size_ = size;
  if (size_ > 0) 
    data_ = new real[size_];
  rows_ = 1;
  cols_ = size_;
}


realvec::realvec(natural rows, natural cols)
{
  
  size_ = rows * cols;
  if (size_ > 0) 
    data_ = new real[size_];
  rows_ = rows;
  cols_ = cols;
}


realvec::realvec(const realvec& a) : size_(a.size_), 
				   rows_(a.rows_), cols_(a.cols_)
{

  data_ = NULL;
  if( a.size_ > 0 )
    data_ = new real[a.size_];
  for (natural i=0; i<size_; i++)
    data_[i] = a.data_[i];
}


realvec& 
realvec::operator=(const realvec& a)
{
  if (this != &a)
    {
      if (size_ != a.size_)
	{
	  MRSERR("realvec::operator= : Different realvec sizes\n");
	  MRSERR("realvec left unchanged\n");
	  
	  MRSERR("Left size = " + size_);
	  MRSERR("Right size = " + a.size_);
	  return *this;
	}
      for (natural i=0; i < size_; i++)
	data_[i] = a.data_[i];
      rows_ = a.rows_;
      cols_ = a.cols_;
    }
  return *this;
}


real *
realvec::getData()
{
  return data_;
}

real 
realvec::mean()
{
  real sum = 0.0;
  for (natural i=0; i < size_; i++)
    {
      sum += data_[i];
    }
  if (sum != 0.0) sum /= size_;
  return sum;
}


void
realvec::meanSample(realvec &means)
{
   means.create(rows_);
   
   for( natural i=0 ; i<rows_ ; i++ )
   {
      for( natural j=0 ; j<cols_ ; j++ )
      {
         means.data_[i] += data_[rows_*j + i];
      }
      if( cols_ != 0 )
         means.data_[i] /= cols_;
   }
}


real 
realvec::sum()
{
  real sum = 0.0;
  for (natural i=0; i < size_; i++)
    {
      sum += data_[i];
    }
  return sum;
}


real 
realvec::var()
{
  real sum = 0.0;
  real sum_sq = 0.0;
  real val;
  real var;
  
  for (natural i=0; i < size_; i++)
    {
      val = data_[i];
      sum += val;
      sum_sq += (val * val);
    }
  if (sum != 0.0) sum /= size_;
  if (sum_sq != 0.0) sum_sq /= size_;
  
  var = sum_sq - sum * sum;
  return var;
}


real 
realvec::std()
{
  return sqrt(var());
}


void
realvec::stdSample(realvec &stds)
{
   realvec means;
   meanSample(means);
   stdSample(means,stds);
}


void
realvec::stdSample(realvec &means, realvec &stds)
{
   stds.create(rows_);
   
   for( natural i=0 ; i<rows_ ; i++ )
   {
      for( natural j=0 ; j<cols_ ; j++ )
      {
         stds.data_[i] += ( data_[rows_*j + i] - means.data_[i] ) * 
         ( data_[rows_*j + i] - means.data_[i] );
      }
      if( cols_ != 0 ){
         stds.data_[i] /= cols_;
         stds.data_[i] = sqrt(stds.data_[i]);
      }
      
   }
}

 
natural 
realvec::getRows()
{
  return rows_;
}


natural 
realvec::getCols()
{
  return cols_;
}


natural
realvec::getSize() const
{
  return size_;
}

void 
realvec::debug_info()
{
  MRSERR("realvec information");
  MRSERR("size = " + size_);
  
}



void 
realvec::create(natural size)
{
  delete [] data_;
  size_ = size;
  if (size_ > 0) 
    data_ = new real[size_];
  rows_ = 1;
  cols_ = size;
  
    
  for (natural i=0; i<size_; i++)
    data_[i] = 0.0;
}


/* keep the old data and possibly extend */ 
void 
realvec::stretch(natural size) 
{
  if (size_ == size) 
    return;
  
  real *ndata = NULL;
  if (size > 0) 
    ndata = new real[size];
  // zero new data 
  for (natural i=0; i < size; i++)
    {
      if (i < size_) 
	ndata[i] = data_[i];
      else 
	ndata[i] = 0.0;
    }
  delete [] data_;
  data_ = ndata;
  size_ = size;
  rows_ = 1;
  cols_ = size;
  
}


/* keep the old data and possibly extend */ 
void 
realvec::stretch(natural rows, natural cols) 
{
  if ((rows == rows_)&&(cols == cols_))
    return;
  
  real *ndata = NULL;
  int size = rows * cols;
  if (size > 0) 
    ndata = new real[size];

  
  
  // copy and zero new data 
  for (natural r=0; r < rows; r++)
    for (natural c = 0; c < cols; c++) 
      {
	if ((r < rows_)&&(c < cols_)) 
	  ndata[c * rows + r] = data_[c * rows_ + r];
	else 
	  ndata[c * rows + r] = 0.0;
      }
  if (data_) 
    delete [] data_;
  data_ = ndata;
  size_ = size;
  rows_ = rows;
  cols_ = cols;
}




void 
realvec::create(natural rows, natural cols)
{
  delete [] data_;
  size_ = rows * cols;
  rows_ = rows;
  cols_ = cols;
  if (size_ > 0) 
    data_ = new real[size_];
  for (natural i=0; i<size_; i++)
    data_[i] = 0.0;
}


//Inverse-------------------------------

int 
realvec::invert(realvec& res)
{
  int rank;
  //assert(rows_ == cols_); how do we do this in 0.2.1 ? checkflow?
  natural r,c,i;
  real temp;
  
  rank = 0;
  for (r = 0; r < rows_; r++)
    for (c=0; c < cols_; c++)
      {
	if (r == c) 
	  res(r,c) = 1.0;
	else 
	  res(r,c) = 0.0;
      }
  for (i = 0; i < rows_; i++)
    {
      if ((*this)(i,i) == 0)
	{
	  for (r = i; r < rows_; r++)
	    for (c = 0; c < cols_; c++)
	      {
		(*this)(i,c) += (*this)(r,c);
		res(i,c) += res(r,c);
	      }
	}
      for (r = i; r < rows_; r++) 
	{
	  temp = (*this)(r,i);
	  if (temp != 0) 
	    for (c =0; c < cols_; c++)
	      {
		(*this)(r,c) /= temp;
		res(r,c) /= temp;
	      }
	}
      if (i != rows_-1)
	{
	  for (r = i+1; r < rows_; r++)
	    {
	      temp = (*this)(r,i);
	      if (temp != 0.0) 
		for (c=0; c < cols_; c++)
		  {
		    (*this)(r,c) -= (*this)(i,c);
		    res(r,c) -= res(i,c);
		  }
	    }
	}
    }
  for (i=1; i < rows_; i++)
    for (r=0; r < i; r++)
      {
	temp = (*this)(r,i);
	for (c=0; c < cols_; c++)
	  {
	    (*this)(r,c) -= (temp * (*this)(i,c));
	    res(r,c) -= (temp * res(i,c));
	  }
      }
  for (r =0; r < rows_; r++)
    for (c=0; c < cols_; c++)
      (*this)(r,c) = res(r,c);
  return rank;
}
//-------------------------------------



void 
realvec::create(real val, natural rows, natural cols)
{
  size_ = rows * cols;
  rows_ = rows;
  cols_ = cols;
  delete [] data_;
  if (size_ > 0) 
    data_ = new real[size_];
  for (natural i=0; i<size_; i++)
    data_[i] = val;
}



void 
realvec::allocate(natural size)
{
  delete [] data_;
  size_ = size;
  if (size_ > 0) 
    data_ = new real[size_];
}



void 
realvec::setval(natural start, natural end, real val)
{
  MRSASSERT(start <= size_);
  MRSASSERT(end <= size_);

  for (natural i=start; i<end; i++)
    {
      data_[i] = val;
    }
}


void 
realvec::setval(real val)
{
  for (natural i=0; i<size_; i++)
    {
      data_[i] = val;
    }
}

void 
realvec::abs()
{
  for (natural i=0; i<size_; i++)
    {
      data_[i] = fabs(data_[i]);
    }
  
}

void 
realvec::renorm(real old_mean, real old_std, real new_mean, real new_std)
{
  natural i;
  
  for(i=0; i < size_; i++)
    {
      data_[i] = (data_[i] - old_mean) / old_std;
      data_[i] *= new_std;
      data_[i] += new_mean;
    }
}


void
realvec::sqr()
{
  for (natural i=0; i<size_; i++)
    {
      data_[i] *= data_[i];
    }
}


void
realvec::sqroot()
{
  for (natural i=0; i<size_; i++)
    {
      data_[i] = sqrt(data_[i]);
    }
}


realvec 
operator+(const realvec& vec1, const realvec& vec2)
{
  natural size;
  natural i;
  if (vec1.size_ != vec2.size_)
    MRSERR("Size of realvecs does not match");
  if (vec1.size_ >= vec2.size_)
    size = vec1.size_;
  else 
    size = vec2.size_;
  realvec sum;
  sum.create(size);    
  
  for (i=0; i<vec1.size_; i++)
    {
      sum.data_[i] = vec1.data_[i];
    }
  for (i=0; i<vec2.size_; i++)
    {
      sum.data_[i] += vec2.data_[i];
    }
      
  return sum;
}


realvec 
operator-(const realvec& vec1, const realvec& vec2)
{
  natural size;
  natural i;
  if (vec1.size_ != vec2.size_)
    MRSERR("Size of realvecs does not match");
  if (vec1.size_ >= vec2.size_)
    size = vec1.size_;
  else 
    size = vec2.size_;
  realvec diff;
  diff.create(size);    
  
  for (i=0; i<vec1.size_; i++)
    {
      diff.data_[i] = vec1.data_[i];
    }
  for (i=0; i<vec2.size_; i++)
    {
      diff.data_[i] -= vec2.data_[i];
    }
      
  return diff;
}

// Jen

realvec&
realvec::operator()(std::string r, std::string c)
{
   natural r_l = r.length();
   natural c_l = c.length();
   
   natural r_c = r.find(":");
   natural c_c = c.find(":");
   
   natural r_a;
   natural r_b;
   
   natural c_a;
   natural c_b;
   
   char *endptr;
   
   MRSASSERT( (r_c == 0 && r_l == 1) || (r_c == string::npos) || (r_c>0 && r_l-r_c>1) );
   MRSASSERT( (c_c == 0 && c_l == 1) || (c_c == string::npos) || (c_c>0 && c_l-c_c>1) );
   
   if( r_c != string::npos && r_l > 1 )
   {
      r_a = (natural)strtol( r.substr(0,r_c).c_str() , &endptr , 10  );
      MRSASSERT( *endptr == '\0' );
      r_b = (natural)strtol( r.substr(r_c+1,r_l-r_c-1).c_str() , &endptr , 10  );
      MRSASSERT( *endptr == '\0' );
   }
   else if( r_c == string::npos )
   {
      r_a = r_b = (natural)strtol( r.c_str() , &endptr , 10 );
      MRSASSERT( *endptr == '\0' );
   }
   else
   {
      r_a = 0;
      r_b = rows_-1;
   }
   
   MRSASSERT( r_a >= 0 && r_b < rows_ );
   
   if( c_c != string::npos && c_l > 1 )
   {
      c_a = (natural)strtol( c.substr(0,c_c).c_str() , &endptr , 10  );
      MRSASSERT( *endptr == '\0' );
      c_b = (natural)strtol( c.substr(c_c+1,c_l-c_c-1).c_str() , &endptr , 10 );      
      MRSASSERT( *endptr == '\0' );
   }
   else if( c_c == string::npos )
   {
      c_a = c_b = (natural)strtol( c.c_str() , &endptr , 10 );
      MRSASSERT( *endptr == '\0' );
   }
   else
   {
      c_a = 0;
      c_b = cols_-1;
   }
   
   MRSASSERT( c_a >= 0 && c_b < cols_ );
   
   r_l = r_b - r_a + 1;
   c_l = c_b - c_a + 1;
   
   realvec matrix;
   
   matrix.create( r_l , c_l );
   
   for( c_c = c_a ; c_c <= c_b ; c_c++ )
   {
      for( r_c = r_a ; r_c <= r_b ; r_c++ )
      {
         matrix.data_[(c_c-c_a) * r_l + (r_c-r_a)] = data_[c_c * rows_ + r_c];
      }
   }       
   
   return matrix;
}

realvec&
realvec::operator()(std::string c)
{
   natural c_l = c.length();
   
   natural c_c = c.find(":");
   
   natural c_a;
   natural c_b;
   
   char *endptr;
   
   MRSASSERT( (c_c == 0 && c_l == 1) || (c_c == string::npos) || (c_c>0 && c_l-c_c>1) );
   
   if( c_c != string::npos && c_l > 1 )
   {
      c_a = (natural)strtol( c.substr(0,c_c).c_str() , &endptr , 10  );
      MRSASSERT( *endptr == '\0' );
      c_b = (natural)strtol( c.substr(c_c+1,c_l-c_c-1).c_str() , &endptr , 10  );      
      MRSASSERT( *endptr == '\0' );
   }
   else if( c_c == string::npos )
   {
      c_a = c_b = (natural)strtol( c.c_str() , &endptr , 10 );
      MRSASSERT( *endptr == '\0' );
   }
   else
   {
      c_a = 0;
      c_b = (rows_*cols_)-1;
   }
   
   MRSASSERT( c_a >= 0 && c_b < rows_*cols_ );   
   
   c_l = c_b - c_a + 1;
   
   realvec matrix;
   
   matrix.create( c_l );
   
   for( c_c = c_a ; c_c <= c_b ; c_c++ )
   {
      matrix.data_[(c_c-c_a)] = data_[c_c];
   }
   
   return matrix;
}

// Jen end

void 
realvec::send(Communicator *com)
{ 
  natural i;
  static char *buf = new char[256];
  string message;
  sprintf(buf, "%i\n", (int)size_);
  message = buf;
  com->send_message(message);
  for (i=0; i<size_; i++)
    {
      sprintf(buf, "%f\n", data_[i]);
      message = buf;
      com->send_message(message);
    }
  MRSERR("realvec::send numbers were sent to the client");
  
}

void
realvec::read(string filename)
{
  ifstream from(filename.c_str());
  from >> (*this);  
}


void 
realvec::shuffle() 
{
  natural size = cols_;
  natural rind;
  real temp;
  
  for (int c=0;  c < cols_; c++)
    {
      rind = (unsigned int)(((real)rand() / (real)(RAND_MAX))*size);
      for (int r=0; r < rows_; r++) 
	{
	  temp = data_[c * rows_ + r];
	  data_[c * rows_ + r] = data_[rind * rows_ + r];
	  data_[rind * rows_ + r] = temp;
	}
    }  
}


void 
realvec::write(string filename)
{
  ofstream os(filename.c_str());
  os << (*this) << endl;
}



ostream& 
operator<< (ostream& o, const realvec& vec)
{
  o << "# MARSYAS realvec" << endl;
  o << "# Size = " << vec.size_ << endl << endl;
  o << endl;

  
  o << "# type: matrix" << endl;
  o << "# rows: " << vec.rows_ << endl;
  o << "# columns: " << vec.cols_ << endl;

  for (natural r=0; r < vec.rows_; r++)
    {
      for (natural c=0; c < vec.cols_; c++)
	o << vec.data_[c * vec.rows_ + r] << " " ;
      o << endl;
    }
  
  // for (natural i=0; i<vec.size_; i++)
  // o << vec.data_[i] << endl;
  o << endl;
  o << "# Size = " << vec.size_ << endl;
  o << "# MARSYAS realvec" << endl;  
  return o;
}

istream& 
operator>>(istream& is, realvec& vec)
{
  if (vec.size_ != 0)
    {
      MRSERR("realvec::operator>>: realvec already allocated cannot read from istream");
      MRSERR("vec.size_ = " + vec.size_);
      
      
      return is;
    }
  string str0,str1,str2;
  natural size;
  natural i;
  
  is >> str0;
  is >> str1;
  is >> str2;
  
  if ((str0 != "#") || (str1 != "MARSYAS") || (str2 != "realvec"))
    {
      MRSERR("realvec::operator>>: Problem1 reading realvec object from istream");
      return is;
    }
  is >> str0;				
  is >> str1;
  is >> str2;
  if ((str0 != "#") || (str1 != "Size") || (str2 != "="))
    {
      MRSERR("realvec::operator>>: Problem2 reading realvec object from istream");
      return is;
    }
  is >> size;

  vec.allocate(size);  
  for (i=0; i<3; i++)
    {
      is >> str0;
    }
  is >> str0 >> str1 >> vec.rows_;
  is >> str0 >> str1 >> vec.cols_;
  

  for (natural r = 0; r < vec.rows_; r++)
    for (natural c = 0; c < vec.cols_; c++)
      {
	is >> vec.data_[c * vec.rows_ + r];
      }


  is >> str0;				
  is >> str1;
  is >> str2;
  if ((str0 != "#") || (str1 != "Size") || (str2 != "="))
    {
      MRSERR("realvec::operator>>: Problem3 reading realvec object from istream");
      return is;
    }
  is >> size;
  
  is >> str0;
  is >> str1;
  is >> str2;
  
  if ((str0 != "#") || (str1 != "MARSYAS") || (str2 != "realvec"))
    {
      MRSERR("realvec::operator>>: Problem4 reading realvec object from istream");

      return is;
    }
  return is;
}



