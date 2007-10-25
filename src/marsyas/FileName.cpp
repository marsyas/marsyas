 /*
 ** Copyright (C) 1998-2005 George Tzanetakis <gtzan@cs.uvic.ca>
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


#include "FileName.h"

using namespace std;
using namespace Marsyas;

FileName::FileName()
{
}

FileName::FileName(string filename)
{
  filename_ = filename;
}


FileName::~FileName()
{
}





string 
FileName::fullname()
{
  return filename_;
}


string 
FileName::name()
{
  string name;
  size_t loc;

#ifdef MARSYAS_WIN32
  loc = filename_.rfind("\\", filename_.lenth()-1);
#else 
  loc = filename_.rfind("/", filename_.length()-1);
#endif
  
  if (loc != string::npos)
    name = filename_.substr(loc+1, filename_.length()-1);
  else 
    name = filename_;			// file in current directory 
  
  return name;
  
}

string
FileName::nameNoExt()
{
  string str = name();
  size_t loc;  

  loc = str.rfind(".", str.length()-1);
  return str.substr(0,loc);
}

string
FileName::ext()
{
  size_t loc;
  loc = filename_.rfind(".", filename_.length()-1);
  return filename_.substr(loc+1, filename_.length()-1);
}

string
FileName::path()
{

  string name;
  size_t loc;

#ifdef MARSYAS_WIN32
  loc = filename_.rfind("\\", filename_.lenth()-1);
#else 
  loc = filename_.rfind("/", filename_.length()-1);
#endif
  
  if (loc != string::npos)
    name = filename_.substr(0, loc+1);
  else 
    name = "";			// file in current directory no path
  
  return name;
  
}

	
      
  
