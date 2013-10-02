/*
** Copyright (C) 2000-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include <marsyas/CommandLineOptions.h>

#include "common-reg.h"
#include "coreChecks.h"
#include "basicChecks.h"
#include "analysisChecks.h"

#include <cstdlib>

CommandLineOptions cmd_options;
string testName;
string outputFilename;
string answerFilename;
mrs_bool helpOpt;

void
printUsage(string progName)
{
  MRSDIAG("regressionChecks.cpp - printUsage");
  cerr << "regressionChecks, MARSYAS" << endl;
  cerr << "-------------------------" << endl;
  cerr << "Usage:" <<endl;
  cerr << "    " << progName;
  cerr << " -t testName file1 file2 ... fileN" <<endl;
  cerr << "        -o outfile.wav      (optional)" << endl;
  cerr << "        -a answerfile.wav   (optional)" << endl;
  cerr << endl << "List of all tests:" << endl;
  cerr << "    " << progName << " -h" << endl <<endl;
  exit(1);
}

void
printHelp(string progName)
{
  MRSDIAG("regressionChecks.cpp - printHelp");
  cerr << "regressionChecks, MARSYAS" << endl;
  cerr << "-------------------------" << endl;
  cerr << "Usage:" << endl;
  cerr << "    " << progName << " -t testName file1 file2 file3" << endl;
  cerr << " -o outfile.wav -a answerfile.wav"<<endl;
  cerr << endl << "    *** Core System tests ***" << endl;
  cerr << "null            : no test" << endl;
  cerr << "audiodevices    : test audio devices" << endl;
  cerr << "isClose         : test if two sound files are (almost) equal" << endl;
  cerr << "realvec         : test realvec" << endl;
  cerr << endl << "    *** Basic Audio Processing tests ***" << endl;
  cerr << "delay           : delay" << endl;
  cerr << "downsample      : downsample" << endl;
  cerr << "negative        : inverts samples" << endl;
  cerr << "shifter         : shifter" << endl;
  cerr << "shiftInput      : shift input" << endl;
  cerr << "windowing       : windows" <<endl;
// TODO: move vibrato test somewhere else
  cerr << "vibrato         : test vibrato" << endl;
  cerr << endl << "    *** Analysis tests ***" << endl;
  cerr << "pitch           : do pitch extraction" << endl;
  cerr << "transcriber     : do music transcription" << endl;
  exit(1);
}

void
initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
  cmd_options.addBoolOption("verbose", "v", false);
  cmd_options.addStringOption("testName", "t", EMPTYSTRING);
  cmd_options.addStringOption("output", "o", EMPTYSTRING);
  cmd_options.addStringOption("answer", "a", EMPTYSTRING);
}

void
loadOptions()
{
  helpOpt = (cmd_options.getBoolOption("help") ||
             cmd_options.getBoolOption("usage") ||
             cmd_options.getBoolOption("verbose") );
  testName = cmd_options.getStringOption("testName");
  outputFilename = cmd_options.getStringOption("output");
  answerFilename = cmd_options.getStringOption("answer");
}

int
main(int argc, const char **argv)
{
  // get name (ie "regressionChecks" or "regressionChecks.exe")
  string progName = argv[0];
  if (argc == 1) printUsage(progName);

  // handling of command-line options
  initOptions();
  cmd_options.readOptions(argc, argv);
  loadOptions();
  if (helpOpt)
    printHelp(progName);

  // get sound files or collections
  vector<string> soundfiles = cmd_options.getRemaining();
  string fname0 = EMPTYSTRING;
  string fname1 = EMPTYSTRING;

  if (soundfiles.size() > 0)
    fname0 = soundfiles[0];
  if (soundfiles.size() > 1)
    fname1 = soundfiles[1];

  /*
    cout << "Marsyas test name = " << testName << endl;
    cout << "fname0 = " << fname0 << endl;
    cout << "fname1 = " << fname1 << endl;
  */

  // Core system
  if (testName == "null")
    core_null();
  else if (testName == "audiodevices")
    core_audiodevices();
  else if (testName == "isClose")
    core_isClose(fname0, fname1);
  else if (testName == "realvec")
    core_realvec();

  // Basic audio processing
// TODO: move
  else if (testName == "vibrato")
    basic_vibrato(fname0, outputFilename);
  else if (testName == "delay")
    basic_delay(fname0, outputFilename);
  else if (testName == "downsample")
    basic_downsample(fname0, outputFilename);
  else if (testName == "negative")
    basic_negative(fname0, outputFilename);
  else if (testName == "shifter")
    basic_shifter(fname0, outputFilename);
  else if (testName == "shiftInput")
    basic_shiftInput(fname0, outputFilename);
  else if (testName == "windowing")
    basic_windowing(fname0, outputFilename);

  // Analysis
  else if (testName == "pitch")
    analysis_pitch(fname0);
  else if (testName == "transcriber")
    analysis_transcriber(fname0);


  else
  {
    cout << "Unsupported test " << endl;
    printHelp(progName);
  }

  if (answerFilename != EMPTYSTRING)
  {
    core_isClose(outputFilename, answerFilename);
  }
  exit(0);
}

