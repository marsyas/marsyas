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
\class PeClusters
\brief 

*/

#include "PeClusters.h"
#include "PeUtilities.h"

using namespace std;
using namespace Marsyas;



PeCluster::PeCluster()
{
	groundLabel = -1;
	oriLabel=-1;
	label=-1;
}

PeCluster::~PeCluster()
{

}

void
PeCluster::init(realvec& peakSet, mrs_natural l)
{
	mrs_natural i;

	// set general informations
	start = MAXREAL;
	end=0;

	for (i=0 ; i< peakSet.getRows() ; i++)
		if (peakSet(i, pkGroup) == l)
		{
			if(peakSet(i, pkTime) < start)
			{
				start = peakSet(i, pkTime);
			}
			if(peakSet(i, pkTime) > end)
			{
				end= peakSet(i, pkTime);
			}
		}

		length = end-start;
		oriLabel = l;

		// compute envelopes

		// compute histograms

}


void PeCluster::toVec(realvec&, string type)
{

}

mrs_natural 
PeCluster::getGroundThruth ()
{
	return groundLabel;
}

void 
PeCluster::setGroundThruth (mrs_natural l)
{
	groundLabel = l;
}

mrs_natural 
PeCluster::getOriLabel ()
{
	return oriLabel;
}

void PeCluster::setOriLabel (mrs_natural l)
{
	oriLabel = l;
}

mrs_natural PeCluster::getLabel ()
{
	return label;
}

void PeCluster::setLabel (mrs_natural l)
{
	label = l;
}


///////////////////////////////////////////////////////////

//////// Clusters STUFF

////////////////////////////////////////////////////////////

PeClusters::PeClusters(realvec &peakSet)
{
	// determine the number of clusters
	nbClusters=0;
	for (int i=0 ; i<peakSet.getRows() ; i++)
	{
		if(peakSet(i, pkGroup) > nbClusters)
			nbClusters = (mrs_natural) peakSet(i, pkGroup);
	}

	// build the vector of clusters
	set = new PeCluster[nbClusters];
	for (int i=0 ; i<nbClusters ; i++)
	{
		set[i].init(peakSet, i);
	}

	conversion.stretch(nbClusters);
}

PeClusters::~PeClusters(){
	delete [] set;
}

realvec&
PeClusters::getConversionTable()
{
	for (int i=0 ; i<nbClusters ; i++)
	{
		conversion(i) = set[i].label;
	}
	return conversion;
}

void
PeClusters::selectBefore(mrs_real val)
{
	for (int i=0 ; i<nbClusters ; i++)
	{
		if(set[i].start < val)
			set[i].label = 0;
		else
			set[i].label = -1;
	}
}