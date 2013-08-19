/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "CommandLineOptions.h"

using namespace std;

namespace Marsyas {

CommandLineOptions::CommandLineOptions()
{
}

void
CommandLineOptions::addBoolOption(const std::string & long_name,
                                  const std::string & short_name,
                                  bool value)
{
  // FIXME:
  // It should not be possible to add a bool option already set,
  // because it can not be un-set via command-line.
  m_bool_options[long_name].is_set = value;
  m_long_names[short_name] = long_name;
}

void
CommandLineOptions::addNaturalOption(const std::string & long_name,
                                     const std::string & short_name,
                                     mrs_natural value)
{
  addOption(long_name, short_name, value, m_natural_options);
}

void
CommandLineOptions::addRealOption(const std::string & long_name,
                                  const std::string & short_name,
                                  mrs_real value)
{
  addOption(long_name, short_name, value, m_real_options);
}

void
CommandLineOptions::addStringOption(const std::string & long_name,
                                    const std::string & short_name,
                                    const std::string & value)
{
  addOption(long_name, short_name, value, m_string_options);
}


bool CommandLineOptions::isBoolOptionSet(const std::string & long_name) const
{
  return isOptionSet(long_name, m_bool_options);
}

bool CommandLineOptions::isRealOptionSet(const std::string & long_name) const
{
  return isOptionSet(long_name, m_real_options);
}

bool CommandLineOptions::isNaturalOptionSet(const std::string & long_name) const
{
  return isOptionSet(long_name, m_natural_options);
}

bool CommandLineOptions::isStringOptionSet(const std::string & long_name) const
{
  return isOptionSet(long_name, m_string_options);
}


bool
CommandLineOptions::getBoolOption(const std::string & long_name) const
{
  return isBoolOptionSet(long_name);
}

mrs_natural
CommandLineOptions::getNaturalOption(const std::string & long_name) const
{
  return getOption(long_name, m_natural_options);
}

mrs_real
CommandLineOptions::getRealOption(const std::string & long_name) const
{
  return getOption(long_name, m_real_options);
}

string
CommandLineOptions::getStringOption(const std::string & long_name) const
{
  return getOption(long_name, m_string_options);
}

const std::vector<std::string> &
CommandLineOptions::getRemaining() const
{
  return m_remaining;
}


template<typename T>
bool getValueArgument( const std::vector<std::string> & args,
                       int & arg_index,
                       T & value, const char * required_type_name )
{
  const std::string option_arg = args[arg_index];
  if (arg_index >= (int) args.size()-1)
  {
    std::cerr << "Missing value to option: " << args[arg_index] << std::endl;
    return false;
  }
  ++arg_index;
  const std::string value_arg = args[arg_index];
  istringstream value_stream( value_arg );
  value_stream >> value;
  if (value_stream.fail())
  {
    std::cerr << "Invalid option value (" << required_type_name << " required): "
              << option_arg << ' ' << value_arg << std::endl;
    return false;
  }
  return true;
}

bool CommandLineOptions::readOptions(int argc, const char **argv)
{
  for (int i=0; i < argc; ++i)
  {
    m_arguments.push_back(argv[i]);
  }

  for (int i=1; i < argc; ++i)
  {
    const string & argument = m_arguments[i];
    string option_name;

    if (argument.size() > 1 && argument[0] == '-')
    {
      if (argument.size() > 2 && argument[1] == '-')
      {
        option_name = argument.substr(2);
      }
      else
      {
        string short_name = argument.substr(1);
        std::map<std::string, std::string>::iterator it;
        it = m_long_names.find(short_name);
        if (it != m_long_names.end())
        {
          option_name = it->second;
        }
        else
        {
          cerr << "Invalid option: " << argument << endl;
          return false;
        }
      }
    }

    if (option_name.empty())
    {
      m_remaining.push_back(argument);
      continue;
    }

    std::map<std::string, Option<bool> >::iterator bool_it;
    bool_it = m_bool_options.find(option_name);
    if (bool_it != m_bool_options.end())
    {
      bool_it->second.is_set = true;
      continue;
    }

    {
      std::map<std::string, Option<mrs_natural> >::iterator it;
      it = m_natural_options.find(option_name);
      if (it != m_natural_options.end())
      {
        if (!getValueArgument(m_arguments, i, it->second.value, "natural number"))
          return false;

        it->second.is_set = true;
        continue;
      }
    }

    {
      std::map<std::string, Option<mrs_real> >::iterator it;
      it = m_real_options.find(option_name);
      if (it != m_real_options.end())
      {
        if (!getValueArgument(m_arguments, i, it->second.value, "real number"))
          return false;

        it->second.is_set = true;
        continue;
      }
    }

    {
      std::map<std::string, Option<string> >::iterator it;
      it = m_string_options.find(option_name);
      if (it != m_string_options.end())
      {
        if (!getValueArgument(m_arguments, i, it->second.value, "string number"))
          return false;

        it->second.is_set = true;
        continue;
      }
    }

    cerr << "Invalid option: " << argument << endl;
    return false;
  }

  return true;
}

} // namespace Marsyas
