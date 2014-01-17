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

#include <marsyas/MrsLog.h>
#include <marsyas/common_source.h>

#include <iostream>
#include <fstream>

using namespace Marsyas;
using namespace std;

mrs_string MrsLog::fname_ = "marsyas.log";
bool MrsLog::warnings_off_ = false;
bool MrsLog::messages_off_ = false;

MrsLog::log_function_t MrsLog::message_function_ = 0;
MrsLog::log_function_t MrsLog::warning_function_ = 0;
MrsLog::log_function_t MrsLog::error_function_ = 0;
MrsLog::log_function_t MrsLog::debug_function_ = 0;
MrsLog::log_function_t MrsLog::diagnostic_function_ = 0;

void
MrsLog::setLogFile(mrs_string fname)
{
  fname_ = fname;
}

void MrsLog::setMessageFunction(log_function_t function)
{
  message_function_ = function;
}

void MrsLog::setWarningFunction(log_function_t function)
{
  warning_function_ = function;
}

void MrsLog::setErrorFunction(log_function_t function)
{
  error_function_ = function;
}

void MrsLog::setDebugFunction(log_function_t function)
{
  debug_function_ = function;
}

void MrsLog::setDiagnosticFunction(log_function_t function)
{
  diagnostic_function_ = function;
}

void MrsLog::setAllFunctions(log_function_t function)
{
  message_function_ = function;
  warning_function_ = function;
  error_function_ = function;
  debug_function_ = function;
  diagnostic_function_ = function;
}

void
MrsLog::mrsMessage(const ostringstream& oss)
{
#ifdef MARSYAS_LOG_MESSAGES
  if (!messages_off_)
  {
    if (message_function_)
      message_function_(oss.str());

#ifdef MARSYAS_LOG2STDOUT
    cout << "[MRS_MESSAGE] " << oss.str() << endl;
#endif

#ifdef MARSYAS_LOG2STDERR
    std::cerr << "[MRS_MESSAGE] " << oss.str() << endl;
#endif

#ifdef MARSYAS_LOG2FILE
    ofstream ofs(fname_.c_str(), ios::out | ios::app);
    if (ofs.fail())
      return;
    if (!(ofs << "[MRS_MESSAGE] " <<  oss.str() << endl))
    {
      ofs.close();
      return;
    }
    ofs.close();
    return;

#endif
  }


#else
  (void) oss;
#endif

}



void
MrsLog::mrsErr(const ostringstream& oss)
{
#ifdef MARSYAS_LOG_ERRORS

  if (error_function_)
    error_function_(oss.str());

#ifdef MARSYAS_LOG2STDOUT
  cout << "[MRSERR] " << oss.str() << endl;
#endif

#ifdef MARSYAS_LOG2STDERR
  std::cerr << "[MRSERR] " << oss.str() << endl;
#endif

#ifdef MARSYAS_LOG2FILE
  ofstream ofs(fname_.c_str(), ios::out | ios::app);
  if (ofs.fail())
    return;
  if (!(ofs << "[MRSERR] " << oss.str() << endl))
  {
    ofs.close();
    return;
  }
  ofs.close();
  return;
#endif

#else
  (void) oss;
#endif
}

void
MrsLog::mrsWarning(const ostringstream& oss)
{
#ifdef MARSYAS_LOG_WARNINGS
  if (!warnings_off_)
  {
    if (warning_function_)
      warning_function_(oss.str());

#ifdef MARSYAS_LOG2STDOUT
    cout << "[MRS_WARNING] " << oss.str() << endl;
#endif

#ifdef MARSYAS_LOG2STDERR
    std::cerr << "[MRS_WARNING] " << oss.str() << endl;
#endif

#ifdef MARSYAS_LOG2FILE
    ofstream ofs(fname_.c_str(), ios::out | ios::app);
    if (ofs.fail())
      return;
    if (!(ofs << "[MRS_WARNING] " <<  oss.str() << endl))
    {
      ofs.close();
      return;
    }
    ofs.close();
    return;

#endif
  }


#else
  (void) oss;
#endif

}

void
MrsLog::mrsDiagnostic(const ostringstream& oss)
{
#ifdef MARSYAS_LOG_DIAGNOSTICS

  if (diagnostic_function_)
    diagnostic_function_(oss.str());

#ifdef MARSYAS_LOG2STDOUT
  cout << "[MRS_DIAG] " << oss.str() << endl;
#endif


#ifdef MARSYAS_LOG2STDERR
  std::cerr << "[MRS_DIAG] " << oss.str() << endl;
#endif

#ifdef MARSYAS_LOG2FILE
  ofstream ofs(fname_.c_str(), ios::out | ios::app);
  if (ofs.fail())
    return;
  if (!(ofs << "[MRS_DIAG] " <<  oss.str() << endl))
  {
    ofs.close();
    return;
  }
  ofs.close();
  return;
#endif

#else
  (void) oss;
#endif


}

void
MrsLog::mrsDebug(const ostringstream& oss)
{
  (void) oss;

  if (debug_function_)
    debug_function_(oss.str());

#ifdef MARSYAS_LOG2STDOUT
  cout << "[MRS_DEBUG] " << oss.str() << endl;
#endif

#ifdef MARSYAS_LOG2STDERR
  std::cerr << "[MRS_DEBUG] " << oss.str() << endl;
#endif


#ifdef MARSYAS_LOG2FILE
  ofstream ofs(fname_.c_str(), ios::out | ios::app);
  if (ofs.fail())
    return;
  if (!(ofs << "[MRS_DEBUG] " <<  oss.str() << endl))
  {
    ofs.close();
    return;
  }
  ofs.close();
  return;
#endif
}

void
MrsLog::mrsAssert(const char *strFile, unsigned uLine)
{
#ifdef MARSYAS_ASSERTS
  fflush(NULL);
  fprintf(stderr, "\nMARSYAS Assertion failed: %s, line %u\n",
          strFile, uLine);
  fflush(stderr);
  abort();
#else
  (void) strFile;
  (void) uLine;
#endif
}
