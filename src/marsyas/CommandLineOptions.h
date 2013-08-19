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


#ifndef MARSYAS_COMMANDLINEOPTIONS_H
#define MARSYAS_COMMANDLINEOPTIONS_H

#include "common_header.h"

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <cstdlib>

namespace Marsyas
{

/**
\ingroup Notmar
\brief Utility for handling program options
*/

class marsyas_EXPORT CommandLineOptions
{
private:

  template <typename T>
  struct Option
  {
    T default_value;
    T value;
    bool is_set;
  };

  std::vector<std::string> m_arguments;
  std::vector<std::string> m_remaining;

  std::map<std::string, std::string> m_long_names;
  std::map<std::string, Option<bool> > m_bool_options;
  std::map<std::string, Option<mrs_real> > m_real_options;
  std::map<std::string, Option<mrs_natural> > m_natural_options;
  std::map<std::string, Option<std::string> > m_string_options;

  template <typename T>
  void addOption(const std::string & long_name,
                 const std::string & short_name,
                 const T & value,
                 std::map<std::string, Option<T> > & option_map );

  template <typename T>
  bool isOptionSet( const std::string & long_name,
                    const std::map<std::string, Option<T> > & option_map ) const;

  template<typename T>
  T getOption( const std::string & long_name,
               const std::map<std::string, Option<T> > & option_map ) const;

  template<typename T>
  bool getOptionValue( std::vector<std::string>::const_iterator arg,
                       T & value );

public:
  CommandLineOptions();

  bool readOptions(int argc, const char** argv);

  void addBoolOption(const std::string & lname, const std::string & sname,
                     bool value = false);
  void addRealOption(const std::string & lname, const std::string & sname,
                     mrs_real value);
  void addNaturalOption(const std::string & lname, const std::string & sname,
                        mrs_natural value);
  void addStringOption(const std::string & lname, const std::string & sname,
                       const std::string & value);

  bool isBoolOptionSet(const std::string & long_name) const;
  bool isRealOptionSet(const std::string & long_name) const;
  bool isNaturalOptionSet(const std::string & long_name) const;
  bool isStringOptionSet(const std::string & long_name) const;

  bool getBoolOption(const std::string & lname) const;
  mrs_natural getNaturalOption(const std::string & lname) const;
  mrs_real getRealOption(const std::string & lname) const;
  std::string getStringOption(const std::string & lname) const;
  const std::vector<std::string> & getRemaining() const;
};

template <>
struct CommandLineOptions::Option<bool>
{
  bool is_set;
};

template <typename T>
void CommandLineOptions::addOption
(const std::string & long_name, const std::string & short_name,
 const T & value, std::map<std::string, Option<T> > & option_map )
{
  option_map[long_name].value = value;
  m_long_names[short_name] = long_name;
}

template <typename T>
bool CommandLineOptions::isOptionSet
( const std::string & long_name, const std::map<std::string, Option<T> > & option_map ) const
{
  typename std::map<std::string, Option<T> >::const_iterator it;
  it = option_map.find(long_name);
  return (it != option_map.end() && it->second.is_set);
}

template<typename T>
T CommandLineOptions::getOption
( const std::string & long_name, const std::map<std::string, Option<T> > & option_map ) const
{
  typename std::map<std::string, Option<T> >::const_iterator it;
  it = option_map.find(long_name);
  if (it != option_map.end())
  {
    const Option<T> & option = it->second;
    return option.is_set ? option.value : option.default_value;
  }
  else
  {
    return T();
  }
}

}//namespace Marsyas

#endif



