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

#ifndef MARSYAS_COLLECTION_H
#define MARSYAS_COLLECTION_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <marsyas/common_header.h>

namespace Marsyas
{
/**
	\class Collection
	\ingroup NotmarCore
	\brief List of files

List of files used for batch processing.
*/

class marsyas_EXPORT Collection
{
private:
  std::vector<std::string> collectionList_;
  std::vector<std::string> labelList_;
  std::vector<std::string> labelNames_;
  std::string name_;
  bool hasLabels_;
  bool store_labels_;

public:
  Collection();
  ~Collection();
  mrs_natural size();
  void setName(std::string name);
  std::string entry(mrs_natural i);
  std::string labelEntry(mrs_natural i);
  mrs_natural getSize();
  mrs_natural getNumLabels();
  mrs_string getLabelNames();
  mrs_natural labelNum(mrs_string label);
  mrs_string labelName(mrs_natural i);
  mrs_bool hasLabels();
  void add(std::string entry);
  void add(std::string entry, std::string label);
  std::string name();
  void shuffle();
  void read(std::string filename);
  void write(std::string filename);
  void labelAll(std::string label);
  std::string toLongString();
  void concatenate(std::vector<Collection> cls);
  void clear();
  void store_labels(mrs_bool store);
  mrs_real regression_label(mrs_natural cindex);

  marsyas_EXPORT friend std::ostream& operator<<(std::ostream&, const Collection&);
  marsyas_EXPORT friend std::istream& operator>>(std::istream&, Collection&);
};

}//namespace Marsyas

#endif
