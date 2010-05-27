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

#include "common.h"
#include "Collection.h"
#include <algorithm>
#include <iterator>


using namespace std;



namespace Marsyas 
{

// Utility function. Should move this somewhere publicly accessible for code re-use.
string join(const vector<string>& v, const string delim)
{    
    ostringstream os;
    copy(v.begin(), v.end(), ostream_iterator<string>(os, delim.c_str()));
	
    return os.str();
}

Collection::Collection()
{
    collectionList_.reserve(1024);
    hasLabels_ = false;
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

void 
Collection::labelAll(string label) 
{
    if (hasLabels_ == false) 
    {
        hasLabels_ = true;
        labelList_.reserve(collectionList_.size());
        for (unsigned int i = 0; i < collectionList_.size(); ++i) 
            labelList_.push_back(label);
    }
    else 
    {
        for (unsigned int i=0; i < collectionList_.size(); ++i) 
            labelList_[i] = label;
    }
}

ostream& 
operator<<(ostream& o, const Collection& l)
{
    // o << "# MARSYAS Collection " << endl;
    // o << "# name = " << l.name_ << endl << endl;
    for (unsigned int i=0; i < l.collectionList_.size(); ++i)
    {
        o << l.collectionList_[i];
        if (l.hasLabels_) 
            o << "\t" << l.labelList_[i];
        o << endl;
    }
    o << endl;
    return o;
}



size_t
Collection::size()
{
    return collectionList_.size();
}
/// TODO choose one... 
size_t 
Collection::getSize() 
{
	return collectionList_.size();
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



void 
Collection::clear()
{
	collectionList_.clear();
	labelList_.clear();
	
	// Do not clear labelNames so that multiple collections 
	// can share the same label set 
	// maybe at some point make this behavior controllable 
}
	

void 
Collection::add(string entry, string label)
{

	
    collectionList_.push_back(entry);
    hasLabels_ = true;
    labelList_.push_back(label);
	
    if (find(labelNames_.begin(), labelNames_.end(), label) == labelNames_.end())
        labelNames_.push_back(label);

	
	sort(labelNames_.begin(), labelNames_.end());

}




mrs_natural 
Collection::getNumLabels() 
{
    return labelNames_.size();
}

mrs_string 
Collection::labelName(mrs_natural i) 
{
    if ((unsigned)i < labelNames_.size())
        return labelNames_[i];

    return EMPTYSTRING;
}

mrs_string 
Collection::getLabelNames() 
{
    return join(labelNames_, ",");
}

mrs_bool 
Collection::hasLabels() 
{
    return hasLabels_; 
}

void 
Collection::shuffle()
{
    // Use a Fisher-Yates shuffle
    // http://en.wikipedia.org/wiki/Fisher-Yates_shuffle
    unsigned int n = collectionList_.size();
    while (n > 1)
    {
        // Generate a random index in the range [0, n).
        int k = (unsigned int)(n * ((mrs_real)rand() / ((mrs_real)(RAND_MAX) + (mrs_real)1)));

        n--;
        swap(collectionList_[n], collectionList_[k]);
        if (hasLabels_)
            swap(labelList_[n], labelList_[k]);
    }
}

string 
Collection::toLongString()
{
    return join(collectionList_, ",");
}

mrs_natural 
Collection::labelNum(mrs_string label) 
{
	
    vector<string>::iterator it = find(labelNames_.begin(), labelNames_.end(), label);
    if (it == labelNames_.end())
        return -1; 
	
	mrs_natural l = distance(labelNames_.begin(), it);
    return l ;
	
}

string 
Collection::labelEntry(unsigned int i) 
{
    if (hasLabels_)
        return labelList_[i]; 
    return "No label"; 
}

string 
Collection::entry(unsigned int i)
{
    return collectionList_[i];
}


void 
Collection::concatenate(vector<Collection> cls) 
{
    for (size_t cj = 0; cj < cls.size(); cj++)
    {
        Collection l = cls[cj];
        if (l.hasLabels_)
            hasLabels_ = true;

        for (size_t i = 0; i < l.size(); ++i) 
            add(l.entry(i), l.labelEntry(i));
    }
}



istream& 
operator>>(istream& i, Collection& l)
{
    MRSDIAG("Collection.cpp - operator>>");

    string fileEntry;
    while (getline(i, fileEntry))
    {
        // Skip blank lines.
        if (fileEntry.empty())
            continue;

        // Skip comment lines.
        if (fileEntry[0] == '#')
            continue;

        // Check to see if there is a label. Could use rfind for efficiency
        // if we were sure there weren't tabs after the label.
        string::size_type loc = fileEntry.find('\t', 0);
        if (loc != string::npos) 
        {
            string file = fileEntry.substr(0, loc);
            string label = fileEntry.substr(loc+1, fileEntry.size());
            l.add(file, label);
        } 
        else 
            l.add(fileEntry);
    }
   
    return i;
}
}
