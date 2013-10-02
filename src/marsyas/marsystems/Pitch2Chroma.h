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

#ifndef MARSYAS_PITCH2CHROMA_H
#define MARSYAS_PITCH2CHROMA_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
\ingroup	Analysis
\brief		Transform pitch to chroma
			- input = amplitude spectrum with peaks at frequency bins
			that represent pitches and values that represent evidences
			- output = chroma profile with chroma values ordered
			according to the circle of fifths, i.e. A, E,.. D
\author		Matthias Varewyck
\date		20090518

Controls:
- \b mrs_real/SampleRate [rw] : sample rate of the input spectrum
- \b mrs_real/LowestPitch [rw] : lowest pitch taken into account
- \b mrs_natural/NotesPerOctave [rw] : nr. of notes per octave ([f 2*f])
- \b mrs_natural/NrOfNotes [rw] : total nr. of notes to be taken into account
- \b mrs_natural/RefChromaIndex [rw] : index in chroma vector of the ref. pitch (= 440Hz)
*/

class Pitch2Chroma: public MarSystem
{
public:
  Pitch2Chroma(mrs_string inName);
  Pitch2Chroma(const Pitch2Chroma& inToCopy);

  ~Pitch2Chroma();

  MarSystem* clone() const;

  void addControls();
  void myUpdate(MarControlPtr inSender);
  void myProcess(realvec& inVec, realvec& outVec);

private:
  // Pointer to MarControllers
  MarControlPtr ctrl_SampleRate_;
  MarControlPtr ctrl_LowestPitch_;
  MarControlPtr ctrl_NotesPerOctave_;
  MarControlPtr ctrl_NrOfNotes_;
  MarControlPtr ctrl_RefChromaIndex_;

  // Member variables
  mrs_real SampleRate_;					// Sample rate of amplitude spectrum
  mrs_real LowestPitch_;					// Lowest supported pitch
  mrs_natural NotesPerOctave_;			// Number of notes per octave
  mrs_natural NrOfNotes_;					// Total number of notes
  mrs_natural RefChromaIndex_;			// Chroma index of LOWEST FREQUENCY
  // Possible values: 1.. NotesPerOctave

  mrs_realvec PitchToNoteTransform_;		// Dimension: NrOfNotes x NrOfFFTBins
  mrs_realvec StartAndEndIndex_;			// Start/end indexes of notes in input vector

  mrs_realvec NoteToChromaTransform_;		// Dimension: NrOfNotesPerOctave x NrOfNotes

  void UpdatePitchToNoteTransform();
  void UpdateNoteToChromaTransform();
};

}	// End namespace

#endif
