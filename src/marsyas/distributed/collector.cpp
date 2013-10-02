//
// File: collector.cpp
//
// Description: Collects audio feature data from multiple clients, spawing a
// 		seperate thread to handle each connection.
//
// Author: Stuart Bray
// Date: Feb 2005.
//


#include <pthread.h>
#include <iostream>
#include <cstdio>
#include <sstream>
#include <map>
#include <vector>

#include <marsyas/Collection.h>
#include "MrsDoctor.h"
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/CommandLineOptions.h>
#include "SocketException.h"

using namespace std;
using namespace Marsyas;


CommandLineOptions cmd_options;
string filenameOpt("collect.txt");
bool helpOpt;


typedef struct thread_data {
  int dataPort;
  int controlsPort;
};


// thread code
void* run(void * arg)
{

  thread_data* data = (thread_data*) arg;

  MarSystemManager mng;
  NetworkTCPSource* src = new NetworkTCPSource("src");

  MarSystem* featureNetwork = mng.create("Series", "featureNetwork");
  featureNetwork->addMarSystem(src);
  featureNetwork->addMarSystem(mng.create("AudioSink", "sink"));
  // featureNetwork->addMarSystem(mng.create("PlotSink", "psink"));

  featureNetwork->updctrl("NetworkTCPSource/src/mrs_natural/dataPort", data->dataPort);
  featureNetwork->updctrl("NetworkTCPSource/src/mrs_natural/controlsPort", data->controlsPort);

  featureNetwork->linkctrl("mrs_bool/hasData", "NetworkTCPSource/src/mrs_bool/hasData");

  src->refresh();

  mrs_natural wc = 0;

  mrs_real* controls = 0;

  mrs_natural onSamples = featureNetwork->getctrl("mrs_natural/onSamples")->to<mrs_natural>();


  // start the network
  while ( featureNetwork->getctrl("mrs_bool/hasData")->to<mrs_bool>() ) {

    try {

      controls = featureNetwork->recvControls();
      if ( controls != 0 ) {

        // get some reference controls, so if they have changed we update them
        mrs_natural inSamples = featureNetwork->getctrl("mrs_natural/inSamples")->to<mrs_natural>();
        mrs_natural inObservations = featureNetwork->getctrl("mrs_natural/inObservations")->to<mrs_natural>();
        mrs_real israte = featureNetwork->getctrl("mrs_real/israte")->to<mrs_real>();

        if ( (mrs_natural)controls[1] != inSamples || (mrs_natural)controls[2] != inObservations
             || controls[3] != israte ) {

          featureNetwork->updctrl("mrs_natural/inSamples", (mrs_natural)controls[1]);
          featureNetwork->updctrl("mrs_natural/inObservations", (mrs_natural)controls[2]);
          featureNetwork->updctrl("mrs_real/israte", controls[3]);
        }
      }

      featureNetwork->tick(); // everything happens here
    } catch ( SocketException e ) {
      cerr << "Played " << wc << " slices of " << onSamples << " samples" << endl;
      pthread_exit(NULL);
    }
    wc++;
  }
  cout << "played - " << wc << " slices of " << onSamples << " samples" << endl;

  pthread_exit( NULL );
}


void initOptions() {
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addStringOption("filename", "f", "collect.txt");
}


void loadOptions() {
  helpOpt = cmd_options.getBoolOption("help");
  filenameOpt = cmd_options.getStringOption("filename");
}


void printUsage() {
  cout << "Usage: " << endl;
  cout << "collector -f < filename >" << endl;
  cout << endl << "NOTE: the file you pass to the collector is a simple" << endl
       << "textfile of the form:" << endl;
  cout << "data: 5009 controls: 5010\ndata: 5011 controls: 5012\n ... " << endl;
}

// main thread
int main(int argc, const char **argv)
{

  if (argc == 1) {
    printUsage();
    exit(0);
  }

  initOptions();
  cmd_options.readOptions(argc, argv);
  loadOptions();

  map< int, pthread_t > threads;
  map< int, thread_data > arguments;

  char buffer[256];
  fstream file_op(filenameOpt.c_str(), ios::in);


  if ( !file_op.is_open() ) {
    cerr << "Could not find file: " << filenameOpt << endl;
    exit(1);
  }

  int count = 0;

  while ( ! file_op.eof() ) {

    file_op.getline(buffer,256);

    int dataPort, controlsPort;
    string dString, cString;
    stringstream ss(buffer);
    ss >> dString >> dataPort >> cString >> controlsPort;

    if ( dString.compare("") == 0 ) {
      break;
    }

    arguments[count].dataPort = dataPort;
    arguments[count].controlsPort = controlsPort;

    int err = pthread_create(&threads[count], NULL, run, (void*)&arguments[count] );

    if (err) {
      cerr << "Problem creating thread." << endl;
      exit(1);
    }
    count++;
  }

  file_op.close();

  int **status = 0;
  int size = threads.size();

  // collect threads as they exit
  for (int i = 0; i < size; i++) {

    // wait for thread to terminate
    int err = pthread_join( threads[i], (void**)status );

    if ( err ) {
      cerr << "problem joining thread " << i << endl;
      exit(1);
    }

    if ( status != NULL ) {
      cerr << "thread " << i << " terminated abnormally." << endl;
      exit(1);
    }
  }
  return 0;
}

