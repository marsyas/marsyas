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
    \brief Class for storing clusters description

  
*/

#ifndef MARSYAS_PECLUSTERS_H
#define MARSYAS_PECLUSTERS_H

#include "realvec.h"
#include <fstream>
#include <string>
using std::ofstream;

namespace Marsyas
{
	typedef enum {
	start,
	length,
	phase,
	deltaFrequency,
	deltaAmplitude,
	firstLabel,
	thruth,
	nbClustersParameters
	} ctParameter;


	class PeCluster {
		friend class PeClusters;
	private :
	mrs_natural envSize;
	mrs_natural histSize;

	mrs_real start;
	mrs_real end;
mrs_real length;
mrs_natural oriLabel;
mrs_natural groundLabel;
mrs_natural label;

realvec temporalEnvelope;
realvec spectralEnvelope;

realvec frequencyDistribution;
realvec amplitudeDistribution;
realvec harmonicityDistribution;

	public :
 PeCluster();
  ~PeCluster();

	void init(realvec& peakSet, mrs_natural l);
	 
	void toVec(realvec&, std::string type);

	mrs_natural getGroundThruth ();
	void setGroundThruth (mrs_natural);

	mrs_natural getOriLabel ();
	void setOriLabel (mrs_natural);
	
	mrs_natural getLabel ();
	void setLabel (mrs_natural);
	} ;


	class PeClusters
	{
		private:
		realvec conversion;
PeCluster *set;
mrs_natural nbClusters;
mrs_natural nbFrames;

public:
		PeClusters(realvec &peakSet);
		~PeClusters();

		realvec &getConversionTable();
		void selectBefore(mrs_real val);
		void selectGround();

		void synthetize(realvec &peakSet, std::string fileName, std::string outFileName, mrs_natural Nw, mrs_natural D, mrs_natural S, mrs_natural bopt, mrs_natural residual=0);
	};


}

#endif

