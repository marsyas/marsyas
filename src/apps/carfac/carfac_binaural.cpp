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
#include <marsyas/Collection.h>
#include <marsyas/FileName.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/CommandLineOptions.h>
#include <marsyas/common_source.h>
#include <marsyas/marsystems/CARFAC.h>

#include <vector>
#include <iomanip>
#include <sstream>

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
mrs_bool summaryitdopt_;

mrs_real memory_factor_;
mrs_real threshold_alpha_;
mrs_real threshold_jump_factor_;
mrs_real threshold_jump_offset_;

mrs_real orig_memory_factor_;
mrs_real orig_threshold_alpha_;
mrs_real orig_threshold_jump_factor_;
mrs_real orig_threshold_jump_offset_;



mrs_natural position_;
mrs_natural ticks_;
MarSystemManager mng;
MarSystem *net;

CommandLineOptions cmd_options;

mrs_realvec summary;
bool initialized = false;

int width_ = 1000;
int height_ = 500;
int current_summary_pos_ = 0;
vector<vector<double> > summary_itd_;

int summary_itd_width_ = 200;
int summary_itd_height_ = 96;

double nap_max = -99999999.;
double nap_min = 99999999;


mrs_realvec data_;
mrs_realvec waveform_data_;
mrs_realvec nap_data_;
mrs_realvec threshold_data_;
mrs_realvec strobes_data_;


int visualize_channel_ = 50;
int current_time_;
int last_time_;
int iteration = 0;

double fps_;
bool freeze_ = false;
bool key_update_ = false;


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
  cmd_options.addBoolOption("audio", "a", false);
  cmd_options.addBoolOption("summary", "s", false);
  cmd_options.addBoolOption("summaryitd", "si", false);
  cmd_options.addRealOption("memory_factor", "m", 0.9);
  cmd_options.addRealOption("threshold_alpha", "ta", 0.9999);
  cmd_options.addRealOption("threshold_jump_factor", "tjf", 1.5);
  cmd_options.addRealOption("threshold_jump_offset", "tjo", 0.01);
}


void
loadOptions()
{
  helpopt_ = cmd_options.getBoolOption("help");
  usageopt_ = cmd_options.getBoolOption("usage");
  verboseopt_ = cmd_options.getBoolOption("verbose");
  windowSize_ = cmd_options.getNaturalOption("windowsize");
  hopSize_ = cmd_options.getNaturalOption("hopsize");
  audioopt_ = cmd_options.getBoolOption("audio");
  summaryopt_ = cmd_options.getBoolOption("summary");
  memory_factor_ = cmd_options.getRealOption("memory_factor");
  summaryitdopt_ = cmd_options.getBoolOption("summaryitd");
  threshold_alpha_ = cmd_options.getRealOption("threshold_alpha");
  threshold_jump_factor_ = cmd_options.getRealOption("threshold_jump_factor");
  threshold_jump_offset_ = cmd_options.getRealOption("threshold_jump_offset");

  orig_memory_factor_ = memory_factor_;
  orig_threshold_alpha_ = threshold_alpha_;
  orig_threshold_jump_factor_ = threshold_jump_factor_;
  orig_threshold_jump_offset_ = threshold_jump_offset_;
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

  net->addMarSystem(mng.create("Gain", "gain"));

  MarSystem* carfac = mng.create("CARFAC", "carfac");
  net->addMarSystem(carfac);
  net->updControl("CARFAC/carfac/mrs_bool/calculate_binaural_sai", true);
  net->updControl("CARFAC/carfac/mrs_real/sai_memory_factor", memory_factor_);
  net->updControl("CARFAC/carfac/mrs_real/sai_threshold_alpha", threshold_alpha_);
  net->updControl("CARFAC/carfac/mrs_real/sai_threshold_jump_factor", threshold_jump_factor_);
  net->updControl("CARFAC/carfac/mrs_real/sai_threshold_jump_offset", threshold_jump_offset_);

  if (audioopt_) {
    net->updControl("mrs_real/israte", 44100.0);
    net->updControl("AudioSource/src/mrs_natural/nChannels", 2);
    net->updControl("AudioSource/src/mrs_real/gain", 2.0);
    net->updControl("AudioSource/src/mrs_natural/device", 3);
    net->updControl("AudioSource/src/mrs_bool/initAudio", true);
  } else {
    net->updControl("AudioSink/dest/mrs_natural/device", 0);
    net->updControl("AudioSink/dest/mrs_bool/initAudio", true);
  }

  net->updControl("mrs_natural/inSamples", 512);
}

void update_summary_itd(mrs_realvec data)
{
  if (!freeze_) {
    double datarows = data.getRows();
    double datacols = data.getCols();

    for (int row = 0; row < datarows; row++) {
      summary_itd_[row][current_summary_pos_] = 0;
      for (int col = 0; col < datacols; col++) {
        summary_itd_[row][current_summary_pos_] += data(row,col);
      }
    }

    current_summary_pos_++;
    if (current_summary_pos_ > summary_itd_width_) {
      current_summary_pos_ = 0;
    }

  }
}

void drawGLString(double x, double y, double r, double g, double b, string s) {

  glColor4f(r, g, b, 1.0f);
  glRasterPos2f(x,y);

  void * font = GLUT_BITMAP_HELVETICA_18;
  for (string::iterator i = s.begin(); i != s.end(); ++i)
  {
    char c = *i;
    glutBitmapCharacter(font, c);
  }

}

void display(void)
{
  // Tick the network and then display the data that was generated
  if (key_update_ || !freeze_) {
    net->tick();
    data_ = net->getctrl("CARFAC/carfac/mrs_realvec/sai_output_binaural_sai")->to<mrs_realvec>();
    waveform_data_ = net->getctrl("Gain/gain/mrs_realvec/processedData")->to<mrs_realvec>();
    strobes_data_ = net->getctrl("CARFAC/carfac/mrs_realvec/sai_output_strobes")->to<mrs_realvec>();
    nap_data_ = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
    threshold_data_ = net->getctrl("CARFAC/carfac/mrs_realvec/sai_output_threshold")->to<mrs_realvec>();
  }

  // cout << "data=" << data << endl;
  // exit(0);


  // Stop at the end of the input soundfile
  if ((!audioopt_) && (!net->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())) {
    exit(0);
  }

  update_summary_itd(data_);


  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  double side = 0.01;

  if (summaryopt_) {
    if (!initialized) {
      summary = data_;
      initialized = true;
    } else {
      summary += data_;
    }
    data_ = summary;
  }

  double datarows = data_.getRows();
  double datacols = data_.getCols();

  double max = -99999999.;
  double min = 99999999;
  for (int row = 0; row < datarows; row++) {
    for (int col = 0; col < datacols; col++) {
      double color = data_(row,col);
      if (color > max) {
        max = color;
      }
      if (color < min) {
        min = color;
      }
    }
  }

  last_time_ = current_time_;
  current_time_ = glutGet(GLUT_ELAPSED_TIME);

  if (iteration % 10 == 0) {
    fps_ = 1.0 / ((current_time_ - last_time_) / 1000.);
  }

  iteration++;

  for (int row = 0; row < datarows; row++) {
    for (int col = 0; col < datacols; col++) {
      double color = data_(row,col);
      double normalized_color = 1. - ((color - min) / (max - min));
      double y = (((1. - (row / datarows) - 0.5)) * 1.1) + 0.4;
      double x = ((col / datacols) - 0.5) * 1.8;

      if (col < datacols / 2) {
        glColor3f(normalized_color,normalized_color/1.5,normalized_color/1.5);
      } else {
        glColor3f(normalized_color/1.5,normalized_color,normalized_color/1.5);
      }

      glBegin(GL_POLYGON);
      glVertex3f(x+side, y+side,0);
      glVertex3f(x-side, y+side,0);
      glVertex3f(x-side, y-side,0);
      glVertex3f(x+side, y-side,0);
      glEnd();
    }
  }

  //
  // Waveform data
  //
  glColor3f(1.0,0.5,0.5);
  glBegin(GL_LINE_STRIP);
  for (int col = 0; col < waveform_data_.getCols(); col++) {
    double x = ((col / 100.) * 0.3) - 0.95;
    double y = (waveform_data_(0,col) / 20.0) - 0.22;
    glVertex3f(x,y,0);
  }
  glEnd();

  glColor3f(0.5,1.0,0.5);
  glBegin(GL_LINE_STRIP);
  for (int col = 0; col < waveform_data_.getCols(); col++) {
    double x = ((col / 100.) * 0.3) - 0.95;
    double y = (waveform_data_(1,col) / 20.0) - 0.22;
    glVertex3f(x,y,0);
  }
  glEnd();


  //
  // Summary ITD
  //
  double littleside_x = 0.01;
  double littleside_y = 0.02;
  double summary_itd_max = -99999999.;
  double summary_itd_min = 99999999;

  for (unsigned int row = 0; row < summary_itd_.size(); row++) {
    for (unsigned int col = 0; col < summary_itd_[0].size(); col++) {
      double color = summary_itd_[row][col];
      if (color > summary_itd_max) {
        summary_itd_max = color;
      }
      if (color < summary_itd_min) {
        summary_itd_min = color;
      }
    }
  }

  for (unsigned int row = 0; row < summary_itd_.size(); row++) {
    for (unsigned int col = 0; col < summary_itd_[0].size(); col++) {
      int curr_col = (current_summary_pos_ + col) % summary_itd_[0].size();
      // if (curr_col > summary_itd_[0].size()) {
      //   curr_col = current_summary_pos_ - col;
      // }
      // curr_col = 0;
      double color = summary_itd_[row][curr_col];
      double normalized_color = 1. - ((color - summary_itd_min) / (summary_itd_max - summary_itd_min));
      // snessnet(TODO) - These parameters were all picked emperically
      // to make the picture fit.
      double y = (((1. - (row / datarows) - 0.5)) * 0.4) - 0.5;
      double x = (((col / datacols) - 0.5) * 1.52) - 0.2;
      glColor3f(normalized_color,normalized_color,normalized_color);

      glBegin(GL_POLYGON);
      glVertex3f(x+littleside_x, y+littleside_y,0);
      glVertex3f(x-littleside_x, y+littleside_y,0);
      glVertex3f(x-littleside_x, y-littleside_y,0);
      glVertex3f(x+littleside_x, y-littleside_y,0);
      glEnd();

    }
  }



  //
  // Threshold graphs
  //
  double nap_data_rows = nap_data_.getRows();
  double nap_data_cols = nap_data_.getCols();

  // double nap_max = -99999999.;
  // double nap_min = 99999999;

  // Decay the maximum plotted value so outliers don't screw us
  // completely.
  // nap_max *= 0.99;
  // nap_min *= 0.99;
  nap_max = -99999999.;
  nap_min = 99999999.;
  for (int col = 0; col < nap_data_cols; col++) {
    double val = nap_data_(visualize_channel_,col);
    if (val > nap_max) {
      nap_max = val;
    }
    if (val < nap_min) {
      nap_min = val;
    }

    val = threshold_data_(visualize_channel_,col);
    if (val > nap_max) {
      nap_max = val;
    }
    if (val < nap_min) {
      nap_min = val;
    }
  }

  if (freeze_) {

    // Nap data
    glColor3f(0.8,0.9,0.8);
    glBegin(GL_LINE_STRIP);
    for (int col = 0; col < nap_data_cols; col++) {
      double x = ((col / 100.) * 0.3) - 0.955;
      double y = (((nap_data_(visualize_channel_,col) - nap_min) / (nap_max - nap_min)) / 5.) - 0.95;
      glVertex3f(x,y,0);
    }
    glEnd();

    // Threshold data
    double threshold_data_rows = threshold_data_.getRows();
    double threshold_data_cols = threshold_data_.getCols();

    glColor3f(0.95,0.35,0.35);
    glBegin(GL_LINE_STRIP);
    for (int col = 0; col < threshold_data_cols; col++) {
      double x = ((col / 100.) * 0.3) - 0.955;
      double y = (((threshold_data_(visualize_channel_,col) - nap_min) / (nap_max - nap_min)) / 5.) - 0.95;
      glVertex3f(x,y,0);
    }
    glEnd();


    // Strobes data
    glColor3f(1.0,1.0,0.0);
    glBegin(GL_LINES);
    // cout << "strobes ";
    for (int col = 0; col < strobes_data_.getCols(); col++) {
      if (strobes_data_(visualize_channel_,col)) {
        // cout << col << " ";
        double x = ((col / 100.) * 0.3) - 0.955;
        glVertex3f(x,-0.97,0);
        glVertex3f(x,-0.80,0);
      }
    }
    // cout << endl;
    glEnd();
  }

  // Current parameters


  stringstream outstr;
  outstr << setprecision (3);
  outstr << "memory " << memory_factor_;
  drawGLString(0.65,-0.22,1,1,1,outstr.str());

  outstr.str(std::string());
  outstr << "alpha " << threshold_alpha_;
  drawGLString(0.65,-0.27,1,1,1,outstr.str());

  outstr.str(std::string());
  outstr << "jump_factor " << threshold_jump_factor_;
  drawGLString(0.65,-0.32,1,1,1,outstr.str());

  outstr.str(std::string());
  outstr << "jump_offset " << threshold_jump_offset_;
  drawGLString(0.65,-0.37,1,1,1,outstr.str());

  outstr.str(std::string());
  outstr << "channel " << visualize_channel_;
  drawGLString(0.65,-0.42,1,1,1,outstr.str());



  // Help functions
  outstr.str(std::string());
  outstr << "parameters";
  drawGLString(0.65,-0.5,1,1,0,outstr.str());

  outstr.str(std::string());
  outstr << "memory => q/a";
  drawGLString(0.65,-0.55,1,1,0,outstr.str());

  outstr.str(std::string());
  outstr << "alpha => w/s";
  drawGLString(0.65,-0.60,1,1,0,outstr.str());

  outstr.str(std::string());
  outstr << "jump factor => e/d";
  drawGLString(0.65,-0.65,1,1,0,outstr.str());

  outstr.str(std::string());
  outstr << "jump offset => r/f";
  drawGLString(0.65,-0.70,1,1,0,outstr.str());

  outstr.str(std::string());
  outstr << "channel => -/+";
  drawGLString(0.65,-0.75,1,1,0,outstr.str());

  outstr.str(std::string());
  outstr << "reset => z";
  drawGLString(0.65,-0.80,1,1,0,outstr.str());

  // FPS

  outstr.str(std::string());
  outstr << "fps = " << (int)fps_;
  drawGLString(0.65,-0.95,1,0,0,outstr.str());



  glFlush();

  glutSwapBuffers();


  key_update_ = false;
  // sleep(1);
}



void reshape(int width, int height)
{
  glViewport(0, 0, width, height);
}

void idle(void)
{
  glutPostRedisplay();
}

void keyPressed (unsigned char key, int x, int y) {
  bool update_strobes = false;
  if (key == ' ') {
    freeze_ = !freeze_;
  }
  if (key == '-' || key == '_') {
    visualize_channel_--;
    if (visualize_channel_ < 0) {
      visualize_channel_ = 0;
    }
  }
  if (key == '+' || key == '=') {
    visualize_channel_++;
    if (visualize_channel_ > 95) {
      visualize_channel_ = 95;
    }
  }

  if (key == 'q') {
    memory_factor_ *= 1.001;
    if (memory_factor_ > 1.0) {
      memory_factor_ = 1;
    }
  }

  if (key == 'a') {
    memory_factor_ *= 0.999;
    if (memory_factor_ < 0.1) {
      memory_factor_ = 0.1;
    }
  }

  // threshold_alpha

  if (key == 'w') {
    threshold_alpha_ *= 1.001;
    if (threshold_alpha_ > 1.0) {
      threshold_alpha_ = 1;
    }
    key_update_ = true;
  }

  if (key == 's') {
    threshold_alpha_ *= 0.999;
    if (threshold_alpha_ < 0.1) {
      threshold_alpha_ = 0.1;
    }
    key_update_ = true;
  }

  // threshold_alpha

  if (key == 'e') {
    threshold_jump_factor_ *= 1.1;
    if (threshold_jump_factor_ > 100.0) {
      threshold_jump_factor_ = 100;
    }
    key_update_ = true;
  }

  if (key == 'd') {
    threshold_jump_factor_ *= 0.9;
    if (threshold_jump_factor_ < 0.1) {
      threshold_jump_factor_ = 0.1;
    }
    key_update_ = true;
  }
  // threshold_alpha

  if (key == 'r') {
    threshold_jump_offset_ *= 1.1;
    if (threshold_jump_offset_ > 100.0) {
      threshold_jump_offset_ = 100;
    }
    key_update_ = true;
  }

  if (key == 'f') {
    threshold_jump_offset_ *= 0.9;
    if (threshold_jump_offset_ < 0.1) {
      threshold_jump_offset_ = 0.1;
    }
    key_update_ = true;
  }

  if (key == 'z') {
    memory_factor_ = orig_memory_factor_;
    threshold_alpha_ = orig_threshold_alpha_;
    threshold_jump_factor_ = orig_threshold_jump_factor_;
    threshold_jump_offset_ = orig_threshold_jump_offset_;
    key_update_ = true;
  }

  net->updControl("CARFAC/carfac/mrs_real/sai_memory_factor", memory_factor_);
  net->updControl("CARFAC/carfac/mrs_real/sai_threshold_alpha", threshold_alpha_);
  net->updControl("CARFAC/carfac/mrs_real/sai_threshold_jump_factor", threshold_jump_factor_);
  net->updControl("CARFAC/carfac/mrs_real/sai_threshold_jump_offset", threshold_jump_offset_);


}


int
main(int argc, const char **argv)
{

  // Make data structures for summary_itd_
  summary_itd_.resize(summary_itd_height_);
  for (int i = 0; i < summary_itd_height_; i++) {
    summary_itd_[i].resize(summary_itd_width_);
  }

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
  cout << "width_=" << width_ << endl;
  cout << "height_=" << height_ << endl;
  glutInitWindowSize(width_,height_+500);

  (void)glutCreateWindow("GLUT Program");
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutIdleFunc(idle);
  glutKeyboardFunc(keyPressed);

  current_time_ = glutGet(GLUT_ELAPSED_TIME);
  last_time_ = current_time_;

  glutMainLoop();

  exit(0);

}
