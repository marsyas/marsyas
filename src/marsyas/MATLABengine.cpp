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


/**
	\class MATLABengine
	\brief Utility class for exchanging data between Marsyas and MATLAB

	In order to use this class in Marsyas, _MARSYAS_ENGINE_ must be defined.
	Check out the test and demonstration code at the marsyasTests.cpp.
	More info on how to build with MATLAB engine support at:

	http://www.mathworks.com/access/helpdesk/help/techdoc/matlab_external/f39903.html

	Code by Luís Gustavo Martins <lmartins@inescporto.pt>
*/

#include "common_source.h"
#include "MATLABengine.h"

#ifdef MARSYAS_MATLAB
using std::vector;
using std::complex;


using namespace Marsyas;

MATLABengine * MATLABengine::instance_ = 0;

MATLABengine::MATLABengine()
{
  if (!(engine_ = engOpen("\0")))
  {
    MRSERR("Can't start MATLAB Engine! Check MATLAB install and configuration!\n");
    return;
  }
  instance_ = this;

  /*int BUFSIZE=200;
    buffer_ = new char[BUFSIZE];
    buffer_[BUFSIZE] = '\0';
    engOutputBuffer(engine_, buffer_, BUFSIZE);*/
}


MATLABengine::~MATLABengine()
{
//	delete [] buffer_;
  engClose(engine_);
}

MATLABengine*
MATLABengine::getMatlabEng()
{
  if(!instance_)
    instance_ = new MATLABengine();

  return instance_;
}


void
MATLABengine::closeMatlabEng()
{
  delete instance_;
  instance_=0;
}


//-------------------------------------------------------------------
//					setters
//-------------------------------------------------------------------
void
MATLABengine::evalString(const std::ostringstream& MATLABcmd)
{
  engEvalString(engine_, MATLABcmd.str().c_str());
  // cout << buffer_ << endl;
}

void
MATLABengine::putVariable(const mrs_string value, mrs_string MATLABname)
{
  //-----------------------------------
  //send C/C++ string to MATLAB string
  //-----------------------------------

  mxArray *mxVector = mxCreateString(value.c_str());
  engPutVariable(engine_, MATLABname.c_str(), mxVector);

  mxDestroyArray(mxVector);
}

void
MATLABengine::putVariable(const long *const value, unsigned int size, mrs_string MATLABname)
{
  //-----------------------------------
  //send C/C++ vector to MATLAB vector
  //-----------------------------------
  mwSize dims[2];
  dims[0] = 1; //row vector
  dims[1] = size;

  mxArray *mxVector = mxCreateNumericArray(2, dims, mxINT32_CLASS, mxREAL);
  memcpy(mxGetData(mxVector), (void *)value, size*mxGetElementSize(mxVector));
  engPutVariable(engine_, MATLABname.c_str(), mxVector);

  //Convert to MATLAB double type
  mrs_string MatCmd = MATLABname + "=double(" + MATLABname + ");";
  engEvalString(engine_, MatCmd.c_str());

  mxDestroyArray(mxVector);
}

void
MATLABengine::putVariable(const float *const value, unsigned int size, mrs_string MATLABname)
{
  //-----------------------------------
  //send C/C++ vector to MATLAB vector
  //-----------------------------------
  mwSize dims[2];
  dims[0] = 1; //row vector
  dims[1] = size;

  mxArray *mxVector = mxCreateNumericArray(2, dims, mxSINGLE_CLASS, mxREAL);
  memcpy(mxGetData(mxVector), (void *)value, size*mxGetElementSize(mxVector));
  engPutVariable(engine_, MATLABname.c_str(), mxVector);

  //Convert to MATLAB double type
  mrs_string MatCmd = MATLABname + "=double(" + MATLABname + ");";
  engEvalString(engine_, MatCmd.c_str());

  mxDestroyArray(mxVector);
}


void
MATLABengine::putVariable(const double *const value, unsigned int size, mrs_string MATLABname)
{
  //-----------------------------------
  //send C/C++ vector to MATLAB vector
  //-----------------------------------
  mwSize dims[2];
  dims[0] = 1; //row vector
  dims[1] = size;

  mxArray *mxVector = mxCreateNumericArray(2, dims, mxDOUBLE_CLASS, mxREAL);
  memcpy(mxGetData(mxVector), (void *)value, size*mxGetElementSize(mxVector));
  engPutVariable(engine_, MATLABname.c_str(), mxVector);

  mxDestroyArray(mxVector);
}


void
MATLABengine::putVariable(mrs_natural value, mrs_string MATLABname)
{
  long lvalue = (long)value;
  putVariable(&lvalue,1, MATLABname);
}
void
MATLABengine::putVariable(mrs_real value, mrs_string MATLABname)
{
  double dvalue = (double)value;
  putVariable(&dvalue,1, MATLABname);
}

void
MATLABengine::putVariable(mrs_complex value, mrs_string MATLABname)
{
  mwSize dims[2];
  dims[0] = 1;
  dims[1] = 1;

  mxArray *mxVector = mxCreateNumericArray(2, dims, mxDOUBLE_CLASS, mxCOMPLEX);
  double *xr = mxGetPr(mxVector);
  double *xi = mxGetPi(mxVector);

  *xr = (double)value.real();
  *xi = (double)value.imag();

  engPutVariable(engine_, MATLABname.c_str(), mxVector);

  mxDestroyArray(mxVector);
}



void
MATLABengine::putVariable(realvec value, mrs_string MATLABname)
{
  //----------------------------------
  // send a realvec to a MATLAB matrix
  //----------------------------------
  mwSize dims[2]; //realvec is 2D
  dims[0] = value.getRows();
  dims[1] = value.getCols();

  //realvec are by default double precision matrices => mxDOUBLE_CLASS
  mxArray *mxMatrix = mxCreateNumericArray(2, dims, mxDOUBLE_CLASS, mxREAL);
  mrs_real *data = value.getData();
  memcpy((void *)mxGetPr(mxMatrix), (void *)(data), dims[0]*dims[1]*mxGetElementSize(mxMatrix));
  engPutVariable(engine_, MATLABname.c_str(), mxMatrix);

  mxDestroyArray(mxMatrix);
}

void
MATLABengine::putVariable(vector<mrs_natural> value, mrs_string MATLABname)
{
  mwSize dims[2];
  dims[0] = 1; //row vector
  dims[1] = (mwSize)value.size();

  mxArray *mxVector = mxCreateNumericArray(2, dims, mxDOUBLE_CLASS, mxREAL);
  double *x = mxGetPr(mxVector);

  for(unsigned int i = 0; i < value.size(); ++i)
  {
    *(x + i) = (double)value[i];
  }

  engPutVariable(engine_, MATLABname.c_str(), mxVector);

  mxDestroyArray(mxVector);
}


void
MATLABengine::putVariable(vector<mrs_real> value, mrs_string MATLABname)
{
  mwSize dims[2];
  dims[0] = 1; //row vector
  dims[1] = (mwSize)value.size();

  mxArray *mxVector = mxCreateNumericArray(2, dims, mxDOUBLE_CLASS, mxREAL);
  double *x = mxGetPr(mxVector);

  for(unsigned int i = 0; i < value.size(); ++i)
  {
    *(x + i) = (double)value[i];
  }

  engPutVariable(engine_, MATLABname.c_str(), mxVector);

  mxDestroyArray(mxVector);
}

void
MATLABengine::putVariable(vector<mrs_complex> value, mrs_string MATLABname)
{
  mwSize dims[2];
  dims[0] = 1; //row vector
  dims[1] = (mwSize)value.size();

  mxArray *mxVector = mxCreateNumericArray(2, dims, mxDOUBLE_CLASS, mxCOMPLEX);
  double *xr = mxGetPr(mxVector);
  double *xi = mxGetPi(mxVector);

  for(unsigned int i = 0; i < value.size(); ++i)
  {
    *(xr + i) = (double)value[i].real();
    *(xi + i) = (double)value[i].imag();
  }

  engPutVariable(engine_, MATLABname.c_str(), mxVector);

  mxDestroyArray(mxVector);
}


//-------------------------------------------------------------------
//					getters
//-------------------------------------------------------------------
int
MATLABengine::getVariable(std::string MATLABname, mrs_natural& value)
{
  mxArray* mxVector = engGetVariable(engine_, MATLABname.c_str());

  if (mxVector == NULL)
  {
    MRSERR("MATLABengine::getVariable(\"" << MATLABname << "\", mrs_natural&) error: failed getting MATLAB var!");
    return -1;
  }
  else {
    if (mxGetNumberOfElements(mxVector) > 1)
    {
      MRSWARN("MATLABengine::getVariable(\"" << MATLABname << "\", mrs_natural&): MATLAB array => getting first element only!");
    }

    if(mxIsComplex(mxVector))
    {
      MRSWARN("MATLABengine::getVariable(\"" << MATLABname << "\", mrs_natural&): MATLAB complex number => getting real part only!");
    }

    value = (mrs_natural)mxGetScalar(mxVector);
    mxDestroyArray(mxVector);
    return 0;
  }
}

int
MATLABengine::getVariable(std::string MATLABname, mrs_real& value)
{
  mxArray* mxVector = engGetVariable(engine_, MATLABname.c_str());

  if (mxVector == NULL)
  {
    MRSERR("MATLABengine::getVariable(\"" << MATLABname << "\", mrs_real&) error: failed getting MATLAB var!");
    return -1;
  }
  else {
    if (mxGetNumberOfElements(mxVector) > 1)
    {
      MRSWARN("MATLABengine::getVariable(\"" << MATLABname << "\", mrs_real&): MATLAB array => getting first element only!");
    }

    if(mxIsComplex(mxVector))
    {
      MRSWARN("MATLABengine::getVariable(\"" << MATLABname << "\", mrs_real&): MATLAB complex number => getting real part only!");
    }

    value = (mrs_real)mxGetScalar(mxVector);
    mxDestroyArray(mxVector);
    return 0;
  }
}

int
MATLABengine::getVariable(std::string MATLABname, mrs_complex& value)
{
  mxArray* mxVector = engGetVariable(engine_, MATLABname.c_str());

  if (mxVector == NULL)
  {
    MRSERR("MATLABengine::getVariable(\"" << MATLABname << "\", mrs_complex&) error: failed getting MATLAB var!");
    return -1;
  }
  else
  {
    if (mxGetNumberOfElements(mxVector) > 1)
    {
      MRSWARN("MATLABengine::getVariable(\"" << MATLABname << "\", mrs_real&): MATLAB array => getting first element only!");
    }

    if(mxIsComplex(mxVector))
      value = mrs_complex((mrs_real)(*mxGetPr(mxVector)),(mrs_real)(*mxGetPi(mxVector)));
    else
    {
      value = mrs_complex((mrs_real)(*mxGetPr(mxVector)),0);
      MRSWARN("MATLABengine::getVariable(\"" << MATLABname << "\", mrs_complex&): MATLAB real number => setting imaginary part to zero!");
    }

    mxDestroyArray(mxVector);
    return 0;
  }
}

int
MATLABengine::getVariable(std::string MATLABname, realvec& value)
{
  mxArray* mxVector = engGetVariable(engine_, MATLABname.c_str());

  if (mxVector == NULL)
  {
    MRSERR("MATLABengine::getVariable(\"" << MATLABname << "\", realvec&) error: variable does not exist in MATLAB!");
    return -1;
  }
  else {
    if (mxGetNumberOfDimensions(mxVector) > 2)
    {
      MRSERR("MATLABengine::getVariable(\"" << MATLABname << "\", realvecl&): MATLAB array with more than 2 dimensions!");
      return -1;
    }

    if(mxIsComplex(mxVector))
    {
      MRSWARN("MATLABengine::getVariable(\"" << MATLABname << "\", realvec&): MATLAB complex array => getting real part only!");
    }

    //number of rows and cols for the 2D MATLAB array
    //resize realvec accordingly
    value.create(mxGetDimensions(mxVector)[0],mxGetDimensions(mxVector)[1]);
    for(unsigned int i= 0; i < mxGetNumberOfElements(mxVector); ++i)
    {
      //both Marsyas realvec and MATLAB arrays are column-wise,
      //so they can be copied as linear vectors
      value(i) = (mrs_real)(*(mxGetPr(mxVector)+i));
    }

    mxDestroyArray(mxVector);
    return 0;
  }
}

int
MATLABengine::getVariable(std::string MATLABname, vector<mrs_natural>& value)
{
  mxArray* mxVector = engGetVariable(engine_, MATLABname.c_str());

  if (mxVector == NULL) {
    MRSERR("MATLABengine::getVariable(\"" << MATLABname << "\", vector<mrs_natural>&) error: Get Array Failed!");
    return -1;
  }
  else
  {
    if (mxGetNumberOfDimensions(mxVector) > 2)
    {
      MRSERR("MATLABengine::getVariable(\"" << MATLABname << "\", vector<mrs_natural>&): MATLAB array with more than 2 dimensions!");
      return -1;
    }

    if(mxIsComplex(mxVector))
    {
      MRSWARN("MATLABengine::getVariable(\"" << MATLABname << "\", vector<mrs_natural>&): MATLAB complex data => getting real part only!");
    }

    if(mxGetM(mxVector) > 1 && mxGetN(mxVector) > 1)
    {
      MRSWARN("MATLABengine::getVariable(\"" << MATLABname << "\", vector<mrs_natural>&): MATLAB array => will be got as a column-wise vector!");
    }

    value.clear();
    value.reserve(mxGetNumberOfElements(mxVector));

    for(unsigned int i= 0; i < mxGetNumberOfElements(mxVector); ++i)
    {
      //if MATLAB variable is an array, it will be got as a column-wise vector
      value.push_back((mrs_natural)(*(mxGetPr(mxVector)+i)));
    }

    mxDestroyArray(mxVector);
    return 0;
  }
}

int
MATLABengine::getVariable(std::string MATLABname, vector<mrs_real>& value)
{
  mxArray* mxVector = engGetVariable(engine_, MATLABname.c_str());

  if (mxVector == NULL) {
    MRSERR("MATLABengine::getVariable(\"" << MATLABname << "\", vector<mrs_real>&) error: Get Array Failed");
    return -1;
  }
  else
  {
    if (mxGetNumberOfDimensions(mxVector) > 2)
    {
      MRSERR("MATLABengine::getVariable(\"" << MATLABname << "\", vector<mrs_real>&): MATLAB array with more than 2 dimensions!");
      return -1;
    }

    if(mxIsComplex(mxVector))
    {
      MRSWARN("MATLABengine::getVariable(\"" << MATLABname << "\", vector<mrs_real>&): MATLAB complex data => getting real part only!");
    }

    if(mxGetM(mxVector) > 1 && mxGetN(mxVector) > 1)
    {
      MRSWARN("MATLABengine::getVariable(\"" << MATLABname << "\", vector<mrs_real>&): MATLAB array => will be got as a column-wise vector!");
    }

    value.clear();
    value.reserve(mxGetNumberOfElements(mxVector));

    for(unsigned int i= 0; i < mxGetNumberOfElements(mxVector); ++i)
    {
      //if MATLAB variable is an array, it will be got as a column-wise vector
      value.push_back((mrs_real)(*(mxGetPr(mxVector)+i)));
    }

    mxDestroyArray(mxVector);
    return 0;
  }
}

int
MATLABengine::getVariable(std::string MATLABname, vector<mrs_complex>& value)
{
  mxArray* mxVector = engGetVariable(engine_, MATLABname.c_str());

  if (mxVector == NULL) {
    MRSERR("MATLABengine::getVariable(\"" << MATLABname << "\", vector<mrs_complex>&) error: Get Array Failed");
    return -1;
  }
  else
  {
    if (mxGetNumberOfDimensions(mxVector) > 2)
    {
      MRSERR("MATLABengine::getVariable(\"" << MATLABname << "\", vector<mrs_complex>&): MATLAB array with more than 2 dimensions!");
      return -1;
    }

    if(mxGetM(mxVector) > 1 && mxGetN(mxVector) > 1)
    {
      MRSWARN("MATLABengine::getVariable(\"" << MATLABname << "\", vector<mrs_complex>&): MATLAB array => will be got as a column-wise vector!");
    }

    value.clear();
    value.reserve(mxGetNumberOfElements(mxVector));

    for(unsigned int i= 0; i < mxGetNumberOfElements(mxVector); ++i)
    {
      //if MATLAB variable is an array, it will be got as a column-wise vector
      if(mxIsComplex(mxVector))
        value.push_back( mrs_complex( (mrs_real)(*(mxGetPr(mxVector)+i)), (mrs_real)(*(mxGetPi(mxVector)+i)) ));
      else
      {
        value.push_back(mrs_complex( (mrs_real)(*(mxGetPr(mxVector)+i)), 0 ));
        MRSWARN("MATLABengine::getVariable(\"" << MATLABname << "\", vector<mrs_complex>&): MATLAB real numbers => setting imaginary parts to zero!");
      }
    }

    mxDestroyArray(mxVector);
    return 0;
  }
}


#endif //_MATLAB_ENGINE
