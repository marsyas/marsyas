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
    \class SOM
    \brief Self-organizing map


*/

#include "SOM.h"

using namespace std;
using namespace Marsyas;

#define INF		0x7fffffff
#define ALPHA_DEGRADE		0.98
#define NEIGHBOURHOOD_DEGRADE	0.97

SOM::SOM(string name):MarSystem("SOM",name)
{
  //type_ = "SOM";
  //name_ = name;
  
	srand(0);

	addControls();
}


SOM::~SOM()
{
}


MarSystem* 
SOM::clone() const 
{
  return new SOM(*this);
}

void 
SOM::addControls()
{
  addctrl("mrs_string/mode", "train");
  addctrl("mrs_natural/nLabels", 1);	// number of feature vectors 
  setctrlState("mrs_natural/nLabels", true);
  addctrl("mrs_natural/grid_width",  10);
  setctrlState("mrs_natural/grid_width", true);
  addctrl("mrs_natural/grid_height", 10);
  setctrlState("mrs_natural/grid_height", true);
  grid_map_.create(1);
  addctrl("mrs_realvec/grid_map", grid_map_);
  addctrl("mrs_bool/done", false);
  setctrlState("mrs_bool/done", true);

  addctrl("mrs_real/alpha", 1.0);
  setctrlState("mrs_real/alpha", true);
  
  addctrl("mrs_real/neigh_std", 1.0);
  setctrlState("mrs_real/neigh_std", true);  
}

 
void 
SOM::init_grid_map() 
{
  
  
  for (int x=0; x < grid_width_; x++) 
    for (int y=0; y < grid_height_; y++)
      for (int o=0; o < inObservations_ -1; o++)
      {
	grid_map_(x * grid_height_ + y, o) = randD(1.0);
      }
  
  alpha_ = getctrl("mrs_real/alpha")->toReal();
  neigh_std_ = ((0.5*(grid_width_+grid_height_)) /  4.0);
  
}


double 
SOM::gaussian(double x, double mean, double std, bool scale)
{
  if( scale )
    return (1.0/(std*sqrt(2.0*PI))) * exp( -(x-mean)*(x-mean)/(2.0*std*std) );
  
  return exp( -(x-mean)*(x-mean)/(2.0*std*std) );
}

double 
SOM::randD(double max)
{
  return max  *  (double)rand() / ((double)(RAND_MAX)+(double)(1.0)) ; 
}

void
SOM::myUpdate(MarControlPtr sender)
{
  MRSDIAG("SOM.cpp - SOM:myUpdate");

  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", (mrs_natural)3);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
  
  //defaultUpdate();[!]
	inObservations_ = getctrl("mrs_natural/inObservations")->toNatural();
  
  mrs_natural nlabels = getctrl("mrs_natural/nLabels")->toNatural();

  grid_width_ = getctrl("mrs_natural/grid_width")->toNatural();
  grid_height_ = getctrl("mrs_natural/grid_height")->toNatural();
  
  mrs_natural grid_size = grid_width_ * grid_height_;
  
  mrs_natural mrows = (getctrl("mrs_realvec/grid_map")->toVec()).getRows();
  mrs_natural mcols = (getctrl("mrs_realvec/grid_map")->toVec()).getCols();

  mrs_natural nrows = grid_map_.getRows();
  mrs_natural ncols = grid_map_.getCols();

  if ((grid_size != mrows) || 
      (inObservations_-1 != mcols))
  {
    if (inObservations_ != 1) 
		{
			grid_map_.create(grid_size, inObservations_-1);
			adjustments_.create(inObservations_-1);      
			init_grid_map();
			updctrl("mrs_realvec/grid_map", grid_map_);//[!] 	  
		}
    
  }

  if ((grid_size != nrows) || 
      (inObservations_-1 != ncols))
  {
    if (inObservations_ != 1) 
		{
			grid_map_.create(grid_size, inObservations_-1);
			adjustments_.create(inObservations_-1);
			init_grid_map();
			updctrl("mrs_realvec/grid_map", grid_map_);	//[!] 
		}
  }
  
  string mode = getctrl("mrs_string/mode")->toString();
  
  if (mode == "predict")
    {
      grid_map_ = getctrl("mrs_realvec/grid_map")->toVec();
    }  
}

realvec
SOM::find_grid_location(realvec& in, int t)
{
  realvec grid_pos;
  grid_pos.create(2);
  
//  int temp;
  mrs_real ival;				// input value
  mrs_real pval;				// prototype value 
  mrs_real minDist = INF;
 
  

  for (int x=0; x < grid_width_; x++) 
    for (int y=0; y < grid_height_; y++) 
      {
	// for each point in the grid 
	// calculate distance of 
	// input feature vector to the 
	// representatitve vector of that position 
	
	mrs_real dist = 0.0;

	for (o=0; o < inObservations_-1; o++)
	  {
	    ival = in(o,t);
	    pval = grid_map_(x * grid_height_ + y, o);
	    dist += (ival - pval) * (ival - pval);
	  }

	if (dist < minDist) 
	  {
	    minDist = dist;
	    grid_pos(0) = x;
	    grid_pos(1) = y;
	  }
      }
  


  
  return grid_pos;
}



void 
SOM::myProcess(realvec& in, realvec& out)
{
  //checkFlow(in,out);
  string mode = getctrl("mrs_string/mode")->toString();


  mrs_real geom_dist;
  mrs_real geom_dist_gauss;
  
  int px;
  int py;
  


  if (getctrl("mrs_bool/done")->toBool())
    {
      updctrl("mrs_realvec/grid_map", grid_map_);
      setctrl("mrs_bool/done", false);
    }
  else 
    {
      if (mode == "train")  
	{
	  for (t=0; t < inSamples_; t++) 
	    {
	      realvec grid_pos = find_grid_location(in, t);
	      px = (int) grid_pos(0);
	      py = (int) grid_pos(1);
	      out(0,t) = px;
	      out(1,t) = py;
	      out(2,t) = in(inObservations_-1,t);	  
	      
	      for (int x=0; x < grid_width_; x++) 
		for (int y=0; y < grid_height_; y++)
		  {
		    geom_dist = sqrt((double)(px -x) * (px - x) + (py - y) * (py -y));
		    geom_dist_gauss = gaussian( geom_dist, 0.0, neigh_std_, false);
		    
		    
		    // subtract map vector from training data vector 
		    for (o=0; o < inObservations_-1; o++) 
		      adjustments_(o) = in(o,t) - grid_map_(x * grid_height_ + y);
		    
		    adjustments_ *= alpha_ * geom_dist_gauss;
		    
		    for (o=0; o < inObservations_-1; o++) 
		      grid_map_(x * grid_height_ + y, o) += adjustments_(o);
		    
		    
		  }
	    }

	  
	  alpha_ *= ALPHA_DEGRADE;
	  neigh_std_ *= NEIGHBOURHOOD_DEGRADE;	  
	}
      if (mode == "predict")
	{
	  for (t=0; t < inSamples_; t++) 
	    {
	      realvec grid_pos = find_grid_location(in, t);
	      px = (int) grid_pos(0);
	      py = (int) grid_pos(1);
	      out(0,t) = px;
	      out(1,t) = py;
	      out(2,t) = in(inObservations_-1,t);	  
	    }
	}
    }
  


  

	

  
  
}






	

	

	
