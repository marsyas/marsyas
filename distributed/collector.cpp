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
#include <stdio.h>
#include <sstream>
#include <map>
#include <vector>

#include "Collection.h"
#include "MrsDoctor.h"
#include "MarSystemManager.h"
#include "CommandLineOptions.h"
#include "SocketException.h"

using namespace std;


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
  
  featureNetwork->updctrl("NetworkTCPSource/src/natural/dataPort", data->dataPort);
  featureNetwork->updctrl("NetworkTCPSource/src/natural/controlsPort", data->controlsPort);

  featureNetwork->linkctrl("bool/notEmpty", "NetworkTCPSource/src/bool/notEmpty");
  
  src->refresh();
  
  natural wc = 0;
  
  real* controls = 0;
  
  natural onSamples = featureNetwork->getctrl("natural/onSamples").toNatural();
  
  
  // start the network 
  while ( featureNetwork->getctrl("bool/notEmpty").toBool() ) {

	try {
		
		controls = featureNetwork->recvControls();	
      		if ( controls != 0 ) {
			
			// get some reference controls, so if they have changed we update them
			natural inSamples = featureNetwork->getctrl("natural/inSamples").toNatural();
			natural inObservations = featureNetwork->getctrl("natural/inObservations").toNatural();
			real israte = featureNetwork->getctrl("real/israte").toReal();
			
			if ( (natural)controls[1] != inSamples || (natural)controls[2] != inObservations 
					|| controls[3] != israte ) {
			
				featureNetwork->updctrl("natural/inSamples", (natural)controls[1]);
				featureNetwork->updctrl("natural/inObservations", (natural)controls[2]);
				featureNetwork->updctrl("real/israte", controls[3]);
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

