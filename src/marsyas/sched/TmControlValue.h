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

#ifndef MARSYAS_TM_CONTROLVALUE_H
#define MARSYAS_TM_CONTROLVALUE_H

#include <string>
#include <iostream>

#include <marsyas/common_header.h>

namespace Marsyas
{
class MarSystem;

/**
	\brief type enumerations for individual timer control value types.
*/
enum {
  tmcv_null=0, /** null type */
  tmcv_real, /** real type */
  tmcv_natural, /** natural type */
  tmcv_string, /** string type */
  tmcv_bool, /** bool type */
  tmcv_vec, /** realvec type */
  tmcv_marsystem /** marsystem type */
};

// define an enum for marsystem type that doesn't class with the mar_* types
//#define mar_marsystem 8192

/**
	\ingroup Scheduler
	\brief TmControlValue is a simple parameter passing mechanism for updtimer
	\author Neil Burroughs  inb@cs.uvic.ca
*/
class TmControlValue
{
protected:
  /** \brief value type identifier */
  int type_;

  /** \brief supported value type */
  mrs_real r_;
  mrs_natural n_;
  bool b_;
  std::string s_;
  MarSystem* ms_;

public:
  /** \brief null value type (tmcv_null) */
  TmControlValue();
  /** \brief copy constructor
  * \param v TmControlValue to copy
  */
  TmControlValue(const TmControlValue& v);

//  MarControlValue& operator=(const MarControlValue& a);

  /** \brief real value type (tmcv_real)
  * \param re float value to assign as real
  */
  TmControlValue(float re);
  /** \brief real value type (tmcv_real)
  * \param re double value to assign as real
  */
  TmControlValue(double re);
  /** \brief natural value type (tmcv_natural)
  * \param ne int value to assign as natural
  */
  TmControlValue(int ne);
  /** \brief natural value type (tmcv_natural)
  * \param ne long int value to assign as natural
  */
  TmControlValue(long int ne);
  /** \brief string value type (tmcv_string)
  * \param st string value to assign
  */
  TmControlValue(std::string st);
  /** \brief string value type (tmcv_string)
  * \param cc const char* value to assign as string
  */
  TmControlValue(const char * cc);
  /** \brief bool value type (tmcv_bool)
  * \param be bool value to assign
  */
  TmControlValue(bool be);
  /** \brief MarSystem value type (tmcv_marsystem)
  * \param m MarSystem pointer value to assign
  */
  TmControlValue(MarSystem* m);

  /** \brief get value as real. If not a real value then a warning is
  * reported and 0.0 returned.
  * \return the real value or 0.0 if this is not a real.
  */
  mrs_real toReal();
  /** \brief get value as natural. If not a natural value then a warning
  * is reported and 0 returned.
  * \return the natural value or 0 if this is not a natural.
  */
  mrs_natural toNatural();
  /** \brief get value as bool. If not a bool value then a warning
  * is reported and false returned.
  * \return the bool value or false if this is not a bool.
  */
  bool toBool();
  /** \brief get value as string. If not a string value then a warning
  * is reported and "" returned.
  * \return the string value or "" if this is not a string.
  */
  std::string toString();
  /** \brief get value as MarSystem. If not a MarSystem value then a
  * warning is reported and NULL returned.
  * \return the MarSystem value or NULL if this is not a MarSystem.
  */
  MarSystem* toMarSystem();

  /** \brief get the type enumeration for this value type. The value is
  * one of the tmcv values defined in TmControlValue.h
  * \return the timer control value type enumeration.
  */
  int getType();
  /** \brief get the name of this value type.
  * \return the name of the type
  */
  std::string getSType();
  /** \brief send this object out the outstream.
  */
  friend std::ostream& operator<<(std::ostream&, const TmControlValue&);
};

}//namespace Marsyas

#endif


