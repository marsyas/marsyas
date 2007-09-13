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
/** 
    \class realvec
	\ingroup NotmarCore
    \brief Vector of mrs_real values

    Array (vector in the numerical sense) of mrs_real values. Basic 
arithmetic operations and statistics are supported. 


\todo Crash-proof certain functions in realvec (like calling median() on
an empty realvec)

\todo document realvec functions.  In detail.  Using all the doxygen
tricks.  For something as basic as this, it's worth it.
*/


class realvec 
{
protected:
	/// total number of values in data_
  mrs_natural size_;
	/// total memory allocated for data_
  mrs_natural allocatedSize_;
	/// number of rows in array; for a one-dimensional array, this is 1.
  mrs_natural rows_;
	/// number of columns in array.
  mrs_natural cols_;

	/// the actual array
  mrs_real *data_;
  
public:
  realvec();
  virtual ~realvec();
  realvec(mrs_natural size);
  realvec(mrs_natural rows, mrs_natural cols);
  realvec(const realvec& a);
  
  realvec& operator=(const realvec& a);

	/** \name Memory allocation */
	//@{
  void allocate(mrs_natural size);  
  void allocate(mrs_natural rows, mrs_natural cols);	

	///allocate(size) + fill with zeros
  void create(mrs_natural size);
	///allocate(rows,cols) + fill with zeros
  void create(mrs_natural rows, mrs_natural cols);
	///allocate(rows,cols) + fill with val
  void create(mrs_real val, mrs_natural rows, mrs_natural cols);

	/// allocate(size) + keep old vals.  May also be used to shrink realvec.
  void stretch(mrs_natural rows, mrs_natural cols);
	/// allocate(size) + keep old vals.  May also be used to shrink realvec.
  void stretch(mrs_natural size);

  	/// write to array, stretching the array if necessary
  void stretchWrite(const mrs_natural pos, const mrs_real val);
  	/// write to array, stretching the array if necessary
  void stretchWrite(const mrs_natural r, const mrs_natural c, const mrs_real val);
	//@}
  
	/** \name Modify values in array */
	//@{
	/// set all entries to val 
  void setval(mrs_natural start, mrs_natural end, mrs_real val);
	/// set all entries to val 
  void setval(mrs_real val);
	/// appends values from the newValues realvec.  Resizes the original
	/// realvec to make space for the new values.
  void appendRealvec(const realvec newValues);
	//@}
  
	/** \name other */
	//@{
	/// apply a given function to all the elements
  void apply(mrs_real (*func) (mrs_real));
	//@}

	/** \name Getting information */
	//@{
  mrs_natural getSize() const;
  mrs_natural getCols() const;
  mrs_natural getRows() const;
	/// extracts a subset of a realvec.  One-dimensional realvecs only.
  realvec getSubVector(mrs_natural startPos, mrs_natural length) const;
	/// dirty for easy integration 
  mrs_real *getData() const;
	//@}


  void shuffle();
  
	/** \name Vector Operations */
	//@{
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
  friend realvec operator*(const realvec& vec1, const realvec& vec2);
  friend realvec operator/(const realvec& vec1, const realvec& vec2);
  friend bool operator!=(const realvec& v1, const realvec& v2);
	//@}
   
	/** \name Item access */
	//@{
  mrs_real& operator()(const mrs_natural i);
  mrs_real operator()(const mrs_natural i) const;
  mrs_real& operator()(const mrs_natural r, const mrs_natural c);
  mrs_real operator()(const mrs_natural r, const mrs_natural c) const;
	//@}


	/** \name Indexing*/
	//@{
	/// matlab-like
  realvec operator()(std::string r, std::string c);
	/// matlab-like
  realvec operator()(std::string r);
	/// vector indexing
  void getRow(const mrs_natural r, realvec& res) const;
	/// vector indexing
  void getCol(const mrs_natural c, realvec& res) const;
	//@}

	/** \name Output functions */
	//@{
  void debug_info();
  void dump();
  bool write(std::string filename) const;
  bool read(std::string filename);
  friend std::ostream& operator<<(std::ostream&, const realvec&);
  friend std::istream& operator>>(std::istream&, realvec&);
	/// input function for line-separated text files
  bool readText(std::string filename);
	/// output function for line-separated text files
  bool writeText(std::string filename);
	//@}

	/** \name Observations statics */
	//@{
  void meanObs(realvec& res) const;
  void stdObs(realvec& res) const;
  void varObs(realvec& res) const;
  void normObs();
  void normSpl(mrs_natural=0);
  void normObsMinMax();
  void normSplMinMax(mrs_natural=0);
	//@}

	/** \name Vector/Matrix Algebra and Statistics */
	//@{
  mrs_real maxval(mrs_natural *index=NULL) const;
  mrs_real minval() const;
  mrs_real mean() const;
  mrs_real median() const;
  mrs_real sum() const;
  mrs_real std() const;
  mrs_real var() const;
  void sort();
  void abs();
  void sqr();
  void sqroot();
  void norm();
  void norm(mrs_real mean, mrs_real std);
  void renorm(mrs_real old_mean, mrs_real old_std, mrs_real new_mean, mrs_real new_std);
  mrs_natural invert(realvec& res);//lmartins: this seems to be both inplace and returning the inverse matrix in "res"... why both?!? [!][?] 
  void transpose();
  void covariance(realvec& res) const; //Typical covariance calculation, as performed by MATLAB cov(). 
  void covariance2(realvec& res) const;//Marsyas0.1 covariance calculation (assumes standardized data, and uses biased estimation) 
  void correlation(realvec& res) const; 
  mrs_real trace() const; 
  mrs_real det() const; 
	//@}

	/** \name Communications */
	//@{
  void send(Communicator *com);
  mrs_natural search(mrs_real val);
	//@}
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
mrs_real realvec::operator()(const mrs_natural r, const mrs_natural c) const
{
  MRSASSERT(r < rows_);
  MRSASSERT(c < cols_);

  return data_[c * rows_ + r];
  // return data_[r * cols_ + c];
}

inline 
mrs_real& realvec::operator()(const mrs_natural r, const mrs_natural c)
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

