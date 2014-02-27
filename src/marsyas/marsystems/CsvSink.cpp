#include "CsvSink.h"
#include <iomanip>

using namespace std;

namespace Marsyas {

CsvSink::CsvSink(const string & name):
  MarSystem("CsvSink", name)
{
  addControl("mrs_string/filename", string());
  addControl("mrs_string/separator", string(" "));

  setControlState("mrs_string/filename", true);
  setControlState("mrs_string/separator", true);
}

CsvSink::CsvSink(const CsvSink & other):
  MarSystem(other)
{}

CsvSink::~CsvSink()
{
  m_file.close();
}

void CsvSink::myUpdate( MarControlPtr cause )
{
  MarSystem::myUpdate(cause);

  const string & new_filename = getControl("mrs_string/filename")->to<string>();
  if (new_filename != m_filename)
  {
    m_file.close();

    m_filename = new_filename;

    if (!m_filename.empty())
    {
      m_file.open(m_filename.c_str(), ofstream::out);
      m_file.precision(10);
    }
  }

  m_separator = getControl("mrs_string/separator")->to<string>();
}

void CsvSink::myProcess( realvec & in, realvec & out )
{
  out = in;

  if (!m_file.is_open())
    return;

  if (inObservations_ < 1)
    return;

  for (mrs_natural s = 0; s < inSamples_; ++s)
  {
    m_file << in(0,s);

    for (mrs_natural o = 1; o < inObservations_; ++o)
    {
      m_file << m_separator;
      m_file << in(o,s);
    }

    m_file << endl;
  }
}

} // namespace Marsyas
