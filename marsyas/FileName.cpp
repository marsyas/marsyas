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


/** 
    \class FileName
    \brief Various utilities

    FileName is a class bundling together various rather 
unrelated convenience utilities related to filenames. 
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



char* 
FileName::rindex(const char *s, int c) {
  int len = strlen(s);
  int i;
  for (i=len-1; i >=0; i--) {
    if (s[i] == c)
      break;
  }
  return (char *)((i < 0) ? NULL : (s+i));
}


string 
FileName::fullname()
{
  return filename_;
}


string 
FileName::name()
{
  const char *str = filename_.c_str();
  const char *tmp = rindex(str, '/');
  if (tmp==NULL) 
    return filename_;
  else
    {
      string res(tmp+1);
      return res;
    }
  
}


string
FileName::ext()
{
  const char *str = filename_.c_str();
  const char *tmp = rindex(str, '.');
  if (tmp==NULL) 
    return filename_;
  else
    {
      string res(tmp+1);
      return res;
    }
}

string
FileName::path()
{
  string res("path");
  return res;
}
	
      
  
