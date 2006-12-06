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

/**
\class MrsLog
\brief Logging Class

Logging class for diagnostic, warning, error, debug, etc messages. Can output
messages to a log file or std output (i.e. screen).
*/

#ifndef MARSYAS_MRSLOG_H
#define MARSYAS_MRSLOG_H 1	

#include <sstream> 
#include <string>

namespace Marsyas
{
	class MrsLog
	{
		static std::string fname_;

	public:   
		static void setLogFile(std::string fname);
		static void getLogFile();
	  
		static void mrsErr(const std::ostringstream& oss);
		static void mrsErr(const char* c) {mrsErr(std::ostringstream(c));}
		static void mrsErr(std::string c) {mrsErr(std::ostringstream(c));}
		
		static void mrsWarning(const std::ostringstream& oss);
		static void mrsWarning(const char* c) {mrsWarning(std::ostringstream(c));}
		static void mrsWarning(std::string c) {mrsWarning(std::ostringstream(c));}
		
		static void mrsDiagnostic(const std::ostringstream& oss);
		static void mrsDiagnostic(const char* c) {mrsDiagnostic(std::ostringstream(c));}
		static void mrsDiagnostic(std::string c) {mrsDiagnostic(std::ostringstream(c));}
		
		static void mrsDebug(const std::ostringstream& oss);
		static void mrsDebug(const char* c) {mrsDebug(std::ostringstream(c));}
		static void mrsDebug(std::string c) {mrsDebug(std::ostringstream(c));}
		
		static void mrsAssert(char *strFile, unsigned uLine);
	};
} //namespec Marsyas

#endif 
