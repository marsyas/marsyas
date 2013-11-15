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

#include <marsyas/CommandLineOptions.h>

#include <iomanip>
#include <algorithm>

using namespace std;

namespace Marsyas {

CommandLineOptions::CommandLineOptions()
{
}

CommandLineOptions::~CommandLineOptions()
{
  std::map<std::string, Option *>::iterator it;
  for (it = m_options.begin(); it != m_options.end(); ++it)
  {
    delete it->second;
  }
}

void CommandLineOptions::print() const
{
  std::map<std::string, Option *>::const_iterator it;
  std::vector<std::string> syntax_strings;
  std::string::size_type syntax_field_width = 0;

  for (it = m_options.begin(); it != m_options.end(); ++it)
  {
    const std::string & long_name = it->first;
    Option *option = it->second;
    ostringstream syntax;
    if (!option->short_name.empty())
      syntax << '-' << option->short_name << ", ";
    syntax << "--" << long_name;
    if (!option->value_name.empty())
      syntax << " " << option->value_name;
    syntax_field_width = std::max( syntax_field_width, syntax.str().size() );
    syntax_strings.push_back( syntax.str() );
  }

  syntax_field_width += 2;
  int idx = 0;
  for (it = m_options.begin(); it != m_options.end(); ++it, ++idx)
  {
    Option *option = it->second;
    cout << left << "  " << setw(syntax_field_width) << syntax_strings[idx];
    if (!option->description.empty())
      cout << ": " << option->description;
    cout << endl;
  }
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

    std::map<std::string, Option*>::iterator option_it;
    option_it = m_options.find(option_name);
    if (option_it == m_options.end())
    {
      cerr << "Invalid option: " << argument << endl;
      return false;
    }

    Option *option = option_it->second;

    {
      OptionT<bool> *bool_option = option_cast<bool>(option);
      if (bool_option)
      {
        bool_option->is_set = true;
        continue;
      }
    }

    if (i == argc-1)
    {
      std::cerr << "Missing value for option: " << argument << endl;
      return false;
    }

    ++i;
    std::istringstream value_stream( m_arguments[i] );
    if (!option->parse_value( value_stream ))
    {
      std::cerr << "Invalid value for option: "
                << argument << ' ' << value_stream.str() << endl;
      return false;
    }
  }

  return true;
}

} // namespace Marsyas
