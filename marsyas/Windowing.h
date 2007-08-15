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

#ifndef MARSYAS_WINDOWING_H
#define MARSYAS_WINDOWING_H

#include "MarSystem.h"	

namespace Marsyas
{
/** 
\class Windowing
\ingroup Processing
\brief window the input signal

*/


	class Windowing: public MarSystem
	{
	private: 
		realvec envelope_;
		realvec tmp_;
		//mrs_real norm_;
		mrs_natural delta_;

		mrs_natural zeroPadding_;
		mrs_natural size_;

		MarControlPtr ctrl_type_;
		MarControlPtr ctrl_zeroPhasing_;
		MarControlPtr ctrl_zeroPadding_;
		MarControlPtr ctrl_size_;
		MarControlPtr ctrl_variance_;

		void addcontrols();
		void myUpdate(MarControlPtr sender);

	public:
		Windowing(std::string name);
		Windowing(const Windowing& a);
		~Windowing();

		MarSystem* clone() const;

		void myProcess(realvec& in, realvec& out);

	};

}//namespace Marsyas

#endif
