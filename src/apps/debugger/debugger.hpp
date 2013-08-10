#ifndef MARSYAS_DEBUGGER_DEBUGGER_INCLUDED
#define MARSYAS_DEBUGGER_DEBUGGER_INCLUDED

#include "recording.hpp"

#include <MarSystem.h>
#include <MarControl.h>

using namespace std;
using namespace Marsyas;

class debugger
{
  MarSystem * m_system;
  recorder m_recorder;
  const recording * m_reference;
  recording::const_iterator m_reference_iter;

public:
  enum bug_flags {
    no_bug = 0,
    path_missing,
    format_mismatch,
    value_mismatch
  };

  struct bug {
    bug() : flags(no_bug), max_deviation(0.0), average_deviation(0.0) {}
    bug_flags flags;
    mrs_real max_deviation;
    mrs_real average_deviation;
  };

  typedef std::map<std::string, bug> report;

  debugger( MarSystem * system, const recording *reference ):
    m_system(system),
    m_recorder(system),
    m_reference(reference),
    m_reference_iter(reference->records.begin())
  {}

  bool advance()
  {
    if (!at_end()) {
      ++m_reference_iter;
      m_recorder.clear_record();
    }
    return !at_end();
  }

  bool at_end()
  {
    return m_reference_iter == m_reference->records.end();
  }

  void rewind()
  {
    m_reference_iter = m_reference->records.begin();
    m_recorder.clear_record();
  }

  report * evaluate()
  {
    if (at_end())
      return 0;

    report * bug_report = new report;

    record * reference_rec = *m_reference_iter;
    record * actual_rec = m_recorder.current_record();

    for (unsigned int p = 0; p < m_reference->path_count(); ++p)
    {
      bug bug_info;

      const std::string & path = m_reference->paths[p];

      if (p >= m_recorder.paths().size()
          || m_recorder.paths()[p] != path)
      {
        bug_info.flags = path_missing;
        bug_report->insert(pair<string, bug>(path, bug_info));
        continue;
      }

      const mrs_realvec & ref_data = reference_rec->entries[p];
      const mrs_realvec & act_data = actual_rec->entries[p];

      if (ref_data.getRows() != act_data.getRows() ||
          ref_data.getCols() != act_data.getCols())
      {
        bug_info.flags = format_mismatch;
        bug_report->insert(pair<string, bug>(path, bug_info));
        continue;
      }

      mrs_real max_dev = 0.0, avg_dev = 0.0;
      for (int i = 0; i < ref_data.getSize(); ++i)
      {
        if (ref_data(i) != act_data(i))
        {
          mrs_real dev = std::abs( act_data(i) - ref_data(i) );
          if (dev > max_dev)
            max_dev = dev;
          avg_dev += dev;
        }
      }

      if (ref_data.getSize())
        avg_dev /= ref_data.getSize();

      if (max_dev != 0.0)
      {
        bug_info.flags = value_mismatch;
        bug_info.average_deviation = avg_dev;
        bug_info.max_deviation = max_dev;
        bug_report->insert(pair<string, bug>(path, bug_info));
        continue;
      }
    }

    delete actual_rec;

    return bug_report;
  }
};

#endif // MARSYAS_DEBUGGER_DEBUGGER_INCLUDED
