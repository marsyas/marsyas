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

#if !defined(__QGMMMODEL_h)
#define __QGMMMODEL_H

#include <vector>
#include <marsyas/common_header.h>
#include <marsyas/realvec.h>

namespace Marsyas
{
/**
\class QGMMModel
	\ingroup none
\brief Quasi-GMM Model

Implementation by Luis Gustavo Martins - lmartins@inescporto.pt

*/


class QGMMModel
{
private:

  mrs_natural maxNrGSMixes_;
  mrs_natural nrGSMixes_;
  std::vector<mrs_natural> GSmixNrFrames_;
  std::vector<realvec> GSmixCovMatrix_;
  mrs_natural totalNrFrames_;

  mrs_real divergenceThres_;

  realvec updateCovModel(realvec C, mrs_natural N, realvec Cm, mrs_natural Nm);
  mrs_real deltaBIC(realvec C1, mrs_natural N1, realvec C2, mrs_natural N2, realvec C, mrs_real lambda);

public:

  QGMMModel();
  QGMMModel(const QGMMModel &a);
  ~QGMMModel();

  void updateModel(realvec &covMatrix, mrs_natural nrModelFrames);
  void resetModel();

  mrs_real BICdistance(realvec &covMatrix, mrs_natural Nframes, mrs_real lambda);

  mrs_natural nrGSMixes() {return nrGSMixes_; };
  realvec& GSmixCovMatrix(mrs_natural GSmix) { return GSmixCovMatrix_[GSmix]; };
  mrs_natural GSmixNrFrames(mrs_natural GSmix) { return GSmixNrFrames_[GSmix]; };
  mrs_natural totalNrFrames() {return totalNrFrames_; };

};

}//namespace Marsyas

#endif








