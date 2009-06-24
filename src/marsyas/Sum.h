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

#ifndef MARSYAS_SUM_H
#define MARSYAS_SUM_H

#include "MarSystem.h"

namespace Marsyas
{
/**
	\class Sum
	\ingroup Processing Basic
	\brief Sum the input rows of observations into one row.

	Sum the input rows of observation into one row. Frequently
	used for mixing audio signals before SoudFileSink or AudioSink.

	Controls:
	- \b mrs_real/weight [w] : scales input to avoid clipping.

	\deprecated This MarSystem has a poorly chosen name "Sum", which is too
	general and does not clearly communicates what kind of summation
	is implemented: summation of rows, columns, the complete slice, with
	or without inter-slice memory maybe?
	For the most obvious use case: averaging observations channels into
	one mono channel, use MixToMono instead.


	\see MixToMono

	\todo Sum mrs_real/weight should be implemented as a meta-composite in
	marsystemmanager.  (?   -- lmartins said this)

*/


class Sum: public MarSystem
{
private:
	void myUpdate(MarControlPtr sender);
	MarControlPtr ctrl_weight_;
	void addControls();
public:
	Sum(std::string name);
	Sum(const Sum& a);
	~Sum();
	MarSystem* clone() const;

	void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif

