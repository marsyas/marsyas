#ifndef MARSYAS_DEBUGGER_RECORDING_INCLUDED
#define MARSYAS_DEBUGGER_RECORDING_INCLUDED

#include <realvec.h>
#include <MarSystem.h>
#include <MarControl.h>

#include <list>
#include <vector>
#include <cassert>

using namespace Marsyas;

struct record
{
  std::vector<realvec> entries;
};

struct recording
{
  typedef std::list<record*>::iterator iterator;
  typedef std::list<record*>::const_iterator const_iterator;

  std::vector<std::string> paths;
  std::list<record*> records;

  unsigned int record_count() const { return records.size(); }
  unsigned int path_count() const { return paths.size(); }
};

class recorder : public MarSystemObserver
{
  MarSystem *m_system;
  recording *m_recording;
  std::map<std::string, realvec> m_current_values;

public:

  recorder(MarSystem *system):
    m_system(system),
    m_recording(new recording)
  {
    recursive_add_paths(system);
  }

  virtual ~recorder()
  {
    delete m_recording;
  }

  const std::vector<std::string> & paths() { return m_recording->paths; }

  const recording * product() { return m_recording; }

  const realvec * current_value( const std::string & path ) const
  {
    const auto & it = m_current_values.find(path);
    if (it != m_current_values.end())
      return &it->second;
    else
      return 0;
  }

  record *current_record()
  {
    record *rec = new record;
    for (const auto & path : m_recording->paths)
    {
      assert(m_current_values.find(path) != m_current_values.end());
      rec->entries.push_back(m_current_values[path]);
    }
    return rec;
  }

  void push_record()
  {
    m_recording->records.push_back(current_record());
    clear_record();
  }

  void clear_record()
  {
    m_current_values.clear();
  }

private:
  void processed( MarSystem * system, const realvec &in, const realvec &out)
  {
    (void) in;
    m_current_values[system->getAbsPath()] = out;
  }

  void recursive_add_paths(MarSystem *system)
  {
    system->addObserver(this);
    m_recording->paths.push_back( system->getAbsPath() );
    std::vector<MarSystem*> children = system->getChildren();
    for (MarSystem *child : children)
      recursive_add_paths(child);
  }
};

#endif // MARSYAS_DEBUGGER_RECORDING_INCLUDED
