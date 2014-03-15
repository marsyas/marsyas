/*
** Copyright (C) 1998-20010 George Tzanetakis <gtzan@cs.uvic.ca>
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

#ifndef MARSYAS_COLLECTIONFILESOURCE_H
#define MARSYAS_COLLECTIONFILESOURCE_H

#include "AbsSoundFileSource.h"
#include "SoundFileSource.h"
#include <marsyas/Collection.h>
#include "DownSampler.h"

namespace Marsyas
{
/**
   \class CollectionFileSource
   \ingroup Internal
   \brief CollectionFileSource abstracts a collections of soundfiles as a SoundFileSource.

   CollectionFileSource reads in a collection file consisting of a list of
   SoundFiles. The soundfiles are played one after the other with specifics
   such as offsets,durations as a single SoundFileSource.
*/


class CollectionFileSource: public AbsSoundFileSource
{
private:
  void addControls();
  void myUpdate(MarControlPtr sender);
  void getHeader(std::string filename);

  mrs_natural nChannels_;
  mrs_real myIsrate_;
  MarSystem* isrc_;
  MarSystem* downsampler_;

  Collection col_;
  bool mngCreated_;
  mrs_natural cindex_;
  mrs_real repetitions_;
  mrs_real duration_;

  mrs_natural advance_;

  realvec temp_;
  realvec tempi_;

public:
  CollectionFileSource(std::string name);
  CollectionFileSource(const CollectionFileSource& a);

  ~CollectionFileSource();
  MarSystem* clone() const;

  void myProcess(realvec& in,realvec &out);

  // public for efficiency; just like AbsFileSource
  mrs_bool iHasData_;
  mrs_bool iLastTickWithData_;
  mrs_bool iNewFile_;
};

}//namespace Marsyas


#endif    /* !MARSYAS_COLLECTIONFILESOURCE_H */




