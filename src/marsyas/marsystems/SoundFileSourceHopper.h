/*
** Copyright (C) 2009 Stefaan Lippens
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

#ifndef MARSYAS_SOUNDFILESOURCEHOPPER_H
#define MARSYAS_SOUNDFILESOURCEHOPPER_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\class SoundFileSourceHopper
	\ingroup IO
	\brief Generates slices from a sound file source in a sliding window
	fashion with hopping.

	The SoundFileSourceHopper provides the basic IO functionality in audio
	processing of producing slices of audio data in a sliding window fashion
	with a given window size and hop size.

	Traditionally, a sliding window with hopping had to be
	implemented in Marsyas by a normal SoundFileSource Marsystem
	(without hopping), followed by a ShiftInput Marsystem (to
	provide the hopping).
	This was not very intuitive, e.g. there was no explicit
	"hopSize" control.
	Moreover, the hop size had to be set through the window size
	of the top level MarSystem, which was unobvious, lead too
	easily to bugs with wrongly overwritten hop sizes and made
	it near to impossible to provide it as a reusable component.
	The SoundFileSourceHopper is designed to be a reusable
	sliding/hopping windowing component for without the issues
	mentioned above.

	The implementation of SoundFileSourceHopper is largely based
	on Series, especially SoundFileSourceHopper::myUpdate() and
	SoundFileSourceHopper::myProcess(). The main difference is
	that SoundFileSourceHopper is prefilled with a SoundFileSource,
	ShiftInput and MixToMono MarSystem and
	SoundFileSourceHopper::myUpdate() sets the inSamples control
	of the internal SoundFileSource based on the desired hop size
	instead of the inSamples control of the container.

	\see SoundFileSource, ShiftInput, MixToMono

	Controls:
	- \b mrs_string/filename: the file name of the file to load
	- \b mrs_natural/windowSize: the window size (in samples)
	- \b mrs_natural/hopSize: the hop size (in samples)
	- \b mrs_bool/mixToMono [rw] : mix the audio down to mono.
	- \b mrs_bool/hasData [r]: true as long as there is data to read.

	Additional SoundFileSource settings can be configured through
	the internal SoundFileSource "src", for example with
	a control string like "SoundFileSource/src/mrs_string/currentlyPlaying".

	@todo: provide reset functionality to reset the Shiftinput
	memory and also takes reseting the SoundFileSource into account.
*/

class SoundFileSourceHopper: public MarSystem
{
private:

  /// Add specific controls needed by this MarSystem.
  void addControls();

  /// Reads changed controls and sets up variables if necessary.
  void myUpdate(MarControlPtr sender);

  /// MarControlPtr for the window size control.
  MarControlPtr ctrl_windowSize_;

  /// MarControlPtr for the hop size.
  MarControlPtr ctrl_hopSize_;

  /// MarControlPtr for the gain control.
  MarControlPtr ctrl_mixToMono_;

public:
  /// SoundFileSourceHopper constructor.
  SoundFileSourceHopper(std::string name);

  /// SoundFileSourceHopper copy constructor.
  SoundFileSourceHopper(const SoundFileSourceHopper& a);

  /// SoundFileSourceHopper destructor.
  ~SoundFileSourceHopper();

  /// Implementation of the MarSystem::clone() method.
  MarSystem* clone() const;

  /// Implementation of the MarSystem::myProcess method.
  void myProcess(realvec& in, realvec& out);
};

}
//namespace Marsyas

#endif
//MARSYAS_SOUNDFILESOURCEHOPPER_H
