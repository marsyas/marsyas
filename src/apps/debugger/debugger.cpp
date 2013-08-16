/*
** Copyright (C) 1998-2013 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include <MarSystemManager.h>
#include <marsyas/debug/recorder.h>
#include <marsyas/debug/file_io.h>
#include <marsyas/debug/debugger.h>

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
  Debug::Recorder recorder(system);
  Debug::FileWriter writer(filename, system);

  while(tick_count--)
  {
    recorder.clear();
    system->tick();
    recorder.commit();
    writer.write( recorder.record() );
  }

  cout << "Done recording." << endl;
  return 0;
}

int perform_compare( MarSystem * system, const std::string & filename )
{
  Debug::FileReader reader(filename);
  Debug::Recorder recorder(system);

#if 0
  cout << "Comparing " << rec->records.size() << " records..." << endl;
  cout << "Comparing paths:" << endl;
  for ( const auto & path : rec->paths )
  {
    cout << path << endl;
  }

    recorder online_recorder(system);
  cout << "System paths:" << endl;
  for ( const auto & path : online_recorder.paths() )
    cout << path << endl;


  if (online_recorder.paths() != rec->paths)
  {
    cerr << "System and recording paths do not match!" << endl;
    return 1;
  }

  debugger debug(system, rec);
#endif
  bool ok = true;
  int tick_count = 0;

  Debug::Record reference_record;

  while (reader.read(reference_record))
  {
    system->tick();
    ++tick_count;

    recorder.commit();
    const Debug::Record & actual_record = recorder.record();

    Debug::BugReport report;
    Debug::compare(actual_record, reference_record, report);

    for (const auto & report_entry : report)
    {
      ok = false;
      const std::string & path = report_entry.first;
      const Debug::Bug & bug = report_entry.second;

      switch (bug.flags)
      {
      case Debug::path_missing:
        cout << tick_count << " @ " << path << endl
             << "\t" << "FAILURE: system has no control for path."
             << endl;
        break;
      case Debug::format_mismatch:
        cout << tick_count << " @ " << path << endl
             << "\t" << "FAILURE: format mismatch: "
             //<< rec_data.getCols() << "x" << rec_data.getRows() << " != "
             //<< sys_data.getCols() << "x" << sys_data.getCols()
             << endl;
        break;
      case Debug::value_mismatch:
        cout << tick_count << " @ " << path << endl
             << "\t" << "MISMATCH:"
             << " average deviation = " << bug.average_deviation
             << ", maximum deviation = " << bug.max_deviation
             << endl;
      default:
        break;
      }
    }

    reference_record.clear();
    recorder.clear();
  }

  if (ok)
    cout << "OK." << endl;

  return ok ? 0 : 1;
}
