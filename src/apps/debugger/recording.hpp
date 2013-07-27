#ifndef MARSYAS_DEBUGGER_RECORDING_INCLUDED
#define MARSYAS_DEBUGGER_RECORDING_INCLUDED

#include <realvec.h>
#include <MarSystem.h>
#include <MarControl.h>

#include <list>
#include <vector>

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

  int record_count() const { return records.size(); }
  int path_count() const { return paths.size(); }
};

class recorder
{
  MarSystem *m_system;
  recording *m_recording;

public:

  recorder(MarSystem *system):
    m_system(system),
    m_recording(new recording)
  {
    recursive_add_paths(system);
  }

  const recording * product() { return m_recording; }

  void store()
  {
    record *rec = new record;
    for (const std::string & path : m_recording->paths)
    {
      std::string output_path = path;
      output_path += "mrs_realvec/processedData";
      MarControlPtr output_control = m_system->getControl(output_path);
      assert(!output_control.isInvalid());
      const realvec & data = output_control->to<mrs_realvec>();
      rec->entries.push_back(data);
    }
    m_recording->records.push_back(rec);
  }

private:
  void recursive_add_paths(MarSystem *system)
  {
    m_recording->paths.push_back( system->getAbsPath() );
    std::vector<MarSystem*> children = system->getChildren();
    for (MarSystem *child : children)
      recursive_add_paths(child);
  }
};

#endif // MARSYAS_DEBUGGER_RECORDING_INCLUDED
