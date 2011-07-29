/*
** Copyright (C) 2000-2011 George Tzanetakis <gtzan@cs.uvic.ca>
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


//
// carfac_hacking
//
// Run the CARFAC filter on audio data
//
// written by sness (c) 2011 - GPL - sness@sness.net
//

#include <stdlib.h>
#include <cstdio>
#include "Collection.h"
#include "FileName.h"
#include "MarSystemManager.h"
#include "CommandLineOptions.h"
#include "common.h"
#include "CARFAC.h"

#include <vector>
#include <iomanip>

#if defined(__APPLE_CC__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


using namespace std;
using namespace Marsyas;

//
// Global variables for various commandline options
//
mrs_natural helpopt_;
mrs_natural usageopt_;
mrs_natural verboseopt_;
mrs_natural windowSize_;
mrs_natural hopSize_;
mrs_natural memorySize_;
mrs_real gain_;
mrs_natural highFreq_;
mrs_natural lowFreq_;
mrs_natural audioopt_;
mrs_bool summaryopt_;
mrs_real memory_factor_;
mrs_bool summaryitdopt_;

mrs_natural position_;
mrs_natural ticks_;
mrs_natural width_, height_;
MarSystemManager mng;
MarSystem *net;

CommandLineOptions cmd_options;

mrs_realvec summary;
bool initialized = false;


void
printUsage(string progName)
{
	MRSDIAG("carfac_binaural.cpp - printUsage");
	cerr << "Usage : " << progName << " in.wav out.png" << endl;
	cerr << endl;
	cerr << "where : " << endl;
	cerr << "   in.wav is a sound file in a MARSYAS supported format" << endl;
	cerr << "   out.png is the name of the PNG file to be generated" << endl;
	exit(1);
}

void
printHelp(string progName)
{
	MRSDIAG("carfac_binaural.cpp - printHelp");
	cerr << "carfac_binaural" << endl;
	cerr << "-------------------------------------------------------------" << endl;
	cerr << "Generate a PNG of an input audio file.  The PNG can either be" << endl;
	cerr << "the waveform or the spectrogram of the audio file" << endl;
	cerr << endl;
	cerr << "written by sness (c) 2010 GPL - sness@sness.net" << endl;
	cerr << endl;
	cerr << "Usage : " << progName << " in.wav [out.png]" << endl;
	cerr << endl;
	cerr << "where : " << endl;
	cerr << "   in.wav is a sound file in a MARSYAS supported format" << endl;
	cerr << "   out.png is the optional name of the PNG file to be generated" << endl;
	cerr << "Help Options:" << endl;
	cerr << "-u --usage        : display short usage info" << endl;
	cerr << "-h --help         : display this information " << endl;
	cerr << "-v --verbose      : verbose output" << endl;
	cerr << "------------------------------------------" << endl;

	exit(1);
}

void
initOptions()
{
	cmd_options.addBoolOption("help", "h", false);
	cmd_options.addBoolOption("usage", "u", false);
	cmd_options.addBoolOption("verbose", "v", false);
	cmd_options.addNaturalOption("windowsize", "ws", 512);
	cmd_options.addNaturalOption("hopsize", "hs", 256);
	cmd_options.addNaturalOption("memorysize", "ms", 300);
	cmd_options.addRealOption("gain", "g", 1.5);
	cmd_options.addNaturalOption("maxfreq", "mxf", 22050);
	cmd_options.addNaturalOption("minfreq", "mnf", 0);
	cmd_options.addNaturalOption("ticks", "t", -1);
	cmd_options.addNaturalOption("position", "p", 0);
	cmd_options.addBoolOption("audio", "a", false);
	cmd_options.addBoolOption("summary", "s", false);
	cmd_options.addBoolOption("summaryitd", "si", false);
	cmd_options.addRealOption("memory_factor", "m", 0.9);
}


void
loadOptions()
{
	helpopt_ = cmd_options.getBoolOption("help");
	usageopt_ = cmd_options.getBoolOption("usage");
	verboseopt_ = cmd_options.getBoolOption("verbose");
	windowSize_ = cmd_options.getNaturalOption("windowsize");
	memorySize_ = cmd_options.getNaturalOption("memorysize");
	hopSize_ = cmd_options.getNaturalOption("hopsize");
	gain_ = cmd_options.getRealOption("gain");
	highFreq_ = cmd_options.getNaturalOption("maxfreq");
	lowFreq_ = cmd_options.getNaturalOption("minfreq");
	position_ = cmd_options.getNaturalOption("position");
	ticks_ = cmd_options.getNaturalOption("ticks");
	height_ = cmd_options.getNaturalOption("height");
	audioopt_ = cmd_options.getBoolOption("audio");
	summaryopt_ = cmd_options.getBoolOption("summary");
	memory_factor_ = cmd_options.getRealOption("memory_factor");
	summaryitdopt_ = cmd_options.getBoolOption("summaryitd");
}

void carfac_setup(string inAudioFileName)
{
  cout << "input=" << inAudioFileName << endl;

  // Create Marsyas network
  net = mng.create("Series", "net");

  if (audioopt_) {
    net->addMarSystem(mng.create("AudioSource", "src"));
  } else {
    net->addMarSystem(mng.create("SoundFileSource", "src"));
    net->updControl("SoundFileSource/src/mrs_string/filename",inAudioFileName);
    net->addMarSystem(mng.create("AudioSink", "dest"));
  }

  MarSystem* carfac = mng.create("BinauralCARFAC", "carfac");
  net->addMarSystem(carfac);
  net->updControl("BinauralCARFAC/carfac/mrs_real/memory_factor", memory_factor_);

  if (audioopt_) {
    net->updControl("mrs_real/israte", 44100.0);
    net->updControl("AudioSource/src/mrs_natural/nChannels", 2);
    net->updControl("AudioSource/src/mrs_real/gain", 2.0);
    net->updControl("AudioSource/src/mrs_natural/device", 3);
    net->updControl("AudioSource/src/mrs_bool/initAudio", true);
  } else {
    net->updControl("AudioSink/dest/mrs_natural/device", 3);
    net->updControl("AudioSink/dest/mrs_bool/initAudio", true);
  }

  net->updControl("mrs_natural/inSamples", 512);


}

void display(void)
{
  // Tick the network and then display the data that was generated
  net->tick();
  mrs_realvec data = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

  // Stop at the end of the input soundfile
  if ((!audioopt_) && (!net->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())) {
    exit(0);
  }

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  int width = 1000;
  int height = 500;
  double side = 0.01;

  if (summaryopt_) {
    if (!initialized) {
      summary = data;
      initialized = true;
    } else {
      summary += data;
    }
    data = summary;
  }

  double datarows = data.getRows();
  double datacols = data.getCols();

  double max = -99999999.;
  double min = 99999999;
  for (int row = 0; row < datarows; row++) {
    for (int col = 0; col < datacols; col++) {
      double color = data(row,col);
      if (color > max) {
        max = color;
      }
      if (color < min) {
        min = color;
      }
    }
  }

  for (int row = 0; row < datarows; row++) {
    for (int col = 0; col < datacols; col++) {
      double color = data(row,col);
      double normalized_color = 1. - ((color - min) / (max - min));
      double y = ((1. - (row / datarows) - 0.5)) * 1.8;
      double x = ((col / datacols) - 0.5) * 1.8;
      glColor3f(normalized_color,normalized_color,normalized_color);

      glBegin(GL_POLYGON);
      glVertex3f(x+side, y+side,0);
      glVertex3f(x-side, y+side,0);
      glVertex3f(x-side, y-side,0);
      glVertex3f(x+side, y-side,0);
      glEnd();
    }
  }

  glFlush();

  glutSwapBuffers();
}

void reshape(int width, int height)
{
  glViewport(0, 0, width, height);
}

void idle(void)
{
  glutPostRedisplay();
}

int
main(int argc, const char **argv)
{
	MRSDIAG("carfac_binaural.cpp - main");

	string progName = argv[0];
	if (argc == 1)
		printUsage(progName);

	// handling of command-line options
	initOptions();
	cmd_options.readOptions(argc, argv);
	loadOptions();

	vector<string> files = cmd_options.getRemaining();
	if (helpopt_)
		printHelp(progName);

	if (usageopt_)
		printUsage(progName);

    carfac_setup(files[0]);

    glutInit(&argc, (char**) argv);

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(1000, 500);

    (void)glutCreateWindow("GLUT Program");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);

    glutMainLoop();

    exit(0);

}
