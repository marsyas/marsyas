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

#ifndef MARSYAS_PVMULTIRESOLUTION_H
#define MARSYAS_PVMULTIRESOLUTION_H

#include "MarSystem.h"

namespace Marsyas
{
/**
	\class PvMultiResolution
	\ingroup Synthesis 
	\brief Switches between short and long windows in multiresolution phasevocoder


	Controls:
	- \b mrs_string/mode [w] : sets the mode (long or short)
*/

class PvMultiResolution: public MarSystem
{
private:
	void addControls();
	void myUpdate(MarControlPtr sender);

		MarControlPtr ctrl_mode_;
		mrs_realvec median_buffer_;
		mrs_natural mbindex_;
		
		
public:
	PvMultiResolution(std::string name);
	PvMultiResolution(const PvMultiResolution& a);
	~PvMultiResolution();
	MarSystem* clone() const;

	void myProcess(realvec& in, realvec& out);
};

}

#endif

