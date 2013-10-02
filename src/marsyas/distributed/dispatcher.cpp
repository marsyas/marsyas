//
// File: dispatcher.cpp
//
// Description: Dispatches multiple collections of audio files
// 		to different computers using a seperate thread
// 		for each.
//
// Author: Stuart Bray
// Date: Feb 2005.
//


#include <pthread.h>
#include <iostream>
#include <cstdio>
#include <sstream>
#include <map>

#include <marsyas/Collection.h>
#include "MrsDoctor.h"
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/CommandLineOptions.h>
#include "SocketException.h"

using namespace std;
using namespace Marsyas;

// define our mutex and the soundfile collection
pthread_mutex_t sf_mutex;
fstream* sf_op;

typedef struct thread_data {
  string host;
  int dataPort;
  int controlsPort;
  int thread_id;
};

bool helpOpt = false;
string filenameOpt("dispatch.txt");
string collectionOpt("music.mf");

CommandLineOptions cmd_options;

// thread code
void* run(void * arg)
{

  thread_data* data = (thread_data*) arg;

  MarSystemManager mng;

  // Output destination is a NetworkTCPSink
  NetworkTCPSink* dest = (NetworkTCPSink*) mng.create("NetworkTCPSink", "dest");

  dest->updctrl("mrs_natural/dataPort", data->dataPort);
  dest->updctrl("mrs_natural/controlsPort", data->controlsPort);
  dest->updctrl("mrs_string/host", data->host);

  // create playback network
  MarSystem* playbacknet = mng.create("Series", "playbacknet");
  playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));
  playbacknet->addMarSystem(dest);
  playbacknet->update();

  playbacknet->linkctrl("mrs_string/filename", "SoundFileSource/src/mrs_string/filename");
  playbacknet->linkctrl("mrs_natural/nChannels", "SoundFileSource/src/mrs_natural/nChannels");
  playbacknet->linkctrl("mrs_real/israte", "SoundFileSource/src/mrs_real/israte");
  playbacknet->linkctrl("mrs_natural/pos", "SoundFileSource/src/mrs_natural/pos");
  playbacknet->linkctrl("mrs_bool/hasData", "SoundFileSource/src/mrs_bool/hasData");

  dest->refresh();

  playbacknet->updctrl("mrs_natural/inSamples", 512);

  mrs_natural wc = 0;
  mrs_natural onSamples = playbacknet->getctrl("mrs_natural/onSamples")->to<mrs_natural>();
  char sf_buffer[256];

  while ( true ) {

    // when a thread notices eof, we can quit
    pthread_mutex_lock(&sf_mutex);
    if (sf_op->eof()) {
      cout << "end of file." << endl;
      pthread_mutex_unlock(&sf_mutex);
      break;
    }
    //get another files from the collection
    sf_op->getline(sf_buffer, 256);
    pthread_mutex_unlock(&sf_mutex);

    string soundfile;
    stringstream sfstream(sf_buffer);
    sfstream >> soundfile;

    if ( soundfile.compare("")==0 ) {
      pthread_exit(NULL);
    }

    cout << "Thread: " << data->thread_id << " playing " << soundfile << endl;
    playbacknet->updctrl("SoundFileSource/src/mrs_string/filename", soundfile);
    playbacknet->updctrl("mrs_bool/hasData", true);

    // play the sound
    while ( playbacknet->getctrl("mrs_bool/hasData")->to<mrs_bool>() )
    {
      try {
        playbacknet->tick(); // everything happens here
      } catch (SocketException e) {
        cerr << "SocketException: played - " << wc <<
             " slices of " << onSamples << " samples" << endl;
        exit(1);
      }

      wc ++;
    }
    cout << "Thread: " << data->thread_id << " processed " <<  wc << " slices of "
         << onSamples << " samples" << endl;
    wc = 0;
  }
  pthread_exit( NULL );
}


void initOptions() {
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addStringOption("filename", "f", "dispatch.txt");
  cmd_options.addStringOption("collection", "c", "music.mf");
}


void loadOptions() {
  helpOpt = cmd_options.getBoolOption("help");
  filenameOpt = cmd_options.getStringOption("filename");
  collectionOpt = cmd_options.getStringOption("collection");
}


void printUsage() {
  cout << "Usage: " << endl;
  cout << "dispatcher -f < network.txt > -c < collection.mf >" << endl;
  cout << endl << "NOTE: network.txt is a simple file of the form:" << endl;
  cout << "<host> <dataPort> <controlPort>" << endl;
  cout << "<host> <dataPort> <controlPort> ..." << endl;
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

  int count = 0;
  map< int, pthread_t > threads;
  map< int, thread_data > arguments;

  char buffer[256];

  fstream file_op(filenameOpt.c_str(), ios::in);

  if (! file_op.is_open()) {
    cerr << "Error opening file: " << filenameOpt << endl;
    exit (1);
  }

  int thread_id = 0;
  while ( ! file_op.eof() ) {

    file_op.getline(buffer, 256);

    string host;
    int dataPort, controlsPort;
    stringstream ss(buffer);
    ss >> host >> dataPort >> controlsPort;

    if ( host.compare("") == 0 ) {
      break;
    }

    arguments[count].host = host;
    arguments[count].dataPort = dataPort;
    arguments[count].controlsPort = controlsPort;
    arguments[count].thread_id = ++thread_id;

    // open the file here...
    sf_op = new fstream();
    sf_op->open(collectionOpt.c_str(), ios::in);


    if (! sf_op->is_open()) {
      cerr << "Error opening collection file." << endl;
      exit(1);
    }

    int err = pthread_create(&threads[count], NULL, run, (void*)&arguments[count]);

    count++;

    if (err) {
      cerr << "Problem creating thread." << endl;
      exit(1);
    }
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
  pthread_mutex_destroy(&sf_mutex);
  pthread_exit(NULL);
  return 0;
}

