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

/** 
\class MarSystemTemplateBasic
\brief Basic example on how to use controls efficiently in MarSystems

*/

#ifndef MARSYAS_MARSYSTEMTEMPLATEBASIC_H
#define MARSYAS_MARSYSTEMTEMPLATEBASIC_H

#include "MarSystem.h"	

namespace Marsyas
{

	class MarSystemTemplateBasic: public MarSystem
	{
	private:
		//Add specific controls needed by this MarSystem.
		void addControls();

		//"Pointers" to controls allow efficient access to their values.
		//(for clarity sake, we use the ctrl_ prefix so these "pointers"
		//can be easly identified through out the code.. but this is not
		//mandatory, just recommended)
		MarControlPtr ctrl_gain_;
		MarControlPtr ctrl_repeats_;

		void myUpdate();

	public:
		MarSystemTemplateBasic(std::string name);
		MarSystemTemplateBasic(const MarSystemTemplateBasic& a);//copy ctor
		~MarSystemTemplateBasic();
		MarSystem* clone() const;  

		void myProcess(realvec& in, realvec& out);
	};

}//namespace Marsyas

#endif //MARSYAS_MARSYSTEMTEMPLATEBASIC_H
