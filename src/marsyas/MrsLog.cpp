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

#include <iostream>
#include <fstream>
#include "MrsLog.h"
#include "common_source.h"

using std::ostringstream;
using std::cout;
using std::endl;


using namespace Marsyas;

mrs_string MrsLog::fname_ = "marsyas.log";
bool MrsLog::warnings_off_ = false;
bool MrsLog::messages_off_ = false;


void 
MrsLog::setLogFile(mrs_string fname)
{
	fname_ = fname;
}

void 
MrsLog::mrsMessage(const ostringstream& oss)
{
#ifdef MARSYAS_LOG_MESSAGES
	if (!messages_off_) 
    {
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
