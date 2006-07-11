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
\class MATLABengine
\brief Utility class for exchanging data between Marsyas and MATLAB

In order to use this class in Marsyas, _MARSYAS_ENGINE_ must be defined.
Check out the test and demonstration code at the marsyasTests.cpp.
More info on how to build with MATLAB engine support at: 

http://www.mathworks.com/access/helpdesk/help/techdoc/matlab_external/f39903.html

Code by Luís Gustavo Martins <lmartins@inescporto.pt>
*/

#ifdef _MATLAB_ENGINE_ 

#if !defined(__MATLABENGINE_H__)
#define __MATLABENGINE_H__

#include "engine.h" //$(MATLAB)\extern\include

#include <string>
#include <vector>

#include "common.h"

#define MATLAB MATLABengine::getMatlabEng()

namespace Marsyas
{

class realvec;
  
class MATLABengine
{
private:
  Engine *engine_;
  static MATLABengine *instance_;
  
  MATLABengine();
  
public:
  ~MATLABengine();
  
  static MATLABengine *getMatlabEng();
  
  void evalString(std::string MATLABcmd);
  
  //------------------------------------------------
  //					setters
  //------------------------------------------------
  void putVariable(const long *const value, unsigned int size, std::string MATLABname);	
  void putVariable(const float *const value, unsigned int size, std::string MATLABname);
  void putVariable(const double *const value, unsigned int size, std::string MATLABname);
  
  void putVariable(mrs_natural value, std::string MATLABname);//OK
  void putVariable(mrs_real value, std::string MATLABname);//OK
  void putVariable(mrs_complex value, std::string MATLABname);//OK
  
  void putVariable(realvec value, std::string MATLABname);//OK
  
  void putVariable(std::vector<mrs_natural> value, std::string MATLABname);
  void putVariable(std::vector<mrs_real> value, std::string MATLABname);
  void putVariable(std::vector<mrs_complex> value, std::string MATLABname);
  
  //------------------------------------------------
  //					getters
  //------------------------------------------------
  int getVariable(std::string MATLABname, mrs_natural& value);//OK
  int getVariable(std::string MATLABname, mrs_real& value);//OK
  int getVariable(std::string MATLABname, mrs_complex& value);//OK
  
  int getVariable(std::string MATLABname, realvec& value);//OK
  
  int getVariable(std::string MATLABname, std::vector<mrs_natural> &value);
  int getVariable(std::string MATLABname, std::vector<mrs_real> &value);
  int getVariable(std::string MATLABname, std::vector<mrs_complex> &value);
};
  
} //namespace Marsyas

#endif //__MATLABENGINE_H__

#endif //_MATLAB_ENGINE


