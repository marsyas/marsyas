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
    \class Conversions
    \brief Various conversion functions
*/

#ifndef MARSYAS_CONVERSIONS_H
#define MARSYAS_CONVERSIONS_H

#include "common.h"

#include <math.h> 
#include <string> 


real pitch2hertz(real pitch);
natural hertz2samples(real hz, real srate);
real samples2hertz(natural samples, real srate);
real hertz2pitch(real hz);

natural time2samples(std::string time, real srate);
natural time2usecs(std::string time);



#endif

	
