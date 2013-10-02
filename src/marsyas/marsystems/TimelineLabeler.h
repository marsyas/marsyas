/*
** Copyright (C) 1998-2012 George Tzanetakis <gtzan@cs.uvic.ca>
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

#ifndef MARSYAS_TIMELINELABELER_H
#define MARSYAS_TIMELINELABELER_H

#include <marsyas/system/MarSystem.h>
#include <marsyas/TimeLine.h>

namespace Marsyas
{
/**
	\class TimelineLabeler
	\ingroup Annotator

	\brief A MarSystem that loads a TimeLine object with label info
	corresponding to an audio file, where at each timeline region a
	class or label is assigned. Can be used for training a classifier,
	using the different labeled regions of the signal as ground truth.

	This MarSystem is intended to be used in series right after a
	SoundFileSource (or other sound source MarSystem - e.g. a
	CollectionFileSource), linking its labelFiles and currentLabelFile
	controls to the SoundFileSource labelNames and currentLabel
	controls, respectively. It then exposes the same controls that
	SoundFileSource would expose if not using timelines at all, so
	that the label, labelNames and nLabels controls of an Annotator or
	an WekaSink MarSystem can be linked to this MarSystem similarly to
	the way they are connected to a SoundFileSource MarSystem.
	TimelineLabaler assumes that the regions in the timeline are
	non-overlapping and ordered by their start sample.

	There is an implicit assumptions that each time-line file contains
	examples from all the desired classes. This can easily be added
	by adding a few regions with short duration and the "missing" labels
	if a particular timeline does not have regions for all the labels.
	Alternatively it is possible to use an externally provided lexicon with the
	desired labelNames and nLabels.

	Controls:

	- \b mrs_string/labelFiles [w] : list of label files (Marsyas
	timeline files - .mtl) to be opened, depending on the
	currentLabelFile control (see below). It should have a format
	like, for e.g.: "audio1.tml,audio2.tml,audio3.tml,", which is what
	is obtained when using a CollectionFileSource to read a audio1.wav
	TAB audio1.tml collection file (just link this control to its
	mrs_string/labelNames control).

	- \b mrs_real/currentLabelFile [w] : selects which one of the
	labelFiles (see above) should be selected. Usually this control is
	to be linked to the currentLabel control of a SoundFileSource.

	- \b mrs_string/labelNames [r] : list of classes in the currently
      loaded timeline, in the format "label1,label2,label3,".

	- \b mrs_natural/nLabels [r] : number of distinct labels in the
	labelNames (see above) controls (for the example given above, it
	would have a value of 3).

	- \b mrs_bool/useLexicon [w] : use the lexicon overriding

	- \b mrs_string/lexiconLabelNames [w]: list of classes to be used
      in the format "label1,label2,label3,"

	- \b mrs_natural/lexiconNLabels [w]: the number of distinct labels
      in the lexicon labelNames

	- \b mrs_real/currentLabel [r] : indicates the label of the
	current audio stream position (as a numeric index to the label in
	labelNames control - see above).

*/

class TimelineLabeler: public MarSystem
{
private:
  MarControlPtr ctrl_labelFiles_;
  MarControlPtr ctrl_currentLabelFile_;
  MarControlPtr ctrl_selectLabel_;
  MarControlPtr ctrl_useLexicon_;


  MarControlPtr ctrl_labelNames_;
  MarControlPtr ctrl_lexiconLabelNames_;

  MarControlPtr ctrl_currentLabel_;
  MarControlPtr ctrl_previousLabel_;
  MarControlPtr ctrl_nLabels_;
  MarControlPtr ctrl_lexiconNLabels_;


  MarControlPtr ctrl_playRegionsOnly_;
  MarControlPtr ctrl_pos_;
  MarControlPtr ctrl_advance_;

  mrs_string labelFiles_;
  std::vector<mrs_string> labelFilesVec_;

  mrs_string selectedLabel_;

  TimeLine timeline_;
  mrs_natural numClasses_;
  mrs_natural curRegion_;
  mrs_bool foundNextRegion_;
  mrs_bool myAdvance_; // ctrl_advance gets overwritten by CollectionFileSoruce

  void addControls();
  void myUpdate(MarControlPtr sender);
  mrs_bool noLabelFile_;

  mrs_bool load_next_region_file();

public:
  TimelineLabeler(std::string name);
  TimelineLabeler(const TimelineLabeler& a);
  ~TimelineLabeler();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}

#endif

