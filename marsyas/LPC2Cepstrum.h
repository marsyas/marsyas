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

/** 
\class LPC2Cepstrum
\brief Convert LPC coefficients to Cepstrum coefficients.

This MarSystem is expecting to receive at its input LPC coefficients + Pitch + Power,
as output by the LPCWarped MarSystem (see LPCWarped.cpp/.h). It only converts the LPC
coefficients to cepstral coefficients and ignores the pitch and power values received 
from LPCwarped.

Code by:
Luís Gustavo Martins - lmartins@inescporto.pt
November 2006
*/

#ifndef MARSYAS_LPC2CEPSTRUM_H
#define MARSYAS_LPC2CEPSTRUM_H

#include "MarSystem.h"

namespace Marsyas
{

	class LPC2Cepstrum: public MarSystem
	{
	private:
		MarControlPtr ctrl_cepstralOrder_;
		mrs_natural LPCorder_;

		void addControls();
		void myUpdate();

	public:
		LPC2Cepstrum(std::string name);
		LPC2Cepstrum(const LPC2Cepstrum& a);
		~LPC2Cepstrum();

		MarSystem* clone() const;  

		void myProcess(realvec& in, realvec& out);
	};

}//namespace Marsyas

#endif



