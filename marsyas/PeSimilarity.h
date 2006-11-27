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
    \class PeSimilarity
    \brief similarities computation routines for peaks extraction project

  
*/

#ifndef MARSYAS_PESIMILARITY_H
#define MARSYAS_PESIMILARITY_H

#include "realvec.h"


namespace Marsyas
{

	void similarityCompute(realvec&, realvec&);
void similarityMatrix(realvec&, realvec&, std::string type, mrs_natural, mrs_natural);
 void harmonicitySimilarityCompute(realvec&, std::vector<realvec>&, std::vector<realvec>&, realvec& m, mrs_natural);

}

#endif

