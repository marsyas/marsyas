#include "file_io.hpp"
#include "recording.hpp"
#include "debugger.hpp"

#include <MarSystemManager.h>
#include <iostream>
#include <string>
#include <fstream>

using namespace std;
using namespace Marsyas;

int perform_record( MarSystem * system, const std::string & filename, int tick_count );
int perform_compare( MarSystem * system, const std::string & filename );

void printUsage()
{
  cout << "Usage: marsyas-debug [record|compare] <plugin-file> <recording-file> <tick-count>" << endl;
}

int main (int argc, char *argv[])
{
  if (argc < 4) {
    printUsage();
    return 1;
  }

  string command( argv[1] );
  if (command != "record" && command != "compare")
  {
    cerr << "Unrecognized command:" << command << endl;
    printUsage();
    return 1;
  }

  string plugin_filename( argv[2] );
  string record_filename( argv[3] );

  cout << "Using plugin file: " << plugin_filename << endl;
  cout << "Using recording file: " << record_filename << endl;

  MarSystemManager mng;
  ifstream plugin_stream( plugin_filename );
  MarSystem *system = mng.getMarSystem(plugin_stream);
  if (!system) {
    cout << "Failed to load plugin file." << endl;
    return 1;
  }

  int result = 0;

  if (command == "record")
  {
    int tick_count;
    if (argc < 5)
      tick_count = 1;
    else
      tick_count = atoi(argv[4]);
    if (tick_count < 1)
    {
      cerr << "Command 'record' requires 'tick-count' of 1 or greater." << endl;
      return 1;
    }
    result = perform_record(system, record_filename, tick_count);
  }
  else if (command == "compare")
  {
    result = perform_compare(system, record_filename);
  }

  return result;
}

int perform_record( MarSystem * system, const std::string & filename, int tick_count )
{
  recorder rec(system);

  while(tick_count--)
  {
    system->tick();
    rec.push_record();
  }

  const recording *written_recording = rec.product();
  recording_writer writer( system, written_recording );
  writer.write(filename);

  cout << "Done recording." << endl;
  return 0;
}

int perform_compare( MarSystem * system, const std::string & filename )
{
  recording_reader reader;
  recording * rec = reader.read(filename);
  if (!rec)
    return 1;

  recorder online_recorder(system);
  if (online_recorder.paths() != rec->paths)
  {
    cerr << "System and recording paths do not match!" << endl;
    return 1;
  }

  debugger debug(system, rec);

  cout << "Comparing " << rec->records.size() << " records..." << endl;
  cout << "Comparing paths:" << endl;

  for ( const auto & path : rec->paths )
  {
    cout << path << endl;
  }

  bool ok = true;
  int tick_count = 0;

  while (!debug.at_end())
  {
    ++tick_count;
    online_recorder.clear_record();
    system->tick();
    record *current_state = online_recorder.current_record();
    debugger::report *bug_report = debug.evaluate(current_state);

    debugger::report::iterator bug_report_iter;
    for( bug_report_iter = bug_report->begin();
         bug_report_iter != bug_report->end();
         ++bug_report_iter)
    {
      ok = false;
      const std::string & path = bug_report_iter->first;
      const debugger::bug & state = bug_report_iter->second;
      switch (state.flags)
      {
      case debugger::path_missing:
        cout << tick_count << "@" << path
             << " FAILURE: system has no control for path."
             << endl;
        break;
      case debugger::format_mismatch:
        cout << tick_count << "@" << path
             << " FAILURE: format mismatch: "
             //<< rec_data.getCols() << "x" << rec_data.getRows() << " != "
             //<< sys_data.getCols() << "x" << sys_data.getCols()
             << endl;
        break;
      case debugger::value_mismatch:
        cout << tick_count << "@" << path
             << " MISMATCH:"
             << " average deviation = " << state.average_deviation
             << ", maximum deviation = " << state.max_deviation
             << endl;
      default:
        break;
      }
    }
    delete bug_report;
    delete current_state;
    debug.advance();
  }

  if (ok)
    cout << "OK." << endl;

  return ok ? 0 : 1;
}
