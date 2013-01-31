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

#include "common_source.h"
#include "Collection.h"
#include <algorithm>
#include <iterator>
#include <time.h>  // for srand(time(NULL))


 
using std::ostringstream;
using std::vector;
using std::ifstream;
using std::ofstream;
using std::ostream_iterator;
using std::endl;
using std::ostream;
using std::istream;



// if the directory doesn't exist, we need to make it a "".
static std::string marsyas_datadir_ =
  std::getenv("MARSYAS_DATADIR") == NULL ?
    "" : std::getenv("MARSYAS_DATADIR");

namespace Marsyas 
{

// Utility function. Should move this somewhere publicly accessible for code re-use.
mrs_string join(const vector<mrs_string>& v, const mrs_string delim)
{    
    ostringstream os;
    copy(v.begin(), v.end(), ostream_iterator<mrs_string>(os, delim.c_str()));
	
    return os.str();
}

Collection::Collection()
{
    collectionList_.reserve(1024);
    hasLabels_ = false;
    store_labels_ = true;
    // initialize random number generation.
    srand( (unsigned int) time( NULL) );
}

Collection::~Collection()
{
}

void 
Collection::setName(mrs_string name) 
{
    name_ = name;
}

void 
Collection::store_labels(mrs_bool store) 
{
    store_labels_ = store;
}


void 
Collection::read(mrs_string filename)
{
    ifstream is(filename.c_str());
    name_ = filename.substr(0, filename.rfind(".", filename.length()));

    is >> (*this);
}


void 
Collection::write(mrs_string filename)
{
    ofstream os(filename.c_str());
    os << (*this) << endl;
}

void 
Collection::labelAll(mrs_string label) 
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
    //o << endl;
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


mrs_string
Collection::name()
{
    return name_;
}

void 
Collection::add(mrs_string entry)
{
    collectionList_.push_back(entry);
	hasLabels_ = false;
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
Collection::add(mrs_string entry, mrs_string label)
{

    collectionList_.push_back(entry);
    hasLabels_ = true;
    labelList_.push_back(label);

    if (store_labels_) {
        if (find(labelNames_.begin(), labelNames_.end(), label) == labelNames_.end()) {
            labelNames_.push_back(label);
        }
	sort(labelNames_.begin(), labelNames_.end());
    }

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

mrs_string 
Collection::toLongString()
{
    return join(collectionList_, ",");
}

mrs_natural 
Collection::labelNum(mrs_string label) 
{
	
    vector<mrs_string>::iterator it = find(labelNames_.begin(), labelNames_.end(), label);
    if (it == labelNames_.end())
        return -1; 
	
	mrs_natural l = distance(labelNames_.begin(), it);
    return l ;
	
}
mrs_real
Collection::regression_label(mrs_natural cindex)
{
	if (hasLabels_) {
		return (mrs_real) atof(labelList_[cindex].c_str());
	}
	return 0.0;
}

mrs_string 
Collection::labelEntry(unsigned int i) 
{
    if (hasLabels_) {
        if (i < labelList_.size()){
            return labelList_[i]; 
        }
    }
    return "No label"; 
}

mrs_string 
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


/* I can't be bothered to think about this myself, so copied from
 http://stackoverflow.com/questions/3418231/c-replace-part-of-a-string-with-another-string
 -gp */
bool replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
        return true;
}


istream& 
operator>>(istream& i, Collection& l)
{
    MRSDIAG("Collection.cpp - operator>>");

    mrs_string fileEntry;
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
        if (marsyas_datadir_.length() > 0) {
            replace(fileEntry, "MARSYAS_DATADIR", marsyas_datadir_);
        }
        mrs_string::size_type loc = fileEntry.find('\t', 0);
        if (loc != mrs_string::npos) 
        {
            mrs_string file = fileEntry.substr(0, loc);
            mrs_string label = fileEntry.substr(loc+1, fileEntry.size());
            l.add(file, label);
        } 
        else {
            l.add(fileEntry);
        }
    }
   
    return i;
}
}
