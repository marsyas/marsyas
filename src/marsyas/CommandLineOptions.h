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

#include <marsyas/common_header.h>

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <typeinfo>

namespace Marsyas
{

/**
\ingroup Notmar
\brief Utility for handling program options
*/

class marsyas_EXPORT CommandLineOptions
{
private:

  struct Option
  {
    Option(): is_set(false) {}
    virtual ~Option() {}
    virtual bool parse_value( std::istringstream & argument ) = 0;
    bool is_set;
    std::string short_name;
    std::string value_name;
    std::string description;
  };

  template <typename T>
  struct OptionT : public Option
  {
    OptionT( const T & default_value ):
      default_value(default_value),
      value()
    {}
    bool parse_value( std::istringstream & argument )
    {
      argument >> value;
      bool ok = !argument.fail();
      if (ok)
        is_set = true;
      return ok;
    }
    T default_value;
    T value;
  };

  template <typename T> OptionT<T> * option_cast(Option *option) const
  {
    if (typeid(*option) == typeid(OptionT<T>))
      return static_cast<OptionT<T>*>(option);
    else
      return 0;
  }

  std::vector<std::string> m_arguments;
  std::vector<std::string> m_remaining;

  std::map<std::string, std::string> m_long_names;
  std::map<std::string, Option *> m_options;

  template <typename T>
  void define_private(const std::string & long_name,
                      const std::string & short_name,
                      const std::string & value_name,
                      const std::string & description,
                      const T & default_value = T())
  {
    Option *option = new OptionT<T>(default_value);
    option->short_name = short_name;
    option->value_name = value_name;
    option->description = description;
    m_options[long_name] = option;
    if (!short_name.empty())
      m_long_names[short_name] = long_name;
  }

public:
  CommandLineOptions();
  virtual ~CommandLineOptions();

  void print() const;

  bool readOptions(int argc, const char** argv);

  template <typename T>
  void define(const std::string & long_name,
              char short_name,
              const std::string & value_name,
              const std::string & description,
              const T & default_value = T())
  {
    define_private(long_name,
                   std::string(1, short_name),
                   value_name,
                   description,
                   default_value);
  }

  template <typename T>
  void define(const std::string & long_name,
              const std::string & value_name,
              const std::string & description,
              const T & default_value = T())
  {
    define_private(long_name,
                   std::string(),
                   value_name,
                   description,
                   default_value);
  }

  template <typename T>
  void define_old_style(const std::string & long_name,
                        const std::string & short_name,
                        const std::string & value_name,
                        const std::string & description,
                        const T & default_value = T())
  {
    define_private(long_name, short_name, value_name, description, default_value);
  }

  bool has( const std::string & long_name ) const
  {
    std::map<std::string, Option*>::const_iterator it = m_options.find(long_name);
    return (it != m_options.end() && (it->second->is_set));
  }

  template<typename T>
  T value( const std::string & long_name ) const
  {
    std::map<std::string, Option*>::const_iterator it = m_options.find(long_name);
    if (it == m_options.end())
      return T();
    OptionT<T> *typed_option = option_cast<T>(it->second);
    if (!typed_option)
      return T();
    return typed_option->is_set ? typed_option->value : typed_option->default_value;
  }

  void addBoolOption(const std::string & long_name,
                     const std::string & short_name,
                     bool value = false)
  {
    define_private(long_name, short_name, "", "", value);
  }

  void addRealOption(const std::string & long_name,
                     const std::string & short_name,
                     mrs_real value)
  {
    define_private(long_name, short_name, "", "", value);
  }

  void addNaturalOption(const std::string & long_name,
                        const std::string & short_name,
                        mrs_natural value)
  {
    define_private(long_name, short_name, "", "", value);
  }

  void addStringOption(const std::string & long_name,
                       const std::string & short_name,
                       const std::string & value)
  {
    define_private(long_name, short_name, "", "", value);
  }

  bool getBoolOption(const std::string & long_name) const
  {
    return has(long_name);
  }

  mrs_natural getNaturalOption(const std::string & long_name) const
  {
    return value<mrs_natural>(long_name);
  }

  mrs_real getRealOption(const std::string & long_name) const
  {
    return value<mrs_real>(long_name);
  }

  std::string getStringOption(const std::string & long_name) const
  {
    return value<std::string>(long_name);
  }

  const std::vector<std::string> & getRemaining() const
  {
    return m_remaining;
  }
};

template <>
struct CommandLineOptions::OptionT<bool> : public CommandLineOptions::Option
{
  OptionT(bool set) { is_set = set; }
  bool parse_value( std::istringstream& ) { return false; }
};

template <> inline
bool CommandLineOptions::value<bool>( const std::string & long_name ) const
{
  return has(long_name);
}

} //namespace Marsyas

#endif
