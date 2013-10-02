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

#ifndef MARSYAS_ANNOTATOR_H
#define MARSYAS_ANNOTATOR_H

#include <marsyas/system/MarSystem.h>

#include <vector>

namespace Marsyas
{
/**
   \class Annotator
   \ingroup MachineLearning
   \brief Annotate a feature vector/slice with a label.

   The Annotator adds an observation row for labeling the feature stream.
   The label can be set through the mrs_natural/label control.
   Note that this label control expects a natural number representation of
   the label.
   Also note that only one label can be given at each processing step and this
   given label is applied to all samples of the time slice.

   Controls:
   - \b mrs_real/label [w]: number representation of the label;
     classification (i.e. integers) are stored as a mrs_real and
     rounded later.
   - \b mrs_bool/labelInFront [w]: toggle to put the label in front instead of
   at the end of the observations (which is the default).
   - \b mrs_string/annotationName [w]: the name to use for the annotation.

*/


class Annotator: public MarSystem
{
private:

  /// Implementation of the MarSystem::addControls() method.
  void addControls();

  /// Implementation of the MarSystem::myUpdate() method.
  void myUpdate(MarControlPtr sender);

  /// MarControl for setting the annotation label.
  MarControlPtr ctrl_label_;

  /// MarControl toggle for putting the annotation label in front of the observations.
  MarControlPtr ctrl_labelInFront_;




  /// Cache for storing the value of the labelInFront control.
  mrs_bool labelInFront_;

  /// MarControl for setting the annotation name.
  MarControlPtr ctrl_annotationName_;

public:

  /// Annotator constructor.
  Annotator(std::string name);

  /// Annotator copy constructor.
  Annotator(const Annotator& a);

  /// Annotator destructor.
  ~Annotator();

  MarSystem* clone() const;

  /// Implementation of the MarSystem::myProcess() method.
  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
