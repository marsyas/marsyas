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

#ifndef MARSYAS_MRSLOG_H
#define MARSYAS_MRSLOG_H 1

#include <marsyas/common_header.h>
#include <marsyas/export.h>

#include <sstream>
#include <string>
#include <cstdlib>

namespace Marsyas
{
/**
\class MrsLog
\ingroup Special
\brief Logging Class

Logging class for diagnostic, warning, error, debug, etc messages. Can output
messages to a log file or std output (i.e. screen).
*/

class marsyas_EXPORT MrsLog
{
public:
  typedef void (*log_function_t)(const std::string & msg);

private:
  static std::string fname_;

  static log_function_t message_function_;
  static log_function_t warning_function_;
  static log_function_t error_function_;
  static log_function_t debug_function_;
  static log_function_t diagnostic_function_;

public:
  static bool warnings_off_;
  static bool messages_off_;

  static  void setLogFile(std::string fname);
  static  void getLogFile();

  static void setMessageFunction(log_function_t function);
  static void setWarningFunction(log_function_t function);
  static void setErrorFunction(log_function_t function);
  static void setDebugFunction(log_function_t function);
  static void setDiagnosticFunction(log_function_t function);
  static void setAllFunctions(log_function_t function);

  static  void mrsMessage(const std::ostringstream& oss);
  //logging methods
  static  void mrsErr(const std::ostringstream& oss);
  static  void mrsWarning(const std::ostringstream& oss);
  static  void mrsDiagnostic(const std::ostringstream& oss);
  static  void mrsDebug(const std::ostringstream& oss);
  static  void mrsAssert(const char *strFile, unsigned uLine);
};
} //namespec Marsyas

#endif
