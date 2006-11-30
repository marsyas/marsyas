/*
** Copyright (C) 1998-2004 George Tzanetakis <gtzan@cs.uvic.ca>
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
    \brief Vector of mrs_real values

    Array (vector in the numerical sense) of mrs_real values. Basic 
arithmetic operations and statistics are supported. 
*/

#ifndef MARSYAS_REALVEC_H
#define MARSYAS_REALVEC_H 

#include <cmath>
#include <string>
#include <iostream>
#include <fstream>

#include "common.h"
#include "MrsLog.h"
#include "Communicator.h"

namespace Marsyas
{

class realvec 
{
private:
  mrs_real *data_;
  mrs_natural size_;
	mrs_natural allocatedSize_;
  mrs_natural rows_;
  mrs_natural cols_;
  
public:
  realvec();
  ~realvec();
  realvec(mrs_natural size);
  realvec(mrs_natural rows, mrs_natural cols);
  realvec(const realvec& a);
  realvec& operator=(const realvec& a);

  void allocate(mrs_natural size);//just allocate(size)  
  void allocate(mrs_natural rows, mrs_natural cols);	

  void create(mrs_natural size);// allocate(size) + fill zeros
  void create(mrs_natural rows, mrs_natural cols); 
  void create(mrs_real val, mrs_natural rows, mrs_natural cols);	
  
  void stretch(mrs_natural rows, mrs_natural cols); // allocate(size) + keep old vals
  void stretch(mrs_natural size);// allocate(size) + keep old vals

  void setval(mrs_natural start, mrs_natural end, mrs_real val);// set all entries to val 
  void setval(mrs_real val);// set all entries to val 
  
	void apply(mrs_real (*func) (mrs_real));// apply a given function to all the elts

  mrs_natural getSize() const;
  mrs_natural getCols() const;
  mrs_natural getRows() const;
  
  mrs_real *getData() const;// dirty for easy integration 

  void shuffle();
  
  // vector operations 
  realvec& operator+=(const realvec& vec);
  realvec& operator-=(const realvec& vec);
  realvec& operator*=(const realvec& vec);
  realvec& operator/=(const realvec& vec);
  realvec& operator*=(const mrs_real val);
  realvec& operator/=(const mrs_real val);
  realvec& operator+=(const mrs_real val);
  realvec& operator-=(const mrs_real val);

  friend realvec operator+(const realvec& vec1, const realvec& vec2);
  friend realvec operator-(const realvec& vec1, const realvec& vec2);
	friend bool operator!=(const realvec& v1, const realvec& v2);
   
  // item access
  mrs_real& operator()(const mrs_natural i);
  mrs_real operator()(const mrs_natural i) const;
  mrs_real& operator()(const long r, const long c);
  mrs_real operator()(const long r, const long c) const;
  // Matlab-like indexing
  realvec operator()(std::string r, std::string c);//Jen
  realvec operator()(std::string r);//Jen
  // vector indexing
  realvec getRow(const mrs_natural r) const;//lmartins
  realvec getCol(const mrs_natural c) const;//lmartins

  // output functions 
  void debug_info();
  void write(std::string filename) const;
	void dump();
  void read(std::string filename);
  friend std::ostream& operator<<(std::ostream&, const realvec&);
  friend std::istream& operator>>(std::istream&, realvec&);

  // Observations statistics 
  realvec meanObs() const;//lmartins
  realvec stdObs() const;//lmartins
  realvec varObs() const;//lmartins
  void normObs();//lmartins
	 void normSpl(mrs_natural=0);//lmartins
 void normObsMinMax();//lmartins
	 void normSplMinMax(mrs_natural=0);//lmartins

  // Vector/Matrix Algebra and Statistics
  mrs_real maxval(mrs_natural *index=NULL) const;//lmartins
  mrs_real minval() const;//lmartins
  mrs_real mean() const;
  mrs_real sum() const;
  mrs_real std() const;
  mrs_real var() const;
  void abs();
  void sqr();
  void sqroot();
  void renorm(mrs_real old_mean, mrs_real old_std, mrs_real new_mean, mrs_real new_std);
  mrs_natural invert(realvec& res);//lmartins: this seems to be both inplace and returning the inverse matrix in "res"... why both?!? [!][?] 

  realvec covariance() const; //Typical covariance calculation, as performed by MATLAB cov(). //lmartins
  realvec covariance2() const;//Marsyas0.1 covariance calculation (assumes standardized data, and uses biased estimation) //lmartins
  realvec correlation() const; //lmartins
  mrs_real trace() const; //lmartins
  mrs_real det() const; //lmartins
  
  // Distances and metrics (should this be here?)[?]
  static mrs_real divergenceShape(realvec Ci, realvec Cj);//lmartins 
  static mrs_real bhattacharyyaShape(realvec Ci, realvec Cj); //lmartins

  // Communications
  void send(Communicator *com);

};


inline 
realvec&
realvec::operator/=(const mrs_real val)
{
  for (mrs_natural i=0; i<size_; i++)
    data_[i] /= val;
  return *this;
}


inline 
realvec&
realvec::operator*=(const mrs_real val)
{
  for (mrs_natural i=0; i<size_; i++)
    data_[i] *= val;
  return *this;
}

inline 
realvec&
realvec::operator-=(const mrs_real val)
{
  for (mrs_natural i=0; i<size_; i++)
    data_[i] -= val;
  return *this;
}

inline 
realvec&
realvec::operator+=(const mrs_real val)
{
  for (mrs_natural i=0; i<size_; i++)
    data_[i] += val;
  return *this;
}
  
inline
realvec& 
realvec::operator+=(const realvec& vec)
{
  for (mrs_natural i=0; i<size_; i++)
    data_[i] += vec.data_[i];
  return *this;
}

inline
realvec& 
realvec::operator-=(const realvec& vec)
{
  for (mrs_natural i=0; i<size_; i++)
    data_[i] -= vec.data_[i];
  return *this;
}

inline
realvec& 
realvec::operator*=(const realvec& vec)
{
  for (mrs_natural i=0; i<size_; i++)
    data_[i] *= vec.data_[i];
  return *this;
}

inline
realvec& 
realvec::operator/=(const realvec& vec)
{
  for (mrs_natural i=0; i<size_; i++)
    data_[i] /= vec.data_[i];
  return *this;
}
 
inline 
mrs_real realvec::operator()(const long r, const long c) const
{
  MRSASSERT(r < rows_);
  MRSASSERT(c < cols_);

  return data_[c * rows_ + r];
  // return data_[r * cols_ + c];
}

inline 
mrs_real& realvec::operator()(const long r, const long c)
{
  MRSASSERT(r < rows_);
  MRSASSERT(c < cols_);

  return data_[c * rows_ + r];  
}

inline 
mrs_real realvec::operator()(const mrs_natural i) const
{
  MRSASSERT(i < size_);
  return data_[i];
}

inline 
mrs_real& realvec::operator()(const mrs_natural i)
{
  
  MRSASSERT(i < size_);
  
  return data_[i];
}

}//namespace Marsyas

#endif /* !MARSYAS_REALVEC_H */

