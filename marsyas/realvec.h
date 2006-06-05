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
    \brief Vector of real values

    Array (vector in the numerical sense) of real values. Basic 
arithmetic operations and statistics are supported. 
*/

	

#ifndef MARSYAS_REALVEC_H
#define MARSYAS_REALVEC_H 1



#include <math.h>
#include "common.h"


#include <string>
#include <iostream>
#include <fstream>


#include "MrsLog.h"
#include "Communicator.h"

class realvec 
{
private:
  real *data_;
  natural size_;
  natural rows_;
  natural cols_;

  
  
public:
  realvec();
  ~realvec();
  realvec(natural size);
  realvec(natural rows, natural cols);
  
  realvec(const realvec& a);
  realvec& operator=(const realvec& a);
  

  void create(natural size);		// allocate(size) + fill zeros
  void stretch(natural size);		// allocate(size) + keep old vals
  void allocate(natural size);	        // just allocate(size)  

  void create(natural rows, natural cols); 
  void stretch(natural rows, natural cols); // allocate(size) + keep old vals
  void create(real val, natural rows, natural cols);	
  void allocate(natural rows, natural cols);	        
  int  invert(realvec& res);


  void setval(natural start, natural end, real val);
  void setval(real val);			// set all entries to val 
  natural getSize() const;
  natural getCols();
  natural getRows();
  
  real *getData();			// dirty for easy integration 


  void shuffle();
  

  // vector operations 
  realvec& operator+=(const realvec& vec);
  realvec& operator-=(const realvec& vec);
  realvec& operator*=(const realvec& vec);
  realvec& operator/=(const realvec& vec);
  realvec& operator*=(const real val);
  realvec& operator/=(const real val);
  realvec& operator+=(const real val);
  realvec& operator-=(const real val);

  friend realvec operator+(const realvec& vec1, const realvec& vec2);
  friend realvec operator-(const realvec& vec1, const realvec& vec2);
  
  
  // item access
  real& operator()(const natural i);
  real operator()(const natural i) const;
  real& operator()(const long r, const long c);
  real operator()(const long r, const long c) const;
  
  // output functions 
  void debug_info();
  void write(std::string filename);
  void read(std::string filename);
  friend std::ostream& operator<<(std::ostream&, const realvec&);
  friend std::istream& operator>>(std::istream&, realvec&);


  // Statistics 
  real mean();
  real sum();
  real std();
  real var();
  void abs();
  void sqr();
  void sqroot();
  void renorm(real old_mean, real old_std, real new_mean, real new_std);
  void send(Communicator *com);
};






inline 
realvec&
realvec::operator/=(const real val)
{
  for (natural i=0; i<size_; i++)
    data_[i] /= val;
  return *this;
}


inline 
realvec&
realvec::operator*=(const real val)
{
  for (natural i=0; i<size_; i++)
    data_[i] *= val;
  return *this;
}




inline 
realvec&
realvec::operator-=(const real val)
{
  for (natural i=0; i<size_; i++)
    data_[i] -= val;
  return *this;
}

inline 
realvec&
realvec::operator+=(const real val)
{
  for (natural i=0; i<size_; i++)
    data_[i] += val;
  return *this;
}



  
inline
realvec& 
realvec::operator+=(const realvec& vec)
{
  for (natural i=0; i<size_; i++)
    data_[i] += vec.data_[i];
  return *this;
}

inline
realvec& 
realvec::operator-=(const realvec& vec)
{
  for (natural i=0; i<size_; i++)
    data_[i] -= vec.data_[i];
  return *this;
}







inline
realvec& 
realvec::operator*=(const realvec& vec)
{
  for (natural i=0; i<size_; i++)
    data_[i] *= vec.data_[i];
  return *this;
}

inline
realvec& 
realvec::operator/=(const realvec& vec)
{
  for (natural i=0; i<size_; i++)
    data_[i] /= vec.data_[i];
  return *this;
}
  

inline 
real realvec::operator()(const long r, const long c) const
{
  MRSASSERT(r < rows_);
  MRSASSERT(c < cols_);

  return data_[c * rows_ + r];
  // return data_[r * cols_ + c];
}


inline 
real& realvec::operator()(const long r, const long c)
{
  
  MRSASSERT(r < rows_);
  MRSASSERT(c < cols_);

  



  return data_[c * rows_ + r];  
}



inline 
real realvec::operator()(const natural i) const
{
  MRSASSERT(i < size_);
  return data_[i];
}


inline 
real& realvec::operator()(const natural i)
{
  
  MRSASSERT(i < size_);
  
  return data_[i];
}



#endif /* !MARSYAS_REALVEC_H */


