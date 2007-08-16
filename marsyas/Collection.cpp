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


#include "Collection.h"

using namespace std;
using namespace Marsyas;

Collection::Collection()
{
  collectionList_.reserve(1000);
}

Collection::~Collection()
{
}

void 
Collection::setName(string name) 
{
  name_ = name;
}


void 
Collection::read(string filename)
{
  ifstream is(filename.c_str());
  name_ = filename.substr(0, filename.rfind(".", filename.length()));
  
  is >> (*this);
}


void 
Collection::write(string filename)
{
  ofstream os(filename.c_str());
  os << (*this) << endl;
}


ostream& 
Marsyas::operator<<(ostream& o, const Collection& l)
{
  // o << "# MARSYAS Collection " << endl;
  // o << "# name = " << l.name_ << endl << endl;
  for (unsigned int i=0; i < l.collectionList_.size(); i++)
    {
      o << l.collectionList_[i] << endl;
    }
  o << endl;
  return o;
}



mrs_natural
Collection::size()
{
  return (mrs_natural)collectionList_.size();
}


string
Collection::name()
{
  return name_;
}

void 
Collection::add(string entry)
{
  collectionList_.push_back(entry);
}

mrs_natural 
Collection::getSize() 
{
  mrs_natural size = collectionList_.size();
  return size;
}


void 
Collection::shuffle()
{
  unsigned int i;
  unsigned int size = collectionList_.size();
  unsigned int rind;
  string temp;
  
  for (i=0;  i < size; i++)
    {
      rind = (unsigned int)(((mrs_real)rand() / (mrs_real)(RAND_MAX))*size);
      temp = collectionList_[i];
      collectionList_[i] = collectionList_[rind];
      collectionList_[rind] = temp;
    }
}


string 
Collection::toLongString()
{
  string result;
  
  vector<string>::iterator sfi;
  for (sfi = collectionList_.begin(); sfi != collectionList_.end(); ++sfi)
    {
      string sfname = *sfi;
      result += sfname +',';
    }
  return result;
}


string 
Collection::labelEntry(unsigned int i) 
{
  return labelList_[i]; 
}

string 
Collection::entry(unsigned int i)
{
  return collectionList_[i];
}



istream& 
Marsyas::operator>>(istream& i, Collection& l)
{
  
  MRSDIAG("Collection.cpp - operator>>");
  string str0, str1, str2;
  string fileEntry;
  string fname;
  
  while (getline(i,fileEntry))
    {
      if (fileEntry.empty())
	{ 
	  ; // do nothing
	}
      else 
	{
	  char c = fileEntry[0];
	  istringstream iss(fileEntry);
	  iss >> fname;
	  string::size_type loc = fileEntry.find( "\t", 0 );
	  if (loc != string::npos) 
	    {
	      l.collectionList_.push_back(fileEntry.substr(0, loc));
	      l.labelList_.push_back(fileEntry.substr(loc+1, fileEntry.size()));
	      cout << "Found tab" << endl;
	    } 
	  else 
	    {
	      if (c != '#') 
		{
		  l.collectionList_.push_back(fileEntry);
		}
	    }
	}
    }
  
  
  return i;
}

