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



#include "common_source.h" 

#ifdef MARSYAS_WIN32
#include <io.h>
#endif

#include "FileName.h"

using std::ostringstream;
using std::max;

using namespace Marsyas;

FileName::FileName()
{
}

FileName::FileName(mrs_string filename)
{
  filename_ = filename;



#if (defined(MARSYAS_WIN32))
  if (isDir	())
	  removeLastSlash ();
#endif 
}


FileName::~FileName()
{
}





mrs_string 
FileName::fullname()
{
  return filename_;
}


mrs_string 
FileName::name()
{
  mrs_string name;
  size_t loc = getLastSlashPos ();
  
  if (loc != mrs_string::npos)
    name = filename_.substr(loc+1, filename_.length()-1);
  else 
    name = filename_;			// file in current directory 
  
  return name;
  
}

mrs_string
FileName::nameNoExt()
{
  mrs_string str = name();
  size_t loc;  

  loc = str.rfind(".", str.length()-1);
  return str.substr(0,loc);
}

mrs_string
FileName::ext()
{
  size_t loc;
  loc = filename_.rfind(".", filename_.length()-1);
  return filename_.substr(loc+1, filename_.length()-1);
}

mrs_string
FileName::path()
{

  mrs_string name;
  size_t loc = getLastSlashPos ();

  
  if (loc != mrs_string::npos)
    name = filename_.substr(0, loc+1);
  else 
    name = "";			// file in current directory no path
  
  return name;
  
}

mrs_bool
FileName::isDir ()
{

#if (defined(MARSYAS_WIN32))
	const DWORD attr = GetFileAttributes (filename_.c_str ());

	return (attr != 0xffffffff)
		&& ((attr & FILE_ATTRIBUTE_DIRECTORY) != 0);
#else
	MRSWARN("isDir only implemented on Windows");
	return false;	
#endif
	

	
	
}

std::vector<mrs_string> 
FileName::getFilesInDir (mrs_string wildcard)
{
	std::vector<mrs_string>		result;

#ifdef MARSYAS_WIN32
	struct _finddata_t          CurrentFile;
	long                        hFile;
	mrs_string                  search4;

	search4	= filename_ + "/" + wildcard;

	// find first file 
	if( (hFile = _findfirst( search4.c_str (), &CurrentFile )) == -1L )
		return result;
	else
	{
		// file found, add it to the list
		result.push_back (filename_ + "/" + CurrentFile.name);

		// Find the rest of the files
		while( _findnext( hFile, &CurrentFile ) == 0 )
		{
			// file found, add it to the list
			result.push_back (filename_ + "/" + CurrentFile.name);
		}

		// has to be called at the end
		_findclose( hFile );
	}
#else
    (void) wildcard;
	MRSWARN("getFilesInDir only works on Windows");
#endif
	return result;
}
size_t
FileName::getLastSlashPos ()
{
	size_t loc;

#ifdef MARSYAS_WIN32
	size_t loc2  = filename_.rfind("/", filename_.length()-1);  // you can use the slash in windows, too
	loc = filename_.rfind("\\", filename_.length()-1);
	if (loc2 != mrs_string::npos)
		loc = max (loc, loc2);
#else 
	loc = filename_.rfind("/", filename_.length()-1);
#endif
	return loc;
}

void
FileName::removeLastSlash ()
{
	size_t loc	= getLastSlashPos ();

	if (loc == filename_.length()-1)
		filename_	= filename_.substr(0, loc);
}
      
  
