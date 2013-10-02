/*
** Copyright (C) 1998-2008 George Tzanetakis <gtzan@cs.uvic.ca>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
3** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


#include "SOM.h"
#include "../common_source.h"

using namespace std;
using namespace Marsyas;



#define ALPHA_DEGRADE		0.98
#define NEIGHBOURHOOD_DEGRADE	0.97

SOM::SOM(mrs_string name):MarSystem("SOM",name)
{
  //type_ = "SOM";
  //name_ = name;

  addControls();
}


SOM::~SOM()
{
}



SOM::SOM(const SOM& a) : MarSystem(a)
{
  ctrl_gridmap_ = getctrl("mrs_realvec/grid_map");
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

  addctrl("mrs_realvec/grid_map", realvec(), ctrl_gridmap_);

  addctrl("mrs_bool/done", false);
  setctrlState("mrs_bool/done", true);

  addctrl("mrs_real/alpha", 1.0);
  setctrlState("mrs_real/alpha", true);

  addctrl("mrs_real/neigh_std", 1.0);
  setctrlState("mrs_real/neigh_std", true);

  addctrl("mrs_real/alpha_decay_init", ALPHA_DEGRADE);
  setctrlState("mrs_real/alpha_decay_init", true);

  addctrl("mrs_real/alpha_decay_train", ALPHA_DEGRADE);
  setctrlState("mrs_real/alpha_decay_train", true);

  addctrl("mrs_real/neighbourhood_decay_init", NEIGHBOURHOOD_DEGRADE);
  setctrlState("mrs_real/neighbourhood_decay_init", true);

  addctrl("mrs_real/neighbourhood_decay_train", NEIGHBOURHOOD_DEGRADE);
  setctrlState("mrs_real/neighbourhood_decay_train", true);

  addctrl("mrs_real/std_factor_train", 0.17);
  setctrlState("mrs_real/std_factor_train", true);

  addctrl("mrs_real/std_factor_init", 0.17);
  setctrlState("mrs_real/std_factor_init", true);
}


void
SOM::init_grid_map()
{
  //cout << "randomizing***********" << endl;
  MarControlAccessor acc_grid(ctrl_gridmap_);
  realvec& grid_map = acc_grid.to<mrs_realvec>();
  srand(0);

  for (int x=0; x < grid_width_; x++)
    for (int y=0; y < grid_height_; y++)
      for (int o=0; o < inObservations_ - 3; o++)
      {
        grid_map(x * grid_height_ + y, o) = randD(1.0);
      }

  alpha_ = getctrl("mrs_real/alpha")->to<mrs_real>();
  mrs_real std_ = getctrl("mrs_real/std_factor_train")->to<mrs_real>();
  neigh_std_ = ((0.5*(grid_width_+grid_height_)) * std_);



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
  //cout<<"SOM myUpdate*************"<<endl;
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("SOM.cpp - SOM:myUpdate");

  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", (mrs_natural)3);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

  grid_pos_.create(2);

  //defaultUpdate();[!]
  inObservations_ = getctrl("mrs_natural/inObservations")->to<mrs_natural>();

  // FIXME This variable is defined but (possibly) unused.
  // mrs_natural nlabels = getctrl("mrs_natural/nLabels")->to<mrs_natural>();

  grid_width_ = getctrl("mrs_natural/grid_width")->to<mrs_natural>();
  grid_height_ = getctrl("mrs_natural/grid_height")->to<mrs_natural>();

  mrs_natural grid_size = grid_width_ * grid_height_;

  mrs_natural mrows = (getctrl("mrs_realvec/grid_map")->to<mrs_realvec>()).getRows();
  mrs_natural mcols = (getctrl("mrs_realvec/grid_map")->to<mrs_realvec>()).getCols();

  mrs_string mode = getctrl("mrs_string/mode")->to<mrs_string>();

  if ((grid_size != mrows) ||
      (inObservations_-3 != mcols))
  {
    if (inObservations_ != 1)
    {
      MarControlAccessor acc_grid(ctrl_gridmap_);
      realvec& grid_map = acc_grid.to<mrs_realvec>();
      grid_map.create(grid_size, inObservations_-3);
      adjustments_.create(inObservations_-3);

      init_grid_map();
    }

  }
}

void
SOM::find_grid_location(realvec& in, int t)
{
  mrs_natural o;
  //  int temp;
  mrs_real ival;				// input value
  mrs_real pval;				// prototype value
  mrs_real minDist = MAXREAL;

  MarControlAccessor acc_grid(ctrl_gridmap_);
  realvec& grid_map = acc_grid.to<mrs_realvec>();

  for (int x=0; x < grid_width_; x++)
    for (int y=0; y < grid_height_; y++)
    {
      // for each point in the grid
      // calculate distance of
      // input feature vector to the
      // representatitve vector of that position

      mrs_real dist = 0.0;

      for (o=0; o < inObservations_-3; o++)
      {
        ival = in(o,t);
        pval = grid_map(x * grid_height_ + y, o);
        dist += (ival - pval) * (ival - pval);
        //cout << "dist:" << dist << " ival: " << ival << " pval " << pval <<  " x: " << x << " y: " << y << endl;
      }

      if (dist < minDist)
      {
        minDist = dist;
        grid_pos_(0) = x;
        grid_pos_(1) = y;
      }
    }




}



void
SOM::myProcess(realvec& in, realvec& out)
{
  mrs_string mode = getctrl("mrs_string/mode")->to<mrs_string>();

  mrs_natural o,t;
  mrs_real geom_dist;
  mrs_real geom_dist_gauss;

  int px;
  int py;

  MarControlAccessor acc_grid(ctrl_gridmap_);
  realvec& grid_map = acc_grid.to<mrs_realvec>();


  if (mode == "train")
  {

    mrs_real dx;
    mrs_real dy;
    mrs_real adj;

    for (t=0; t < inSamples_; t++)
    {


      px = (int) in(inObservations_-2, t);
      py = (int) in(inObservations_-1, t);



      if ((px == -1.0)&&(px == -1.0))
      {
        find_grid_location(in, t);
        px = (int) grid_pos_(0);
        py = (int) grid_pos_(1);
      }
      out(0,t) = px;
      out(1,t) = py;
      out(2,t) = in(inObservations_-3,t);

      for (int x=0; x < grid_width_; x++)
        for (int y=0; y < grid_height_; y++)
        {
          dx = px-x;
          dy = py-y;
          geom_dist = sqrt((double)(dx*dx + dy*dy));
          geom_dist_gauss = gaussian( geom_dist, 0.0, neigh_std_, false);

          // subtract map vector from training data vector
          adj = alpha_ * geom_dist_gauss;
          for (o=0; o < inObservations_-3; o++)
          {
            adjustments_(o) = in(o,t) - grid_map(x * grid_height_ + y, o);
            adjustments_(o) *= adj;
            grid_map(x * grid_height_ + y, o) += adjustments_(o);
          }
        }
    }

    alpha_ *= getctrl("mrs_real/alpha_decay_train")->to<mrs_real>();
    neigh_std_ *= getctrl("mrs_real/neighbourhood_decay_train")->to<mrs_real>();



  }
  if (mode == "init")
  {
    mrs_real dx;
    mrs_real dy;
    mrs_real adj;

    mrs_real std_ = getctrl("mrs_real/std_factor_init")->to<mrs_real>();
    neigh_std_ = ((0.5*(grid_width_+grid_height_)) * std_);

    for (t=0; t < inSamples_; t++)
    {
      // no need to find grid locations, just read from the file
      px = (int) in( in.getRows() - 2, t);
      py = (int) in( in.getRows() - 1, t);
      for(int i =0; i < inObservations_ - 3; ++i)
      {
        grid_map(px * grid_height_ + py, i) = in(i);
      }

      for (int x=0; x < grid_width_; x++)
        for (int y=0; y < grid_height_; y++)
        {
          dx = px-x;
          dy = py-y;
          geom_dist = sqrt((double)(dx*dx + dy*dy));
          geom_dist_gauss = gaussian( geom_dist, 0.0, neigh_std_, false);

          // subtract map vector from training data vector
          adj = alpha_ * geom_dist_gauss;
          for (o=0; o < inObservations_-3; o++)
          {
            adjustments_(o) = in(o,t) - grid_map(x * grid_height_ + y, o);
            adjustments_(o) *= adj;
            grid_map(x * grid_height_ + y, o) += adjustments_(o);
          }

        }


    }
    //WARNING: UGLY HACK
    // Last two rows of init features are x,y locations
    // so we want to set all the coresponding rows in the SOM to 0
    // to prevent randomness from creaping in.
    for (int x=0; x < grid_width_; x++)
    {
      for (int y=0; y < grid_height_; y++)
      {
        grid_map(x * grid_height_ + y, grid_map.getCols() - 2) = 0;
        grid_map(x * grid_height_ + y, grid_map.getCols() - 1) = 0;
        cout << "x: " << x << " y: " << y << endl;
      }
    }
    alpha_ *= getctrl("mrs_real/alpha_decay_init")->to<mrs_real>();
    neigh_std_ *= getctrl("mrs_real/neighbourhood_decay_init")->to<mrs_real>();

  }
  if (mode == "predict")
  {
    for (t=0; t < inSamples_; t++)
    {
      find_grid_location(in, t);
      px = (int) grid_pos_(0);
      py = (int) grid_pos_(1);



      out(0,t) = px;
      out(1,t) = py;
      out(2,t) = in(inObservations_-3,t);
    }
  }





}












