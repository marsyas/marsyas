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

#ifndef MARSYAS_CONFIDENCE_H
#define MARSYAS_CONFIDENCE_H

#include <marsyas/system/MarSystem.h>

#include <fstream>
#include <iostream>
#include <iomanip>

//using std::ostringstream;

namespace Marsyas
{
/**
	\ingroup MachineLearning
	\brief Classification confidence by majority voting.
*/


class Confidence: public MarSystem
{
private:
  void addControls();
  void myUpdate(MarControlPtr sender);

  realvec confidences_;
  realvec gtconfidences_;

  mrs_natural num_labels_;
  mrs_natural count_;
  std::vector<std::string> labelNames_;
  bool print_;
  bool forcePrint_;
  bool csvOutput_;
  mrs_natural write_;
  mrs_natural nbFrames_;
  mrs_natural nbCorrectFrames_;

  mrs_real hopDuration_;
  std::ofstream outputFileSyn_;
  std::ofstream outputFileTran_;
  std::string lastLabel_;
  std::string oriName_;

  mrs_natural predictions_;

  MarControlPtr ctrl_memSize_;
  MarControlPtr ctrl_nLabels_;


public:
  Confidence(std::string name);
  Confidence(const Confidence& a);

  ~Confidence();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif


