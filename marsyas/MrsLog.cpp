

#include "MrsLog.h"
#include <iostream>
#include <fstream>
using namespace std;


string MrsLog::fname_ = "marsyas.log";




void 
MrsLog::mrsErr(const ostringstream& oss)
{
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
}

void 
MrsLog::mrsWarning(const ostringstream& oss)
{
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
}


void 
MrsLog::mrsDiagnostic(const ostringstream& oss)
{
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
}



void 
MrsLog::mrsDebug(const ostringstream& oss)
{
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
}




void 
MrsLog::setLogFile(string fname)
{
  fname_ = fname;
}






void 
MrsLog::mrsAssert(char *strFile, unsigned uLine) 
{
  fflush(NULL);
  fprintf(stderr, "\nMARSYAS Assertion failed: %s, line %u\n", 
	  strFile, uLine);
  fflush(stderr);
  abort();
}

